// This contains the giant switch core that will run the lorito microcode.

#include "lorito.h"
#include "microcode.h"

#define INVALID_OP(s) _opcode_error(s, regtype); *pc = -1;\
  continue;

#define $I(x) (reg->regs_i[x])
#define $N(x) (reg->regs_n[x])
#define $S(x) (reg->regs_s[x])
#define $P(x) (reg->regs_p[x])

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
            printf("%d\n", $I(op->dest));
            break;
          default:
            INVALID_OP("add");
        }
        break;
      case OP_sub:
        break;
      case OP_mul:
        break;
      case OP_div:
        break;
      case OP_mod:
        break;
      case OP_not:
        break;
      case OP_mov:
        break;
      case OP_set:
        break;
      case OP_goto:
        break;
      case OP_if:
        break;
      case OP_iseq:
        break;
      case OP_isgt:
        break;
      case OP_isge:
        break;
      case OP_and:
        break;
      case OP_or:
        break;
      case OP_xor:
        break;
      case OP_new:
        break;
      case OP_call:
        break;
      case OP_loadbc:
        break;
      case OP_read:
        break;
      case OP_write:
        break;
      case OP_gripe:
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
