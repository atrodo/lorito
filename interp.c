// This will initalize and drive the main interpeter.

#include "lorito.h"
#include "interp.h"
#include "loader.h"

Lorito_Ctx *
lorito_ctx_init(Lorito_Ctx *next_ctx, Lorito_Codeseg *codeseg)
{
  Lorito_Ctx *result = (Lorito_Ctx *) malloc(sizeof(Lorito_Ctx));

  result->next_ctx = next_ctx;
  result->pc = 0;
  result->current_codeseg = codeseg;

  return result;
}

Lorito_PMC *
lorito_pmc_init(Lorito_Interp *interp, int size)
{
  Lorito_PMC *result = (Lorito_PMC *) malloc(sizeof(Lorito_PMC));
  result->size = size;

  result->ptrnum = ((int) size / sizeof(void *)) / 2;
  if (result->ptrnum < 4)
    result->ptrnum = 4;
  result->data = malloc(size);
  result->ptrs = (Lorito_PMC *) malloc(result->ptrnum * sizeof(void *));
  return result;
}

Lorito_Interp *
lorito_init()
{
  Lorito_Interp *result = (Lorito_Interp *) malloc(sizeof(Lorito_Interp));
  result->ctx = lorito_ctx_init(NULL, NULL);
  result->regs = &result->ctx->regs;

  return result;
}

int
lorito_run(Lorito_Interp *interp)
{
  interp->ctx = lorito_ctx_init(interp->ctx, interp->last_seg);
  core_exec(interp);
  return 0;
}
