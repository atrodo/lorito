// This contains the definitions of the microcode format

#ifndef LORITO_MICROCODE_H_GUARD
#define LORITO_MICROCODE_H_GUARD

#define REG_OF_OP(o) (o >> 5)
#define  OP_OF_OP(o) (o & 0x1F)

enum OPS_REGS {
  OP_PMC = 0,
  OP_STR = 1,
  OP_NUM = 2,
  OP_INT = 3
};

enum OPS_ENUM {
  OP_noop       = 0,
  OP_end        = 1,
  OP_add        = 2,
  OP_sub        = 3,
  OP_mul        = 4,
  OP_div        = 5,
  OP_mod        = 6,
  OP_not        = 7,
  OP_mov        = 8,
  OP_set        = 9,
  OP_goto       = 10,
  OP_if         = 11,
  OP_iseq       = 12,
  OP_isgt       = 13,
  OP_isge       = 14,
  OP_and        = 15,
  OP_or         = 16,
  OP_xor        = 17,
  OP_new        = 18,
  OP_call       = 19,
  OP_loadbc     = 20,
  OP_read       = 21,
  OP_write      = 22,
  OP_gripe      = 23,

  OP_hcf        = 31
};

#endif /* LORITO_MICROCODE_H_GUARD */
