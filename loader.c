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

  while (!feof(input))
  {
    Lorito_Codeseg *codeseg;
    int read = 0;
    int typed = 0;

    read = fread(&typed, sizeof(int), 1, input);
    if (read == 0)
      break;
    // or die

    if (typed == SEG_code)
    {
      int length = 0;

      codeseg = (Lorito_Codeseg *) malloc(sizeof(Lorito_Codeseg));
      codeseg->fileid = fileid;
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
      Lorito_Dataseg *dataseg;
      int length = 0;

      dataseg = (Lorito_Dataseg *) malloc(sizeof(Lorito_Dataseg));
      dataseg->fileid = fileid;
      dataseg->segid = segid++;

      read = fread(&length, sizeof(int), 1, input);
      // or die
      dataseg->name = (char *) malloc(sizeof(char) * length);
      read = fread(dataseg->name, sizeof(char), length, input);
      // or die

      read = fread(&length, sizeof(int), 1, input);
      // or die
      dataseg->length = length;
      dataseg->data = (void *) malloc(sizeof(Lorito_Opcode) * length);
      read = fread(dataseg->data, sizeof(Lorito_Opcode), length, input);
      // or die

      //interp->last_seg = codeseg;
    }
  }

}
