// This contains all the structures for lorito

#ifndef LORITO_LORITO_H_GUARD
#define LORITO_LORITO_H_GUARD

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

struct lorito_interp_t;
struct lorito_codeseg_t;
struct lorito_opcode_t;
struct lorito_ctx_t;
struct lorito_reg_t;

struct lorito_interp_t
{
  struct lorito_ctx_t* ctx;
  struct lorito_reg_t* regs;
};

struct lorito_codeseg_t
{
  int fileid;
  int segid;
  int length;
  struct lorito_opcode_t* op;
};

struct lorito_opcode_t
{
  unsigned char opcode;
  unsigned char dest;
  unsigned char src1;
  unsigned char src2;
  union
  {
    int immediate;
    int jumploc;
    int offset;
  };
};

struct lorito_reg_t
{
  int regs_i[REGNUM];
};

struct lorito_ctx_t
{
  struct lorito_ctx_t* next_ctx;
  int pc;
  struct lorito_codeseg_t* current_codeseg;
  struct lorito_reg_t regs;
};

typedef struct lorito_interp_t Lorito_Interp;

#endif /* LORITO_LORITO_H_GUARD */
