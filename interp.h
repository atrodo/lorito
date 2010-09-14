#include "lorito.h"

#ifndef LORITO_INTERP_H_GUARD
#define LORITO_INTERP_H_GUARD

Lorito_Interp *
lorito_init();

int
lorito_run(Lorito_Interp *interp);

Lorito_Str *
lorito_string(Lorito_Interp *interp, int length, const char *original);

void
lorito_push_arg(Lorito_Interp *interp, Lorito_Ctx *target, Lorito_PMC *item);

Lorito_PMC *
lorito_pop_arg(Lorito_Interp *interp, Lorito_Ctx *target);

void
lorito_clr_arg(Lorito_Interp *interp, Lorito_Ctx *target);

#endif /* LORITO_INTERP_H_GUARD */
