#include "lorito.h"

#ifndef LORITO_PMC_H_GUARD
#define LORITO_PMC_H_GUARD

Lorito_PMC *
lorito_pmc_init(Lorito_Interp *interp, int size);

Lorito_PMC *
lorito_pmc_encode(Lorito_Interp *interp, Lorito_PMC *dest, int offset, Lorito_PMC *src);

Lorito_PMC *
lorito_pmc_decode(Lorito_Interp *interp, Lorito_PMC *dest, int offset);

#endif /* LORITO_PMC_H_GUARD */
