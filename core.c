// This contains the giant switch core that will run the lorito microcode.

#include "lorito.h"
#include "microcode.h"

#define INVALID_OP(s) fprintf(stderr, "Invalid opcode: %s\n", s); *pc = -1;\
  continue;

void
core_exec(Lorito_Interp *interp)
{
  Lorito_Ctx *ctx = interp->ctx;
  int *pc = &ctx->pc;

  while (*pc >= 0)
  {
    Lorito_Codeseg *codeseg = ctx->current_codeseg;
    if (*pc >= codeseg->length)
      return;
    Lorito_Opcode op = ctx->current_codeseg->op[*pc];
    int reg = REG_OF_OP(op.opcode);
    int opcode = OP_OF_OP(op.opcode);

    printf("PC: %d\n", *pc);
    printf("Opcode: %d\n", opcode);

    switch (opcode)
    {
      case OP_noop:
        break;
      case OP_end:
        break;
      case OP_add:
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
        break;
    }
    
    (*pc)++;
  }
}
