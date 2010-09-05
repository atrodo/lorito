#include "lorito.h"

#ifndef LORITO_INTERNAL_PMC_H_GUARD
#define LORITO_INTERNAL_PMC_H_GUARD

Lorito_PMC *
lorito_internal_pmc_init(Lorito_Interp *interp, int size, Lorito_Internal internal, void *payload);

Lorito_PMC *
lorito_box_int_new(Lorito_Interp *interp, int data);

Lorito_PMC *
lorito_box_num_new(Lorito_Interp *interp, double data);

Lorito_PMC *
lorito_box_str_new(Lorito_Interp *interp, Lorito_Str *str);

Lorito_File *
lorito_file_new(Lorito_Interp *interp, char *name);

Lorito_Codeseg *
lorito_code_block_new(Lorito_Interp *interp, char *name, int length, Lorito_Opcode *code);

Lorito_Dataseg *
lorito_data_block_new(Lorito_Interp *interp, char *name, int length, void *data);

Lorito_Ctx *
lorito_ctx_new(Lorito_Interp *interp, Lorito_Ctx *next_ctx, Lorito_Codeseg *codeseg);

Lorito_Ctx *
lorito_lookup_new(Lorito_Interp *interp, Lorito_Ctx *next_ctx, Lorito_Codeseg *codeseg);

#endif /* LORITO_INTERNAL_PMC_H_GUARD */
