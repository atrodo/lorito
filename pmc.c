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

Lorito_PMC *
lorito_pmc_encode(Lorito_Interp *interp, Lorito_PMC *dest, int offset, Lorito_PMC *src)
{
  Lorito_PMC * result = NULL;
  // Sanity checks
  int length = sizeof(int);

  if (dest == NULL)
  {
    return result;
  }
  if (offset+length >= dest->size)
  {
    return result;
  }

  // Reuse a slot if there is one, otherwise make one.
  int current = *(int *) (src->data + offset);
  if ((current > 0) && (current <= dest->ptr_count))
  {
    // Even if the new PMC is null, we're going to keep using it.
    dest->ptrs[current] = *src;
    result = dest;
  } else {
    if (dest->ptr_count == dest->ptr_last)
    {
      result->ptr_count += 4;
      dest->ptrs = (Lorito_PMC *) realloc(dest->ptrs, result->ptr_count * sizeof(Lorito_PMC *));
      if (dest->ptrs == NULL)
        abort();
    }
    *(int *) (src->data + offset) = ++dest->ptr_last;
    result = dest;
  }
  return result;

}

Lorito_PMC *
lorito_pmc_decode(Lorito_Interp *interp, Lorito_PMC *dest, int offset)
{
}
