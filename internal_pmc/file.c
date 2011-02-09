// This will drive the context pmc logic

#include "lorito.h"
#include "pmc.h"
#include "interp.h"
#include "internal_pmc.h"
#include "internal_pmc/file.h"

void
lorito_internal_pmc_file_get_method(Lorito_Interp *interp, Lorito_Ctx *ctx)
{
  Lorito_PMC *inkey  = lorito_pop_arg(interp, ctx);
  Lorito_PMC *inself = lorito_pop_arg(interp, ctx);
  lorito_clr_arg(interp, ctx);

  printf("1\n");
  if (!IS_BOX_STR(inkey))
  {
    lorito_push_arg(interp, ctx, null);
    return;
  }
  printf("2\n");
  if (!IS_FILE_BLOCK(inself))
  {
    //Bah.
    lorito_push_arg(interp, ctx, null);
    return;
  }

  Lorito_File *self = (Lorito_Ctx *) inself;
  Lorito_Str *key = ((Lorito_BoxStr *) inkey)->data;
  Lorito_PMC *result = null;

  int i;
  for (i=0; i < self->codeseg_count; i++)
  {
    printf("%s\n", self->codesegs[i]->name);
    if (self->codesegs[i]->name == key)
    {
      lorito_push_arg(interp, ctx, PMC self->codesegs[i]);
      return;
    }
  }
}


void
lorito_internal_pmc_file_lookup(Lorito_Interp *interp, Lorito_Ctx *ctx)
{
  Lorito_PMC *inkey = lorito_pop_arg(interp, ctx);
  Lorito_PMC *vtable = lorito_pop_arg(interp, ctx);
  lorito_clr_arg(interp, ctx);

  if (!IS_BOX_STR(inkey))
  {
    lorito_push_arg(interp, ctx, null);
    return;
  }
  Lorito_Str *key = ((Lorito_BoxStr *) inkey)->data;

  if (key == lorito_string(interp, 0, "GET_METHOD"))
  {
    lorito_push_arg(interp, ctx, (Lorito_PMC *) lorito_c_method_new(interp, lorito_internal_pmc_file_get_method));
    return;
  }
  lorito_push_arg(interp, ctx, null);
  return;
}
