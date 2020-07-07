#include <stdio.h>

static void usage ();

int
main (int argc, char **argv)
{
  if (argc != 2)
  {
    usage ();
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
