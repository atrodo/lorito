// This will initalize and drive the main interpeter.

#include "lorito.h"
#include "interp.h"
#include "loader.h"
#include "pmc.h"
#include "internal_pmc.h"

#include <stdlib.h>

Lorito_Interp *
lorito_init()
{
  Lorito_Interp *result = (Lorito_Interp *) malloc(sizeof(Lorito_Interp));
  result->ctx = lorito_ctx_new(result, NULL, NULL);

  result->next_fileid = 0;
  result->allocated_file = 0;
  result->files = NULL;
  result->last_seg = NULL;

  result->symbol_len = 0;
  result->symbols = NULL;

  result->magic = (unsigned short int) rand();

  result->default_lookup = (Lorito_PMC *) lorito_c_method_new(result, lorito_pmc_default_lookup);
  result->null_pmc = lorito_pmc_new(result, 0);

  return result;
}

int
lorito_run(Lorito_Interp *interp)
{
  interp->ctx = lorito_ctx_new(interp, interp->ctx, (Lorito_PMC *) interp->last_seg);
  core_exec(interp);
  return 0;
}

Lorito_Str *
lorito_string(Lorito_Interp *interp, int length, const char *original)
{
  int i = 0;

  // See if this char * has been entered before
  for (i = 0; i < interp->symbol_len; i++)
  {
    if (*original == *interp->symbols[i].original)
      return &interp->symbols[i];
  }

  // See if it compares with an existing symbol
  for (i = 0; i < interp->symbol_len; i++)
  {
    if (length != interp->symbols[i].length)
      continue;
    if (strncmp(original, interp->symbols[i].original, length))
    {
      return &interp->symbols[i];
    }
  }

  // Otherwise, create one.
  i = interp->symbol_len;
  interp->symbol_len++;
  interp->symbols = realloc(interp->symbols, interp->symbol_len * sizeof(Lorito_Str));

  interp->symbols[i].length = length;
  interp->symbols[i].original = original;
  return &interp->symbols[i];
}

void
lorito_push_arg(Lorito_Interp *interp, Lorito_Ctx *target, Lorito_PMC *item)
{
  int cnt = target->args_cnt++;
  target->args[cnt] = item;
}

Lorito_PMC *
lorito_pop_arg(Lorito_Interp *interp, Lorito_Ctx *target)
{
  if (target->args_cnt == 0)
    return null;

  int cnt = --target->args_cnt;
  return target->args[cnt];
}

void
lorito_clr_arg(Lorito_Interp *interp, Lorito_Ctx *target)
{
  target->args_cnt = 0;
}

