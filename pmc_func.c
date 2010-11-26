// This will initalize and drive the pmc logic

#include "lorito.h"
#include "pmc.h"
#include "pmc_func.h"
#include "interp.h"

void
lorito_pmc_func_size(Lorito_Interp *interp, Lorito_Ctx *ctx)
{
  Lorito_PMC *source = lorito_pop_arg(ctx);
  lorito_clr_arg(ctx);
  lorit_push_arg(interp, ctx, source->size);
}
