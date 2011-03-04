#include "lorito.h"

#ifndef LORITO_PMC_H_GUARD
#define LORITO_PMC_H_GUARD

#define null interp->null_pmc
#define PMC (Lorito_PMC *)

Lorito_PMC *
lorito_pmc_init(Lorito_Interp *interp, Lorito_PMC *target);

Lorito_PMC *
lorito_pmc_new(Lorito_Interp *interp, int size);

Lorito_PMC *
lorito_pmc_encode(Lorito_Interp *interp, Lorito_PMC *dest, int offset, Lorito_PMC *src);

Lorito_PMC *
lorito_pmc_decode(Lorito_Interp *interp, Lorito_PMC *src, int offset);

Lorito_Str *
lorito_pmc_str(Lorito_Interp *interp, Lorito_PMC *src);

void
lorito_pmc_default_lookup(Lorito_Interp *interp, Lorito_Ctx *ctx);

#endif /* LORITO_PMC_H_GUARD */
