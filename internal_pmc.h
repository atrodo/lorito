#include "lorito.h"

#ifndef LORITO_INTERNAL_PMC_H_GUARD
#define LORITO_INTERNAL_PMC_H_GUARD

Lorito_PMC *
lorito_internal_pmc_init(Lorito_Interp *interp, int size, Lorito_Internal internal, void *payload);

/*
  NOT_INTERNAL = 0,
  BOX_INT      = 1,
  BOX_NUM      = 2,
  BOX_STR      = 3,
  FILE_BLOCK   = 4,
  CODE_BLOCK   = 5,
  CONST_BLOCK  = 6,
  CONTEXT      = 7,
  LOOKUP       = 8,
  C_METHOD     = 9

*/

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

Lorito_Constseg *
lorito_const_block_new(Lorito_Interp *interp, char *name, int length, void *data);

Lorito_Datadefseg *
lorito_datadef_block_new(Lorito_Interp *interp, char *name, int length);

Lorito_Ctx *
lorito_ctx_new(Lorito_Interp *interp, Lorito_Ctx *next_ctx, Lorito_PMC *codeseg);

Lorito_Ctx *
lorito_lookup_new(Lorito_Interp *interp, Lorito_Ctx *next_ctx, Lorito_PMC *codeseg);

Lorito_C_Method *
lorito_c_method_new(Lorito_Interp *interp, lorito_c_method target);

#endif /* LORITO_INTERNAL_PMC_H_GUARD */
