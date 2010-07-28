// This contains the giant switch core that will run the lorito microcode.

#include "lorito.h"
#include "microcode.h"
#include "interp.h"
#include "pmc.h"

#include <stdio.h>
#include <string.h>

#define INVALID_OP(s) _opcode_error(s, regtype); *pc = -1;\
  continue;

#define $I(x) (reg->regs_i[x])
#define $N(x) (reg->regs_n[x])
#define $S(x) (reg->regs_s[x])
#define $P(x) (reg->regs_p[x])

#define $imm op->immediate

void
_opcode_error(char *op, int typed)
{
  char *s;
  switch(typed)
  {
    case OP_PMC:
      s = "pmc";
      break;
    case OP_STR:
      s = "str";
      break;
    case OP_NUM:
      s = "num";
      break;
    case OP_INT:
      s = "int";
      break;
    default:
      s = "unknown";
      break;
  }
  fprintf(stderr, "Invalid opcode: %s_%s\n", s, op);
}

void
core_exec(Lorito_Interp *interp)
{
  Lorito_Ctx *ctx = interp->ctx;
  Lorito_Reg *reg = &ctx->regs;
  int *pc = &ctx->pc;

  while (*pc >= 0)
  {
    Lorito_Codeseg *codeseg = ctx->current_codeseg;
    if (*pc >= codeseg->length)
      return;
    Lorito_Opcode *op = &ctx->current_codeseg->op[*pc];
    int regtype = REG_OF_OP(op->opcode);
    int opcode  =  OP_OF_OP(op->opcode);

    //printf("PC: %d\n", *pc);
    //printf("Opcode: %d\n", opcode);

    switch (opcode)
    {
      case OP_noop:
        break;
      case OP_end:
        *pc = -1;
        continue;
        break;
      case OP_add:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) + $I(op->src2);
            break;
          default:
            INVALID_OP("add");
        }
        break;
      case OP_sub:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) - $I(op->src2);
            break;
          default:
            INVALID_OP("sub");
        }
        break;
      case OP_mul:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) * $I(op->src2);
            break;
          default:
            INVALID_OP("mul");
        }
        break;
      case OP_div:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) / $I(op->src2);
            break;
          default:
            INVALID_OP("div");
        }
        break;
      case OP_mod:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) % $I(op->src2);
            break;
          default:
            INVALID_OP("mod");
        }
        break;
      case OP_and:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) & $I(op->src2);
            break;
          default:
            INVALID_OP("and");
        }
        break;
      case OP_or:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) | $I(op->src2);
            break;
          default:
            INVALID_OP("or");
        }
        break;
      case OP_xor:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) ^ $I(op->src2);
            break;
          default:
            INVALID_OP("xor");
        }
        break;
      case OP_not:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = ~ $I(op->src1);
            break;
          default:
            INVALID_OP("not");
        }
        break;
      case OP_shl:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) << $I(op->src2);
            break;
          default:
            INVALID_OP("shl");
        }
        break;
      case OP_shr:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) >> $I(op->src2);
            break;
          default:
            INVALID_OP("shr");
        }
        break;
      case OP_iseq:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) == $I(op->src2);
            break;
          default:
            INVALID_OP("iseq");
        }
        break;
      case OP_isgt:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) > $I(op->src2);
            break;
          default:
            INVALID_OP("isgt");
        }
        break;
      case OP_isge:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $I(op->src1) >= $I(op->src2);
            break;
          default:
            INVALID_OP("isge");
        }
        break;
      case OP_goto:
        *pc = $imm;
        continue;
      case OP_if:
        switch (regtype)
        {
          case OP_INT:
            if ($I(op->src1) != 0)
            {
              *pc = $imm;
              continue;
            }
            break;
          default:
            INVALID_OP("if");
        }
        break;
      case OP_set:
        switch (regtype)
        {
          case OP_PMC:
            $P(op->dest) = $P(op->src1);
            break;
          case OP_STR:
            $S(op->dest).size = $S(op->src1).size;
            $S(op->dest).data = $S(op->src1).data;
            break;
          case OP_NUM:
            $N(op->dest) = $N(op->src1);
            break;
          case OP_INT:
            $I(op->dest) = $I(op->src1);
            break;
          default:
            INVALID_OP("set");
        }
        break;
      case OP_load_const:
        ;  // C oddity that stops the declare below working
        Lorito_Dataseg *data = ctx->current_dataseg;

        switch (regtype)
        {
          // Read src2 bytes from the data block at offset src1+imm
          case OP_STR:
            {
              unsigned int offset = $imm + $I(op->src1);

              int length = $I(op->src2) == 0 
                  ? strlen(&((char *)data->data)[offset])+1
                  : $I(op->src2)
                ;

              if (offset+length > data->length)
              {
                INVALID_OP("load_const: outside range");
              }

              $S(op->dest).size = length;
              $S(op->dest).data = malloc(sizeof(char) * length);
              strcpy($S(op->dest).data, &((char *) data->data)[offset]);
              break;
            }
          case OP_INT:
            {
              int length = $I(op->src2) == 0 ? sizeof(int) : $I(op->src2);

              unsigned int offset = $imm + $I(op->src1);

              if (length > sizeof(int))
              {
                INVALID_OP("load_const: length too long");
              }
              if (offset+length >= data->length)
              {
                INVALID_OP("load_const: outside range");
              }
              $I(op->dest) = ((int *) data->data)[offset];
              $I(op->dest) &= (~0 >> ((4 - length) * 8));
              break;
            }
          default:
            INVALID_OP("load_const");
        }
        break;
      case OP_load_imm:
        switch (regtype)
        {
          case OP_INT:
            $I(op->dest) = $imm;
            break;
          default:
            INVALID_OP("load_imm");
        }
        break;
      case OP_new:
        switch (regtype)
        {
          case OP_INT:
            $P(op->dest) = lorito_pmc_init(interp, $imm);
            break;
          case OP_PMC:
            $P(op->dest) = lorito_pmc_init(interp, $P(op->src1)->size);
            break;
          default:
            INVALID_OP("new");
        }
        break;
      case OP_store:
        // dest = PMC to store to, src1 = src, src2 = offset, imm = size
        switch (regtype)
        {
          case OP_PMC:
            {
              int length = sizeof(int);
              unsigned int offset = $I(op->src2);

              if ($P(op->dest) == NULL)
              {
                INVALID_OP("store: cannot store to null pmc");
              }
              if (offset+length >= $P(op->dest)->size)
              {
                INVALID_OP("store: outside range");
              }

              // Encode the PMC address into memory
              Lorito_PMC *output = lorito_pmc_encode(interp, $P(op->dest), $I(op->src2), $P(op->src1));

              if ((output == NULL) || (output != $P(op->dest)))
              {
                INVALID_OP("store: error saving PMC");
              }
              break;
            }
          case OP_NUM:
            {
              int length = $imm == 0 ? sizeof(double) : $imm;

              unsigned int offset = $I(op->src2);

              if ($P(op->dest) == NULL)
              {
                INVALID_OP("store: cannot store to null pmc");
              }
              if (length > sizeof(double))
              {
                INVALID_OP("store: length too long");
              }
              if (offset+length >= $P(op->dest)->size)
              {
                INVALID_OP("store: outside range");
              }

              memcpy(&$P(op->dest)->data[offset], &$N(op->src1), length);
              break;
            }
          case OP_INT:
            {
              int length = $imm == 0 ? sizeof(int) : $imm;

              unsigned int offset = $I(op->src2);

              if ($P(op->dest) == NULL)
              {
                INVALID_OP("store: cannot store to null pmc");
              }
              if (length > sizeof(int))
              {
                INVALID_OP("store: length too long");
              }
              if (offset+length >= $P(op->dest)->size)
              {
                INVALID_OP("store: outside range");
              }

              memcpy(&$P(op->dest)->data[offset], &$I(op->src1), length);
              break;
            }
          default:
            INVALID_OP("store");
        }
        break;
      case OP_load:
        // dest = dest, src1 = PMC to load from, src2 = offset, imm = size
        switch (regtype)
        {
          case OP_PMC:
            {
              int length = sizeof(int);
              unsigned int offset = $I(op->src2);

              // If the src PMC is null, set dest to null and be done.
              if ($P(op->src1) == NULL)
              {
                $P(op->dest) = NULL;
                break;
              }
              if (offset+length >= $P(op->src1)->size)
              {
                INVALID_OP("load: outside range");
              }

              // Decode the PMC address into memory
              $P(op->dest) = lorito_pmc_decode(interp, $P(op->src1), $I(op->src2));

              break;
            }
          case OP_NUM:
            {
              int length = $imm == 0 ? sizeof(double) : $imm;

              unsigned int offset = $I(op->src2);

              if ($P(op->src1) == NULL)
              {
                INVALID_OP("load: cannot load from a null pmc");
              }
              if (length > sizeof(double))
              {
                INVALID_OP("load: length too long");
              }
              if (offset+length >= $P(op->dest)->size)
              {
                INVALID_OP("load: outside range");
              }

              memcpy(&$N(op->dest), &$P(op->src1)[offset], length);
              break;
            }
          case OP_INT:
            {
              int length = $imm == 0 ? sizeof(int) : $imm;

              unsigned int offset = $I(op->src2);

              if ($P(op->src1) == NULL)
              {
                INVALID_OP("load: cannot load from a null pmc");
              }
              if (length > sizeof(int))
              {
                INVALID_OP("load: length too long");
              }
              if (offset+length > $P(op->src1)->size)
              {
                INVALID_OP("load: outside range");
              }

              memcpy(&$I(op->dest), &$P(op->src1)->data[offset], length);
              break;
            }
          default:
            INVALID_OP("load");
        }
        break;
      case OP_call:
        break;
      case OP_loadlib:
        break;
      case OP_read:
        break;
      case OP_write:
        switch (regtype)
        {
          case OP_STR:
            fprintf(stdout, "%s", $S(op->src1).data);
            break;
          case OP_INT:
            fprintf(stdout, "%d", $I(op->src1));
            break;
          default:
            INVALID_OP("write");
        }
        break;
      case OP_say:
        switch (regtype)
        {
          case OP_STR:
            fprintf(stdout, "%s\n", $S(op->src1).data);
            break;
          case OP_INT:
            fprintf(stdout, "%d\n", $I(op->src1));
            break;
          default:
            INVALID_OP("say");
        }
        break;
      case OP_gripe:
        switch (regtype)
        {
          case OP_STR:
            fprintf(stderr, "%s", $S(op->src1).data);
            break;
          case OP_INT:
            fprintf(stderr, "%d", $I(op->src1));
            break;
          default:
            INVALID_OP("gripe");
        }
        break;
      case OP_hcf:
        break;
      default:
        // Error
        INVALID_OP("unknown");
        break;
    }
    
    (*pc)++;
  }
}
