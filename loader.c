// This will load a lorito bytecode file into a lorito codeseg

// Since this is temporary, and we'll end up throwing it away in favor of
//  integrating with parrot's packfile format, this will be real simple.
//
// Integer: segment type (0 = code, 1 = const, 2 = datadef)
//   Integer: Flags of the segment
//   Integer: Size of segement name
//   String: segment name, null terminated
//   Integer: Count (in 8 bytes, so a count of 1 == 8 bytes)
//   Data
//
// The datadef segment has no data.  It is equivalent to a bss in asm terms.

// Flags for code segment:
//   0b0001: Init, Ran after file is loaded
//   0b0010: Main, This segement is an entry point

#include "lorito.h"
#include "internal_pmc.h"
#include "loader.h"
#include <stdio.h>

void file_info(Lorito_Interp *interp)
{
  int i, j;

  for (i = 0; i < interp->next_fileid; i++)
  {
    Lorito_File *file = interp->files[i];
    //printf("%s\n", file->name);
    for (j = 0; j < file->constseg_count; j++)
    {
      Lorito_Constseg *constseg = file->constsegs[j];
      //printf(" DSegNum:  %d\n", constseg->segid);
      //printf(" DSegName: %s\n", constseg->name);
    }
    for (j = 0; j < file->codeseg_count; j++)
    {
      Lorito_Codeseg *codeseg = file->codesegs[j];
      //printf(" CSegNum:  %d\n", codeseg->segid);
      //printf(" CSegName: %s\n", codeseg->name);
    }
  }
}

Lorito_File *
loadbc(Lorito_Interp *interp, char* filename)
{
  int fileid = interp->next_fileid++;
  int segid = 0;
  FILE *input = fopen(filename, "rb");
  if (input == NULL) {
    perror("Failed to open file");
    return;
  }

  if (fileid >= interp->allocated_file)
  {
    interp->allocated_file += 4;
    interp->files = realloc(interp->files, interp->allocated_file);
    if (interp->files == NULL)
      abort();
  }

  /*
  Lorito_File *file = &interp->files[fileid];
  lorito_pmc_init(interp, (Lorito_PMC *) file);
  */

  Lorito_File *file = lorito_file_new(interp, filename);
  interp->files[fileid] = file;

  file->fileid = fileid;

  while (!feof(input))
  {
    int read = 0;
    int typed = 0;
    int flags = 0;

    read = fread(&typed, sizeof(int), 1, input);
    if (read == 0)
      break;
    // or die

    read = fread(&flags, sizeof(int), 1, input);
    if (read == 0)
      break;
    // or die

    if (typed == SEG_code)
    {
      int length = 0;

      read = fread(&length, sizeof(int), 1, input);
      // or die
      char *name = (char *) malloc(sizeof(char) * length);
      read = fread(name, sizeof(char), length, input);
      // or die

      read = fread(&length, sizeof(int), 1, input);
      // or die
      Lorito_Opcode *op = (Lorito_Opcode *) malloc(sizeof(Lorito_Opcode) * length);
      read = fread(op, sizeof(Lorito_Opcode), length, input);
      // or die

      file->codesegs = realloc(file->codesegs, (file->codeseg_count+1)  * sizeof(Lorito_Constseg *));
      if (interp->files == NULL)
        abort();

      Lorito_Codeseg *codeseg = lorito_code_block_new(interp, name, length, op);
      file->codesegs[file->codeseg_count] = codeseg;
      file->codeseg_count++;

      codeseg->fileid = fileid;
      codeseg->file = file;
      codeseg->segid = segid++;
      codeseg->flags = flags;

      interp->last_seg = codeseg;

      //printf("SegNum:  %d\n", codeseg->segid);
      //printf("SegName: %s\n", codeseg->name);

    }

    if (typed == SEG_const)
    {
      int length = 0;

      read = fread(&length, sizeof(int), 1, input);
      // or die
      char *name = (char *) malloc(sizeof(char) * length);
      read = fread(name, sizeof(char), length, input);
      // or die

      read = fread(&length, sizeof(int), 1, input);
      // or die

      void *data = (void *) malloc(sizeof(Lorito_Opcode) * length);
      read = fread(data, sizeof(Lorito_Opcode), length, input);
      // or die

      file->constsegs = realloc(file->constsegs, (file->constseg_count+1) * sizeof(Lorito_Constseg *));
      if (interp->files == NULL)
        abort();

      Lorito_Constseg *constseg = lorito_const_block_new(interp, name, length, data);

      constseg->fileid = fileid;
      constseg->file = file;
      constseg->segid = segid++;
      constseg->flags = flags;

      file->constsegs[file->constseg_count] = constseg;
      file->constseg_count++;

      //printf("DSegNum:  %d\n", constseg->segid);
      //printf("DSegName: %s\n", constseg->name);

    }

    if (typed == SEG_datadef)
    {
      int length = 0;

      read = fread(&length, sizeof(int), 1, input);
      // or die
      char *name = (char *) malloc(sizeof(char) * length);
      read = fread(name, sizeof(char), length, input);
      // or die

      read = fread(&length, sizeof(int), 1, input);
      // or die

      file->datadefsegs = realloc(file->datadefsegs, (file->datadefseg_count+1) * sizeof(Lorito_Datadefseg *));
      if (file->datadefsegs == NULL)
        abort();

      Lorito_Datadefseg *datadefseg = lorito_datadef_block_new(interp, name, length);

      datadefseg->fileid = fileid;
      datadefseg->file = file;
      datadefseg->segid = segid++;
      datadefseg->flags = flags;

      file->datadefsegs[file->datadefseg_count] = datadefseg;
      file->datadefseg_count++;

      //printf("DSegNum:  %d\n", constseg->segid);
      //printf("DSegName: %s\n", constseg->name);

    }
  }

  /* Run the init code for every init codeseg */
  int i;
  for (i=0; i < file->codeseg_count; i++)
  {
    if ((file->codesegs[i]->flags & SEG_FLAG_init) == SEG_FLAG_init)
    {
      interp->ctx = lorito_ctx_new(interp, interp->ctx, (Lorito_PMC *) file->codesegs[i]);
      core_exec(interp);
    }
  }
  
  return file;
}
