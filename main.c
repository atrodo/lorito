// This is the actual main program

#include "lorito.h"
#include "microcode.h"
#include "interp.h"
#include "loader.h"

int
main(int argc, const char *argv[])
{
  int i;
  Lorito_Interp *interp;

  if (argc < 2)
  {
    fprintf(stderr, "Usage: lorito <bytecodefiles>\n");
    return 255;
  }

  interp = lorito_init();

  for (i = 1; i < argc; i++)
  {
    loadbc(interp, argv[i]);
  }

  int retval = lorito_run(interp);
  if (retval != 0)
  {
    fprintf(stderr, "%s\n", interp->last_error->original);
  }
}
