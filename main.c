#include <stdio.h>

#include "get_rid_of_zero.h"

static void usage ();

int
main (int argc, char **argv)
{
  char *mseedfile  = NULL;
  char *outputfile = "output.mseed";
  int rv;

  if (argc != 2)
  {
    usage ();
    return -1;
  }

  mseedfile = argv[1];

  rv = getRidOfZero (mseedfile, outputfile);
  if (rv != 0)
  {
    return -1;
  }

  return 0;
}

static void
usage ()
{
  printf ("Usage: msgetridof0 [mseedfile]\n"
          "\n## Options ##\n"
          " mseedfile      input miniSEED file\n");
}
