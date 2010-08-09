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
      result->internal_int = *(int *) payload;
      break;
    case BOX_NUM:
    case BOX_STR:

    case FILE_BLOCK:
    case CODE_BLOCK:
    case DATA_BLOCK:

    case CONTEXT:
    case C_METHOD:
    default: ;
  }

  return result;
}

