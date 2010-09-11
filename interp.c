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

