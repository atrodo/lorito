#include "lorito.h"

#ifndef LORITO_LOADER_H_GUARD
#define LORITO_LOADER_H_GUARD

#define SEG_code 0
#define SEG_const 1

#define SEG_FLAG_init 0x01
#define SEG_FLAG_main 0x02

Lorito_File *
loadbc(Lorito_Interp *interp, char* filename);

#endif /* LORITO_LOADER_H_GUARD */
