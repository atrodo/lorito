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

  unsigned short magic;

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
  // In opcodes
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
  // In bytes
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

enum INTERNAL_PMC_ENUM {
  NOT_INTERNAL = 0,

  BOX_INT      = 1,
  BOX_NUM      = 2,
  BOX_STR      = 3,

  FILE_BLOCK   = 4,
  CODE_BLOCK   = 5,
  DATA_BLOCK   = 6,
 
  CONTEXT      = 7,
  C_METHOD     = 8
};
typedef enum INTERNAL_PMC_ENUM Lorito_Internal;

struct lorito_pmc_t
{
  unsigned short magic;  // Magic identifier, changes per interp.
  unsigned short internal_type;

  int size;
  unsigned char *data;

  int ptr_count;
  int ptr_last;
  struct lorito_pmc_t *ptrs;

  // The data for the internal types
  union {
    int internal_int;
    double internal_num;
    void *internal_ptr;
  };
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
  double regs_n[REGNUM];
  struct lorito_str_t regs_s[REGNUM];
  struct lorito_pmc_t *regs_p[REGNUM];
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
