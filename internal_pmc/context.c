// This will drive the context pmc logic

#include "lorito.h"
#include "pmc.h"
#include "interp.h"
#include "internal_pmc.h"
#include "internal_pmc/context.h"

void
lorito_internal_pmc_context_lookup(Lorito_Interp *interp, Lorito_Ctx *ctx)
{
  Lorito_PMC *inkey = lorito_pop_arg(interp, ctx);
  if (!IS_BOX_STR(inkey))
  {
    lorito_push_arg(interp, ctx, null);
    return;
  }
  Lorito_Str *key = ((Lorito_BoxStr *) inkey)->data;

  Lorito_PMC *vtable = lorito_pop_arg(interp, ctx);
  lorito_clr_arg(interp, ctx);

  if (key == lorito_string(interp, 0, "GET_DATASEG"))
  {
    lorito_push_arg(interp, ctx, (Lorito_PMC *) lorito_c_method_new(interp, lorito_internal_pmc_context_get_dataseg));
    return;
  }
  lorito_push_arg(interp, ctx, null);
  return;
}

void
lorito_internal_pmc_context_get_dataseg(Lorito_Interp *interp, Lorito_Ctx *ctx)
{
  Lorito_PMC *self     = lorito_pop_arg(interp, ctx);
  Lorito_PMC *box_name = lorito_pop_arg(interp, ctx);
  Lorito_Str *name;
  lorito_clr_arg(interp, ctx);

  if (!IS_CONTEXT(self))
  {
    //Bah.
    return;
  }
  Lorito_Ctx *self_ctx = (Lorito_Ctx *) self;

  if (IS_BOX_STR(box_name))
  {
    name = ((Lorito_BoxStr *) box_name)->data;
  }
  else
  {
    name = lorito_string(interp, 0, self_ctx->current_constseg->name);
  }

  Lorito_File *file = self_ctx->current_file;

  int i;
  for (i = 0; i < file->datadefseg_count; i++)
  {
    if (lorito_string(interp, 0, file->datadefsegs[i]->name) == name)
    {
      lorito_push_arg(interp, ctx, PMC file->datadefsegs[i]);
      break;
    }
  }

  return;
}
