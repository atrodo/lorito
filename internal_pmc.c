// This will initalize and drive the pmc logic

#include "lorito.h"
#include "pmc.h"
#include "internal_pmc.h"
#include "interp.h"
#include "internal_pmc/context.h"

/*
Lorito_PMC *
lorito_internal_pmc_init(Lorito_Interp *interp, int size, Lorito_Internal internal, void *payload)
{
  Lorito_PMC *result;

  if (internal == NOT_INTERNAL)
    return result;

  switch (internal)
  {
    case BOX_INT:
      result = lorito_pmc_new(interp, sizeof(int));
      result->internal_type = internal;
      *(int *) result->data = *(int *) payload;
      break;
    case BOX_NUM:
      break;
    case BOX_STR:
      break;

    case FILE_BLOCK:
      break;
    case CODE_BLOCK:
      result = lorito_pmc_init(interp, 0);
      result->internal_type = internal;
      result->code = payload;
      break;
    case DATA_BLOCK:
      break;

    case LOOKUP:
    case CONTEXT:
      result = lorito_pmc_init(interp, 0);
      result->internal_type = internal;
      result->ctx = payload;
      break;
    case C_METHOD:
      break;
    default:
      break;
  }

  return result;
}
*/

Lorito_PMC *
lorito_box_int_new(Lorito_Interp *interp, int data)
{
  Lorito_PMC *result = lorito_pmc_new(interp, sizeof(int));
  result->internal_type = BOX_INT;
  *(int *) result->data = data;
  return result;
}

Lorito_PMC *
lorito_box_num_new(Lorito_Interp *interp, double data)
{
}

Lorito_PMC *
lorito_box_str_new(Lorito_Interp *interp, Lorito_Str *str)
{
  Lorito_BoxStr *result = (Lorito_BoxStr *) malloc(sizeof(Lorito_BoxStr));

  result->pmc.size = 0;
  result->pmc.internal_type = BOX_STR;
  result->data = str;

  lorito_pmc_init(interp, (Lorito_PMC *) result);
  return (Lorito_PMC *) result;
}

Lorito_File *
lorito_file_new(Lorito_Interp *interp, char *name)
{
  Lorito_File *result = (Lorito_File *) malloc(sizeof(Lorito_File));

  result->pmc.size = 0;
  result->pmc.internal_type = FILE_BLOCK;
  result->name = name;

  result->codeseg_count = 0;
  result->constseg_count = 0;
  result->codesegs = NULL;
  result->constsegs = NULL;

  lorito_pmc_init(interp, (Lorito_PMC *) result);

  return result;
}

Lorito_Codeseg *
lorito_code_block_new(Lorito_Interp *interp, char *name, int length, Lorito_Opcode *code)
{
  Lorito_Codeseg *result = (Lorito_Codeseg *) malloc(sizeof(Lorito_Codeseg));

  result->pmc.size = 0;
  result->pmc.internal_type = CODE_BLOCK;
  result->name = name;
  result->length = length;
  result->op = code;

  lorito_pmc_init(interp, (Lorito_PMC *) result);

  return result;
}

Lorito_Constseg *
lorito_const_block_new(Lorito_Interp *interp, char *name, int length, void *data)
{
  Lorito_Constseg *result = (Lorito_Constseg *) malloc(sizeof(Lorito_Constseg));

  result->pmc.size = 0;
  result->pmc.internal_type = CONST_BLOCK;
  result->name = name;
  result->length = length * sizeof(Lorito_Opcode);
  result->data = data;

  lorito_pmc_init(interp, (Lorito_PMC *) result);

  return result;
}

Lorito_Datadefseg *
lorito_datadef_block_new(Lorito_Interp *interp, char *name, int length)
{
  void *data = (void *) malloc(sizeof(Lorito_Opcode) * length);
  Lorito_Datadefseg *result = (Lorito_Datadefseg *) malloc(sizeof(Lorito_Datadefseg));

  result->pmc.size = length;
  result->pmc.internal_type = DATA_BLOCK;
  result->name = name;
  result->length = length * sizeof(Lorito_Opcode);
  result->data = data;

  lorito_pmc_init(interp, (Lorito_PMC *) result);

  return result;
}

Lorito_Ctx *
lorito_ctx_new(Lorito_Interp *interp, Lorito_Ctx *next_ctx, Lorito_PMC *codeseg)
{
  if (codeseg != null && !IS_CODE(codeseg))
  {
    fprintf(stderr, "Invalid code block to lorito_ctx_new\n");
    return (Lorito_Ctx *) null;
  }
  Lorito_Ctx *result = (Lorito_Ctx *) malloc(sizeof(Lorito_Ctx));

  result->pmc.size = 0;
  result->pmc.internal_type = CONTEXT;

  lorito_pmc_init(interp, (Lorito_PMC *) result);

  result->next_ctx = next_ctx;
  result->pc = 0;
  result->current_codeseg = codeseg;
  result->current_file = null;

  result->args_cnt = 0;
  result->rets_cnt = 0;

  result->pmc.lookup = (Lorito_PMC *) lorito_c_method_new(interp, lorito_internal_pmc_context_lookup);

  // We are going to blank out $S0 and $P0, but the rest may still cause
  //  a seg fault.  TODO
  result->regs.regs_s[0] = lorito_string(interp, 0, "");
  result->regs.regs_p[0] = null;

  result->current_constseg = (Lorito_Constseg *) null;

  if (codeseg != null && IS_CODE_BLOCK(((Lorito_PMC *) codeseg)))
  {
    Lorito_File *file = ((Lorito_Codeseg *) codeseg)->file;
    result->current_file = file;

    // Figure out which const block to use

    // If there is no const segements, use a blank one
    //  TODO

    // If there is only one, use it
    if (file->constseg_count == 1)
    {
      result->current_constseg = file->constsegs[0];
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

Lorito_Ctx *
lorito_lookup_new(Lorito_Interp *interp, Lorito_Ctx *next_ctx, Lorito_PMC *codeseg)
{
  Lorito_Ctx *result = lorito_ctx_new(interp, next_ctx, codeseg);

  result->pmc.internal_type = LOOKUP;

  return result;
}

Lorito_C_Method *
lorito_c_method_new(Lorito_Interp *interp, lorito_c_method target)
{
  Lorito_C_Method *result = (Lorito_C_Method *) malloc(sizeof(Lorito_C_Method));

  result->pmc.size = 0;
  result->pmc.internal_type = C_METHOD;

  lorito_pmc_init(interp, (Lorito_PMC *) result);

  result->method = target;
  result->method_data = NULL;

  return result;
}
