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
struct lorito_opcode_t;
struct lorito_reg_t;
struct lorito_pmc_t;

struct lorito_str_t;
struct lorito_file_t;
struct lorito_codeseg_t;
struct lorito_dataseg_t;
struct lorito_ctx_t;
struct lorito_c_method_t;

struct lorito_interp_t
{
  struct lorito_ctx_t* ctx;
  int next_fileid;

  unsigned short magic;

  int allocated_file;
  struct lorito_file_t **files;
  //
  struct lorito_codeseg_t *last_seg;

  struct lorito_pmc_t *default_lookup;
};
typedef struct lorito_interp_t Lorito_Interp;

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

struct lorito_reg_t
{
  int regs_i[REGNUM];
  double regs_n[REGNUM];
  struct lorito_str_t *regs_s[REGNUM];
  struct lorito_pmc_t *regs_p[REGNUM];
};
typedef struct lorito_reg_t Lorito_Reg;



// The enum of internal PMCs
enum INTERNAL_PMC_ENUM {
  NOT_INTERNAL = 0,

  BOX_INT      = 1,
  BOX_NUM      = 2,
  BOX_STR      = 3,

  FILE_BLOCK   = 4,
  CODE_BLOCK   = 5,
  DATA_BLOCK   = 6,
 
  CONTEXT      = 7,
  LOOKUP       = 8,
  C_METHOD     = 9
};
typedef enum INTERNAL_PMC_ENUM Lorito_Internal;

struct lorito_pmc_t
{
  unsigned short magic;  // Magic identifier, changes per interp.
  Lorito_Internal internal_type;

  int size;
  unsigned char *data;

  int ptr_count;
  int ptr_last;
  struct lorito_pmc_t *ptrs;

  // Lookup method
  struct lorito_pmc_t *lookup;
  // VTable PMC for lookup
  struct lorito_pmc_t *vtable;

  // The data for the internal types
  /*
  union {
    int boxed_int;
    double boxed_num;
    struct lorito_str_t *boxed_str;
    struct lorito_file_t *file;
    struct lorito_codeseg_t *code;
    struct lorito_dataseg_t *data;
    struct lorito_ctx_t *ctx;
  };
  */
};
typedef struct lorito_pmc_t Lorito_PMC;

// Macros for Internal PMCs.
#define IS_INTERNAL(p)   (p->internal_type != NOT_INTERNAL)
#define IS_BOX_INT(p)    (p->internal_type == BOX_INT     )
#define IS_BOX_NUM(p)    (p->internal_type == BOX_NUM     )
#define IS_BOX_STR(p)    (p->internal_type == BOX_STR     )
#define IS_FILE_BLOCK(p) (p->internal_type == FILE_BLOCK  )
#define IS_CODE_BLOCK(p) (p->internal_type == CODE_BLOCK  )
#define IS_DATA_BLOCK(p) (p->internal_type == DATA_BLOCK  )
#define IS_CONTEXT(p)    (p->internal_type == CONTEXT     )
#define IS_LOOKUP(p)     (p->internal_type == LOOKUP      )
#define IS_C_METHOD(p)   (p->internal_type == C_METHOD    )

#define IS_CODE(p)       (p->internal_type == CODE_BLOCK   \
                       || p->internal_type == C_METHOD    )

#define IS_CALLABLE(p)   (p->internal_type == CODE_BLOCK   \
                       || p->internal_type == C_METHOD     \
                       || p->internal_type == CONTEXT     )

#define IS_CTX(p)        (p->internal_type == LOOKUP       \
                       || p->internal_type == CONTEXT     )

#define IS_BOXED(p)      (p->internal_type == BOX_INT      \
                       || p->internal_type == BOX_NUM      \
                       || p->internal_type == BOX_STR     )


// Internal PMCs

struct lorito_str_t
{
  struct lorito_pmc_t pmc;
  int size;
  void *data;
};
typedef struct lorito_str_t Lorito_Str;

struct lorito_file_t
{
  struct lorito_pmc_t pmc;
  int fileid;
  char* name;
  int codeseg_count;
  struct lorito_codeseg_t** codesegs;
  int dataseg_count;
  struct lorito_dataseg_t** datasegs;
};
typedef struct lorito_file_t Lorito_File;

struct lorito_codeseg_t
{
  struct lorito_pmc_t pmc;
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
  struct lorito_pmc_t pmc;
  int fileid;
  struct lorito_file_t *file;
  int segid;
  char* name;
  // In bytes
  int length;
  void* data;
};
typedef struct lorito_dataseg_t Lorito_Dataseg;

struct lorito_ctx_t
{
  struct lorito_pmc_t pmc;
  struct lorito_ctx_t* next_ctx;
  int pc;
  struct lorito_file_t* current_file;
  struct lorito_pmc_t* current_codeseg;
  struct lorito_dataseg_t* current_dataseg;
  struct lorito_reg_t regs;

  unsigned int args_cnt;
  struct lorito_pmc_t *args[32];
  unsigned int rets_cnt;
  struct lorito_pmc_t *rets[32];
};
typedef struct lorito_ctx_t Lorito_Ctx;

typedef void (*lorito_c_method)(Lorito_Interp *interp, Lorito_Ctx *ctx);

struct lorito_c_method_t
{
  struct lorito_pmc_t pmc;
  lorito_c_method method;
  void *method_data;
};
typedef struct lorito_c_method_t Lorito_C_Method;

#endif /* LORITO_LORITO_H_GUARD */
