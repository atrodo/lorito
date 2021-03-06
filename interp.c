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

  Lorito_PMC *null_pmc = lorito_pmc_new(result, 0);

  result->next_fileid = 0;
  result->allocated_file = 0;
  result->files = NULL;
  result->last_seg = NULL;

  result->symbol_len = 0;
  result->symbols = NULL;

  result->magic = (unsigned short int) rand();

  result->null_pmc = null_pmc;

  result->ctx = lorito_ctx_new(result, null_pmc, null_pmc);
  result->default_lookup = (Lorito_PMC *) lorito_c_method_new(result, lorito_pmc_default_lookup);

  // Re-initalize the null pmc
  lorito_pmc_init(result, null_pmc);

  return result;
}

/* Run the interp.  Uses the last file's first main code segment. */
int
lorito_run(Lorito_Interp *interp)
{
  interp->last_error = lorito_string(interp, 0, "");

  if (interp->next_fileid == 0)
  {
    interp->last_error = lorito_string(interp, 0, "No bytecode loaded.");
    return -1;
  }

  Lorito_File *file = interp->files[interp->next_fileid-1];
  Lorito_Codeseg *main = NULL;

  int i;
  for (i=0; i < file->codeseg_count; i++)
  {
    if ((file->codesegs[i]->flags & SEG_FLAG_main) == SEG_FLAG_main)
    {
      main = file->codesegs[i];
      break;
    }
  }

  if (main == NULL)
  {
    interp->last_error = lorito_string(interp, 0, "No entry point found");
    return -1;
  }

  interp->ctx = lorito_ctx_new(interp, interp->ctx, (Lorito_PMC *) main);
  core_exec(interp);
  return 0;
}

Lorito_Str *
lorito_string(Lorito_Interp *interp, int length, const char *original)
{
  int i = 0;
  
  //printf("Lorito_Str: %s\n", original);
  // See if this char * has been entered before
  /*
  */
  for (i = 0; i < interp->symbol_len; i++)
  {
    if (original == (interp->symbols[i]->original))
      return interp->symbols[i];
  }

  if (length == 0)
  {
    // Lorito counts length as total length, including the Null byte.
    length = strlen(original) + 1;
  }

  // See if it compares with an existing symbol
  for (i = 0; i < interp->symbol_len; i++)
  {
    if (length != interp->symbols[i]->length)
      continue;
    //printf("Lorito_Str Cmp: %s\n", interp->symbols[i]->original);
    if (strncmp(original, interp->symbols[i]->original, length) == 0)
    {
      return interp->symbols[i];
    }
  }

  //printf("Lorito_Str Create: %s\n", original);
  // Otherwise, create one.
  i = interp->symbol_len;
  interp->symbol_len++;
  interp->symbols = realloc(interp->symbols, interp->symbol_len * sizeof(Lorito_Str *));

  interp->symbols[i] = malloc(sizeof(Lorito_Str));
  interp->symbols[i]->length = length;
  interp->symbols[i]->original = original;
  return interp->symbols[i];
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

