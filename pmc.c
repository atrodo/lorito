// This will initalize and drive the pmc logic

#include "lorito.h"
#include "pmc.h"
#include "interp.h"
#include "internal_pmc.h"

Lorito_PMC *
lorito_pmc_init(Lorito_Interp *interp, Lorito_PMC *target)
{
  Lorito_PMC *result = target;

  result->magic = interp->magic;
  int size = result->size;

  result->ptr_count = 0;
  result->ptr_last = 0;
  result->data = NULL;
  result->ptrs = NULL;

  //if (result->lookup == NULL)
  {
    result->lookup = interp->default_lookup;
    result->vtable = null;
  }

  if (size > 0)
  {
    result->ptr_count = ((int) size / sizeof(void *)) / 2;
    if (result->ptr_count < 4)
      result->ptr_count = 4;
    result->data = malloc(size);
    result->ptrs = (Lorito_PMC *) malloc(result->ptr_count * sizeof(Lorito_PMC *));
  }

  return result;
}

Lorito_PMC *
lorito_pmc_new(Lorito_Interp *interp, int size)
{
  Lorito_PMC *result = (Lorito_PMC *) malloc(sizeof(Lorito_PMC));

  result->size = size;

  return lorito_pmc_init(interp, result);
  /*
  result->magic = interp->magic;

  result->ptr_count = ((int) size / sizeof(void *)) / 2;
  if (result->ptr_count < 4)
    result->ptr_count = 4;
  result->data = malloc(size);
  result->ptrs = (Lorito_PMC *) malloc(result->ptr_count * sizeof(Lorito_PMC *));
  result->ptr_last = 0;
  return result;
  */
}

Lorito_PMC *
lorito_pmc_encode(Lorito_Interp *interp, Lorito_PMC *dest, int offset, Lorito_PMC *src)
{
  Lorito_PMC * result = null;
  // Sanity checks
  int length = sizeof(int);

  if (dest == null)
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
    current = ++dest->ptr_last;
    *(int *) (dest->data + offset) = current;
    dest->ptrs[current] = *src;
    result = dest;
  }
  return result;

}

Lorito_PMC *
lorito_pmc_decode(Lorito_Interp *interp, Lorito_PMC *src, int offset)
{
  Lorito_PMC * result = null;
  // Sanity checks
  int length = sizeof(int);

  if (src == null)
  {
    return result;
  }
  if (offset+length >= src->size)
  {
    return result;
  }

  int current = *(int *) (src->data + offset);
  if ((current > 0) && (current <= src->ptr_count))
  {
    // Even if the new PMC is null, we're going to keep using it.
    //dest->ptrs[current] = *src;
    result = &src->ptrs[current];
  }
  return result;
}

void
lorito_pmc_default_lookup(Lorito_Interp *interp, Lorito_Ctx *ctx)
{
  Lorito_PMC *key = lorito_pop_arg(interp, ctx);
  Lorito_PMC *vtable = lorito_pop_arg(interp, ctx);
  lorito_clr_arg(interp, ctx);

  lorito_push_arg(interp, ctx, null);
}
