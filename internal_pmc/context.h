#include "lorito.h"

#ifndef LORITO_INTERNAL_PMC_CONTEXT_H_GUARD
#define LORITO_INTERNAL_PMC_CONTEXT_H_GUARD

void
lorito_internal_pmc_context_lookup(Lorito_Interp *interp, Lorito_Ctx *ctx);

void
lorito_internal_pmc_context_get_dataseg(Lorito_Interp *interp, Lorito_Ctx *ctx);

#endif /* LORITO_INTERNAL_PMC_CONTEXT_H_GUARD */
