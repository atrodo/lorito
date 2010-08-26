// This will initalize and drive the pmc logic

#include "lorito.h"
#include "pmc.h"
#include "internal_pmc.h"

Lorito_PMC *
lorito_internal_pmc_init(Lorito_Interp *interp, int size, Lorito_Internal internal, void *payload)
{
  Lorito_PMC *result;

  if (internal == NOT_INTERNAL)
    return result;

  switch (internal)
  {
    case BOX_INT:
      result = lorito_pmc_init(interp, sizeof(int));
      result->internal_type = internal;
      *(int *) result->data = *(int *) payload;
      result->boxed_int = *(int *) payload;
      break;
    case BOX_NUM:
      break;
    case BOX_STR:
      break;

    case FILE_BLOCK:
      break;
    case CODE_BLOCK:
      result = lorito_pmc_init(interp, 0);
      result->internal_type = internal;
      result->code = payload;
      break;
    case DATA_BLOCK:
      break;

    case LOOKUP:
    case CONTEXT:
      result = lorito_pmc_init(interp, 0);
      result->internal_type = internal;
      result->ctx = payload;
      break;
    case C_METHOD:
      break;
    default:
      break;
  }

  return result;
}

