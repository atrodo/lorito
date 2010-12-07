#include "lorito.h"

#ifndef LORITO_LOADER_H_GUARD
#define LORITO_LOADER_H_GUARD

#define SEG_code 0
#define SEG_const 1

void
loadbc(Lorito_Interp *interp, const char* filename);
Lorito_File *
loadbc(Lorito_Interp *interp, char* filename);

#endif /* LORITO_LOADER_H_GUARD */
