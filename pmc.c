// This will initalize and drive the pmc logic

#include "lorito.h"
#include "pmc.h"

Lorito_PMC *
lorito_pmc_init(Lorito_Interp *interp, int size)
{
  Lorito_PMC *result = (Lorito_PMC *) malloc(sizeof(Lorito_PMC));
  result->size = size;

  result->ptr_count = ((int) size / sizeof(void *)) / 2;
  if (result->ptr_count < 4)
    result->ptr_count = 4;
  result->data = malloc(size);
  result->ptrs = (Lorito_PMC *) malloc(result->ptr_count * sizeof(Lorito_PMC *));
  result->ptr_last = 0;
  return result;
}
