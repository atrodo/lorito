// This will initalize and drive the main interpeter.

#include "lorito.h"
#include "interp.h"
#include "loader.h"
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

  result->magic = (unsigned short int) rand();

  return result;
}

int
lorito_run(Lorito_Interp *interp)
{
  interp->ctx = lorito_ctx_new(interp, interp->ctx, interp->last_seg);
  core_exec(interp);
  return 0;
}
