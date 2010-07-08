// This contains the giant switch core that will run the lorito microcode.

#include "lorito.h"
#include "microcode.h"

void
core_exec(Lorito_Interp interp)
{
  int *pc;

  while (pc)
  {
    switch (*pc)
    {
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
      case OP_islt:
        break;
      case OP_and:
        break;
      case OP_or:
        break;
      case OP_new:
        break;
      case OP_call:
        break;
      case OP_load:
        break;
      case OP_read:
        break;
      case OP_write:
        break;
      case OP_gripe:
        break;
      default:
        // Error
        break;
    }
  }
}
