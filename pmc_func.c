// This will initalize and drive the pmc logic

#include "lorito.h"
#include "pmc.h"
#include "pmc_func.h"
#include "interp.h"

void lorito_pmc_func(Lorito_Interp *interp, Lorito_Ctx *ctx, Lorito_Str *func)
{
  if (func == lorito_string(interp, 0, "size"))
  {
    lorito_pmc_func_size(interp, ctx);
  }
}

void
lorito_pmc_func_size(Lorito_Interp *interp, Lorito_Ctx *ctx)
{
  Lorito_PMC *source = lorito_pop_arg(interp, ctx);
  lorito_clr_arg(interp, ctx);
  Lorito_PMC *size = lorito_box_int_new(interp, source->size);
  lorito_push_arg(interp, ctx, size);
}
