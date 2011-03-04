// This contains the giant switch core that will run the lorito microcode.

#include "lorito.h"
#include "microcode.h"
#include "interp.h"
#include "pmc.h"
#include "internal_pmc.h"
#include "pmc_func.h"

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

  Lorito_Codeseg *codeseg = NULL; //ctx->current_codeseg;
  int ctx_chgd = 1;

  while (*pc >= 0)
  {
    if (ctx_chgd == 1)
    {
      Lorito_Ctx *old_ctx = ctx;

      ctx = interp->ctx;
      //printf("typed: %d\n", ctx->pmc.magic);
      //printf("typed: %d\n", ((Lorito_PMC *)ctx)->magic);

      if (IS_C_METHOD(ctx->current_codeseg))
      {
        ((Lorito_C_Method *) ctx->current_codeseg)->method(interp, ctx);
        interp->ctx = old_ctx;
        continue;
      }

      if (!IS_CODE_BLOCK(ctx->current_codeseg))
      {
        fprintf(stderr, "Current Code Block for Context is not a Code Block\n");
      }

      reg = &ctx->regs;
      pc = &ctx->pc;

      codeseg = (Lorito_Codeseg *) ctx->current_codeseg;
      ctx_chgd = 0;
      //printf("SegNum:  %d\n", codeseg->segid);
      //printf("SegName: %s\n", codeseg->name);

      if (IS_LOOKUP(((Lorito_PMC *) old_ctx)))
      {
        Lorito_Opcode *op = &codeseg->op[*pc];
        int regtype = REG_OF_OP(op->opcode);
        int opcode  =  OP_OF_OP(op->opcode);

        if (opcode != OP_lookup)
        {
          // Shouldn't happen, but
          INVALID_OP("lookup: lookup did not return to lookup opcode");
        }

        $P(op->dest) = lorito_pop_arg(interp, old_ctx);
        if (!IS_CODE($P(op->dest)) && $P(op->dest) != null)
        {
          INVALID_OP("lookup: lookup did not return code block");
        }
        (*pc)++;
      }

    }

    if (*pc >= codeseg->length)
    {
      interp->ctx = ctx->next_ctx;
      if ((interp->ctx == null) || (interp->ctx->current_codeseg == null))
      {
        return;
      }
      ctx_chgd = 1;
      continue;
    }
    Lorito_Opcode *op = &codeseg->op[*pc];
    int regtype = REG_OF_OP(op->opcode);
    int opcode  =  OP_OF_OP(op->opcode);

    //printf("PC:     %d\n", *pc);
    //printf("Opcode: %d\n", opcode);

    switch (opcode)
    {
      // BEGIN CORE_OPS
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
          case OP_STR:
            $I(op->dest) = $S(op->src1) == $S(op->src2);
            break;
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
            $S(op->dest) = $S(op->src1);
            //$S(op->dest)->data = $S(op->src1)->data;
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
        Lorito_Constseg *consts = ctx->current_constseg;

        switch (regtype)
        {
          // Read src2 bytes from the const block at offset src1+imm
          case OP_STR:
            {
              unsigned int offset = $imm + $I(op->src1);

              int length = $I(op->src2) == 0 
                  ? strlen(&((char *)consts->data)[offset])+1
                  : $I(op->src2)
                ;

              if (offset+length > consts->length)
              {
                INVALID_OP("load_const: outside range");
              }

              $S(op->dest) = lorito_string(interp, length, &((char *) consts->data)[offset]);
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
              if (offset+length >= consts->length)
              {
                INVALID_OP("load_const: outside range");
              }
              $I(op->dest) = ((int *) consts->data)[offset];
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
      case OP_coerce_int:
        switch (regtype)
        {
          case OP_PMC:
            $P(op->dest) = lorito_box_int_new(interp, $I(op->src1));
            break;
          case OP_INT:
            $I(op->dest) = $I(op->src1);
            break;
          default:
            INVALID_OP("coerce_int");
        }
        break;
      case OP_coerce_str:
        switch (regtype)
        {
          case OP_PMC:
            $P(op->dest) = lorito_box_str_new(interp, $S(op->src1));
            break;
          default:
            INVALID_OP("coerce_int");
        }
        break;
      case OP_coerce_pmc:
        switch (regtype)
        {
          case OP_STR:
            {
              Lorito_PMC *src1 = $P(op->src1);
              if (!IS_BOX_STR(src1))
              {
                $S(op->dest) = lorito_string(interp, 0, "");
                break;
              }
              $S(op->dest) = ((Lorito_BoxStr *) src1)->data;
              break;
            }
          default:
            INVALID_OP("coerce_pmc");
        }
        break;
      case OP_new:
        switch (regtype)
        {
          case OP_INT:
            $P(op->dest) = lorito_pmc_new(interp, $I(op->src1) + $imm);
            break;
          case OP_PMC:
            $P(op->dest) = lorito_pmc_new(interp, $P(op->src1)->size);
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
              if (offset+length > $P(op->dest)->size)
              {
                printf("%d >= %d\n", offset+length, $P(op->dest)->size);
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
              if (offset+length > $P(op->src1)->size)
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
        if (!IS_CTX($P(op->src1)))
        {
          INVALID_OP("call: missing required context pmc");
        }
        switch (regtype)
        {
          case OP_PMC: ;
            interp->ctx = (Lorito_Ctx *) $P(op->src1);
            $P(op->dest) = lorito_pop_arg(interp, ctx);
            ctx_chgd = 1;
            (*pc)++;
            continue;
          case OP_STR: ;
            lorito_pmc_func(interp, $P(op->src1), $S(op->src2));
            break;
        };
        break;
      case OP_push_arg:
        {
          Lorito_Ctx *src1 = ctx;
          if ($P(op->src1) != NULL)
          {
            if (!IS_CTX($P(op->src1)))
            {
              INVALID_OP("push_arg: missing required context pmc");
            }
            src1 = (Lorito_Ctx *) $P(op->src1);
          }
          switch (regtype)
          {
            case OP_PMC: ;
              src1->args[src1->args_cnt] = $P(op->src2);
              src1->args_cnt++;
              break;
            case OP_STR: ;
              lorito_push_arg(interp, src1, lorito_box_str_new(interp, $S(op->src2)));
              break;
            case OP_INT: ;
              src1->args[src1->args_cnt] = lorito_box_int_new(interp, $I(op->src2));
              src1->args_cnt++;
              break;
            default:
              INVALID_OP("push_arg");
          }
          break;
        }
      case OP_pop_arg:
        {
          Lorito_Ctx *src1 = ctx;
          if ($P(op->src1) != null)
          {
            if (!IS_CTX($P(op->src1)))
            {
              INVALID_OP("pop_arg: missing required context pmc");
            }
            src1 = (Lorito_Ctx *) $P(op->src1);
          }
          switch (regtype)
          {
            case OP_PMC: ;
              $P(op->dest) = lorito_pop_arg(interp, src1);
              break;
            case OP_STR: ;
              if ((src1->args_cnt == 0) || (!IS_BOX_STR(src1->args[src1->args_cnt-1])))
              {
                $S(op->dest) = lorito_string(interp, 0, "");
                break;
              }
              src1->args_cnt--;
              $S(op->dest) = ((Lorito_BoxStr *) src1->args[src1->args_cnt])->data;
              break;
            case OP_INT: ;
              if ((src1->args_cnt == 0) || (!IS_BOX_INT(src1->args[src1->args_cnt-1])))
              {
                $I(op->dest) = 0;
                break;
              }
              src1->args_cnt--;
              $I(op->dest) = *(int *) src1->args[src1->args_cnt]->data;
              break;
            default:
              INVALID_OP("pop_arg");
          }
          break;
        }
      case OP_push_ret:
        switch (regtype)
        {
          case OP_INT: ;
            ctx->rets[ctx->rets_cnt] = lorito_box_int_new(interp, $I(op->src1));
            ctx->rets_cnt++;
            break;
          default:
            INVALID_OP("push_ret");
        }
        break;
      case OP_pop_ret:
        {
          Lorito_Ctx *src1 = ctx;
          if ($P(op->src1) != NULL)
          {
            if (!IS_CTX($P(op->src1)))
            {
              INVALID_OP("push_ret: missing required context pmc");
            }
            src1 = (Lorito_Ctx *) $P(op->src1);
          }
          switch (regtype)
          {
            case OP_INT: ;
              if ((src1->rets_cnt == 0) || (!IS_BOX_INT(src1->rets[src1->rets_cnt-1])))
              {
                $I(op->dest) = 0;
                break;
              }
              src1->rets_cnt--;
              $I(op->dest) = *(int *) src1->rets[src1->rets_cnt]->data;
              break;
            default:
              INVALID_OP("pop_ret");
          }
          break;
        }
      case OP_block:
        switch (regtype)
        {
          case OP_INT: ;
            Lorito_File *cur_file = ctx->current_file;
            // Should we clone or use the code_block we have?
            $P(op->dest) = (Lorito_PMC *) cur_file->codesegs[$imm % cur_file->codeseg_count];
            break;
          default:
            INVALID_OP("block");
        }
        break;
      case OP_new_ctx:
        switch (regtype)
        {
          case OP_PMC: ;
            Lorito_Ctx *next_ctx = ctx;
            if ($P(op->src2) != NULL && IS_CTX($P(op->src2)))
            {
              next_ctx = (Lorito_Ctx *) $P(op->src2);
            }
            /*
            if (!IS_CODE($P(op->src1)))
            {
              INVALID_OP("new_ctx: must pass a code block");
            }
            */
            Lorito_Ctx *new_ctx = lorito_ctx_new(interp, next_ctx, $P(op->src1));
            new_ctx->pc = $imm;
            $P(op->dest) = (Lorito_PMC *) new_ctx;
            break;
          default:
            INVALID_OP("new_ctx");
        }
        break;
      case OP_ctx:
        switch (regtype)
        {
          case OP_PMC: ;
            $P(op->dest) = (Lorito_PMC *) ctx;
            break;
          default:
            INVALID_OP("ctx");
        }
        break;
      case OP_lookup:;
        if (!IS_CODE($P(op->src1)->lookup))
        {
          INVALID_OP("lookup: lookup must be code");
        }
        Lorito_Ctx *lookup_ctx = lorito_lookup_new(interp, ctx, $P(op->src1)->lookup);

        lorito_push_arg(interp, lookup_ctx, $P(op->src1)->vtable);

        switch (regtype)
        {
          case OP_STR: ;
            lorito_push_arg(interp, lookup_ctx, lorito_box_str_new(interp, $S(op->src2)));
            break;
          default:
            INVALID_OP("lookup");
        }
        interp->ctx = (Lorito_Ctx *) lookup_ctx;
        ctx_chgd = 1;
        // Do not update the pc.  When this returns, code at the begining of
        //  the loop with handle it on return.
        //(*pc)++;
        continue;
        break;
      case OP_loadlib:
        switch (regtype)
        {
          case OP_STR:
            $P(op->dest) = PMC lorito_load_bytecode(interp, $S(op->src1));
            break;
          default:
            INVALID_OP("loadlib");
        }
        break;
      case OP_read:
        break;
      case OP_write:
        switch (regtype)
        {
          case OP_PMC:
            fprintf(stdout, "%s", lorito_pmc_str(interp, $P(op->src1))->original);
            break;
          case OP_STR:
            fprintf(stdout, "%s", $S(op->src1)->original);
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
          case OP_PMC:
            fprintf(stdout, "%s\n", lorito_pmc_str(interp, $P(op->src1))->original);
            break;
          case OP_STR:
            fprintf(stdout, "%s\n", $S(op->src1)->original);
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
            fprintf(stderr, "%s", $S(op->src1)->original);
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
        ;
        char error[15];
        sprintf(error, "unknown (%d)", opcode);
        INVALID_OP(error);
        break;
      // END CORE_OPS
    }
    
    (*pc)++;
  }
}
