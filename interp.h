#include "lorito.h"

#ifndef LORITO_INTERP_H_GUARD
#define LORITO_INTERP_H_GUARD

Lorito_Ctx *
lorito_ctx_init(Lorito_Ctx *next_ctx, Lorito_Codeseg *codeseg);

Lorito_PMC *
lorito_pmc_init(Lorito_Interp *interp, int size);

Lorito_Interp *
lorito_init();

int
lorito_run(Lorito_Interp *interp);

#endif /* LORITO_INTERP_H_GUARD */
