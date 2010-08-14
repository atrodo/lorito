// This will load a lorito bytecode file into a lorito codeseg

// Since this is temporary, and we'll end up throwing it away in favor of
//  integrating with parrot's packfile format, this will be real simple.
//
// Integer: segment type (0 = code, 1 = data)
//   Integer: Size of segement name
//   String: segment name, null terminated
//   Integer: Count (in 8 bytes, so a count of 1 == 8 bytes)
//   Data

#include "lorito.h"
#include <stdio.h>

void
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

  Lorito_File *file = &interp->files[fileid];
  file->fileid = fileid;
  file->name = filename;
  file->codeseg_count = 0;
  file->dataseg_count = 0;
  file->codesegs = NULL;
  file->datasegs = NULL;

  while (!feof(input))
  {
    int read = 0;
    int typed = 0;

    read = fread(&typed, sizeof(int), 1, input);
    if (read == 0)
      break;
    // or die

    if (typed == SEG_code)
    {
      file->codesegs = realloc(file->codesegs, file->codeseg_count+1);
      if (interp->files == NULL)
        abort();
      //Lorito_Codeseg *codeseg = file->codesegs[file->codeseg_count];
      Lorito_Codeseg *codeseg = (Lorito_Codeseg *) malloc(sizeof(Lorito_Codeseg));
      file->codesegs[file->codeseg_count] = codeseg;
      file->codeseg_count++;

      int length = 0;

      //codeseg = (Lorito_Codeseg *) malloc(sizeof(Lorito_Codeseg));
      codeseg->fileid = fileid;
      codeseg->file = file;
      codeseg->segid = segid++;

      read = fread(&length, sizeof(int), 1, input);
      // or die
      codeseg->name = (char *) malloc(sizeof(char) * length);
      read = fread(codeseg->name, sizeof(char), length, input);
      // or die

      read = fread(&length, sizeof(int), 1, input);
      // or die
      codeseg->length = length;
      codeseg->op = (Lorito_Opcode *) malloc(sizeof(Lorito_Opcode) * length);
      read = fread(codeseg->op, sizeof(Lorito_Opcode), length, input);
      // or die

      interp->last_seg = codeseg;
    }

    if (typed == SEG_data)
    {
      file->datasegs = realloc(file->datasegs, file->dataseg_count+1);
      if (interp->files == NULL)
        abort();
      //Lorito_Dataseg *dataseg = file->datasegs[file->dataseg_count];
      Lorito_Dataseg *dataseg = (Lorito_Dataseg *) malloc(sizeof(Lorito_Dataseg));
      file->datasegs[file->dataseg_count] = dataseg;
      file->dataseg_count++;

      int length = 0;

      //dataseg = (Lorito_Dataseg *) malloc(sizeof(Lorito_Dataseg));
      dataseg->fileid = fileid;
      dataseg->file = file;
      dataseg->segid = segid++;

      read = fread(&length, sizeof(int), 1, input);
      // or die
      dataseg->name = (char *) malloc(sizeof(char) * length);
      read = fread(dataseg->name, sizeof(char), length, input);
      // or die

      read = fread(&length, sizeof(int), 1, input);
      // or die
      dataseg->length = length * sizeof(Lorito_Opcode);
      dataseg->data = (void *) malloc(sizeof(Lorito_Opcode) * length);
      read = fread(dataseg->data, sizeof(Lorito_Opcode), length, input);
      // or die

      //interp->last_seg = codeseg;
    }
  }

}
