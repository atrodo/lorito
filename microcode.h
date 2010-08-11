// This contains the definitions of the microcode format

#ifndef LORITO_MICROCODE_H_GUARD
#define LORITO_MICROCODE_H_GUARD

#define REG_OF_OP(o) (o >> 6)
#define  OP_OF_OP(o) (o & 0x3F)

enum OPS_REGS {
  OP_PMC = 0,
  OP_STR = 1,
  OP_NUM = 2,
  OP_INT = 3
};

enum OPS_ENUM {
  // BEGIN OPS_ENUM

  // Execution Ops
  OP_noop       = 0,
  OP_end        = 1,

  // Math Ops
  OP_add        = 2,
  OP_sub        = 3,
  OP_mul        = 4,
  OP_div        = 5,
  OP_mod        = 6,
  OP_and        = 7,
  OP_or         = 8,
  OP_xor        = 9,
  OP_not        = 10,
  OP_shl        = 11,
  OP_shr        = 12,

  // Comparison Ops
  OP_iseq       = 13,
  OP_isgt       = 14,
  OP_isge       = 15,

  // Flow Control Ops
  OP_goto       = 16,
  OP_if         = 17,

  // Register Manipulation Ops
  OP_set        = 18,
  OP_load_const = 19,
  OP_load_imm   = 20,
  OP_coerce_int = 21,
  OP_coerce_num = 22,
  OP_coerce_str = 23,

  // PMC Ops
  OP_new        = 24,
  OP_store      = 25,
  OP_load       = 26,
  OP_push_arg   = 27,
  OP_pop_arg    = 28,
  OP_call       = 29,

  OP_push_ret   = 39,
  OP_pop_ret    = 40,
  OP_clr_reg    = 41,

  OP_clr_arg    = 42,
  
  OP_lookup     = 35,
  OP_ctx        = 36,
  OP_new_ctx    = 38,

  OP_block      = 37,

  // Environment Ops
  OP_loadlib    = 30,
  OP_read       = 31,
  OP_write      = 32,
  OP_say        = 33,
  OP_gripe      = 34,

  // These Ops
  OP_hcf        = 63
  // END OPS_ENUM
};

#endif /* LORITO_MICROCODE_H_GUARD */
