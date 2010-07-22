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

#define SEG_code 0
#define SEG_data 1

struct lorito_interp_t;
struct lorito_file_t;
struct lorito_codeseg_t;
struct lorito_dataseg_t;
struct lorito_opcode_t;
struct lorito_pmc_t;
struct lorito_reg_t;
struct lorito_ctx_t;

struct lorito_interp_t
{
  struct lorito_ctx_t* ctx;
  struct lorito_reg_t* regs;
  int next_fileid;

  int allocated_file;
  struct lorito_file_t *files;
  //
  struct lorito_codeseg_t *last_seg;
};
typedef struct lorito_interp_t Lorito_Interp;

struct lorito_file_t
{
  int fileid;
  char* name;
  int codeseg_count;
  struct lorito_codeseg_t* codesegs;
  int dataseg_count;
  struct lorito_dataseg_t* datasegs;
};
typedef struct lorito_file_t Lorito_File;

struct lorito_codeseg_t
{
  int fileid;
  struct lorito_file_t *file;
  int segid;
  char* name;
  int length;
  struct lorito_opcode_t* op;
};
typedef struct lorito_codeseg_t Lorito_Codeseg;

struct lorito_dataseg_t
{
  int fileid;
  struct lorito_file_t *file;
  int segid;
  char* name;
  int length;
  void* data;
};
typedef struct lorito_dataseg_t Lorito_Dataseg;

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
typedef struct lorito_opcode_t Lorito_Opcode;

struct lorito_pmc_t
{
  int size;
  void *data;
  int ptrnum;
  struct lorito_pmc_t *ptrs;
};
typedef struct lorito_pmc_t Lorito_PMC;

struct lorito_str_t
{
  int size;
  void *data;
};
typedef struct lorito_str_t Lorito_STR;

struct lorito_reg_t
{
  int regs_i[REGNUM];
  double regs_f[REGNUM];
  struct lorito_pmc_t *regs_p[REGNUM];
  struct lorito_str_t *regs_s[REGNUM];
};
typedef struct lorito_reg_t Lorito_Reg;

struct lorito_ctx_t
{
  struct lorito_ctx_t* next_ctx;
  int pc;
  struct lorito_file_t* current_file;
  struct lorito_codeseg_t* current_codeseg;
  struct lorito_dataseg_t* current_dataseg;
  struct lorito_reg_t regs;
};
typedef struct lorito_ctx_t Lorito_Ctx;

#endif /* LORITO_LORITO_H_GUARD */
