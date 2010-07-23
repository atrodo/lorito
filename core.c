// This contains the giant switch core that will run the lorito microcode.

#include "lorito.h"
#include "microcode.h"
#include "interp.h"

#include <stdio.h>

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

    printf("PC: %d\n", *pc);
    printf("Opcode: %d\n", opcode);

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
        break;
      case OP_load_const:
        switch (regtype)
        {
          // Read src2 bytes from the data block at offset src1+imm
          case OP_INT:
            ;  // C oddity that stops the declare below working
            Lorito_Dataseg *data = ctx->current_dataseg;

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
          default:
            INVALID_OP("write");
        }
        break;
      case OP_gripe:
        switch (regtype)
        {
          case OP_STR:
            fprintf(stderr, "%s", $S(op->src1).data);
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
