// This contains the definitions of the microcode format

#ifndef LORITO_MICROCODE_H_GUARD
#define LORITO_MICROCODE_H_GUARD


enum OPS_ENUM {
  OP_noop       = 0,
  OP_end        = 1,
  OP_add        = 2,
  OP_sub        = 3,
  OP_mul        = 4,
  OP_div        = 5,
  OP_set        = 6,
  OP_goto       = 7,
  OP_if         = 8,
  OP_iseq       = 9,
  OP_isgt       = 10,
  OP_islt       = 11,
  OP_and        = 12,
  OP_or         = 13,
  OP_new        = 14,
  OP_call       = 15,
  OP_load       = 16,
  OP_read       = 17,
  OP_write      = 18,
  OP_gripe      = 19,

  OP_hcf        = 255
};

#endif /* LORITO_MICROCODE_H_GUARD */
