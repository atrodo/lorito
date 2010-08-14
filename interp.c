// This will initalize and drive the main interpeter.

#include "lorito.h"
#include "interp.h"
#include "loader.h"

#include <stdlib.h>

Lorito_Ctx *
lorito_ctx_init(Lorito_Ctx *next_ctx, Lorito_Codeseg *codeseg)
{
  Lorito_Ctx *result = (Lorito_Ctx *) malloc(sizeof(Lorito_Ctx));

  result->next_ctx = next_ctx;
  result->pc = 0;
  result->current_codeseg = codeseg;
  result->current_file = NULL;

  result->args_cnt = 0;

  if (codeseg != NULL)
  {
    Lorito_File *file = codeseg->file;
    result->current_file = file;

    // Figure out which data block to use

    // If there is no data segements, use a blank one
    //  TODO

    // If there is only one, use it
    if (file->dataseg_count == 1)
    {
      result->current_dataseg = file->datasegs[0];
    }

    // If there is one with the same name, use it
    //  TODO

    // If there is one with a blank name, use it
    //  TODO

    // Otherwise, use a blank one
    //  TODO
  }

  return result;
}

Lorito_Interp *
lorito_init()
{
  Lorito_Interp *result = (Lorito_Interp *) malloc(sizeof(Lorito_Interp));
  result->ctx = lorito_ctx_init(NULL, NULL);
  result->regs = &result->ctx->regs;

  result->next_fileid = 0;
  result->allocated_file = 0;
  result->files = NULL;

  result->magic = (unsigned short int) rand();

  return result;
}

int
lorito_run(Lorito_Interp *interp)
{
  interp->ctx = lorito_ctx_init(interp->ctx, interp->last_seg);
  core_exec(interp);
  return 0;
}
