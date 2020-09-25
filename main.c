#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "get_rid_of_zero.h"

static void usage ();

int
main (int argc, char **argv)
{
  char *mseedfile = NULL;
  //char *outputfile = "output.mseed";
  char *outputfile;
  char *rzExtension = ".rz";
  int rv;

  if (argc != 2)
  {
    usage ();
    return -1;
  }

  mseedfile  = argv[1];
  int len    = strlen (mseedfile);
  char *temp = (char *)malloc (sizeof (char) * (len + 1));
  strncpy (temp, mseedfile, len);
  temp[len] = '\0';
  int l     = 0;
  char *ssc = strstr (temp, "/");
  while (ssc)
  {
    l    = strlen (ssc) + 1;
    temp = &temp[strlen (temp) - l + 2];
    ssc  = strstr (temp, "/");
  }
  size_t tempLen = strlen (temp);
  outputfile     = (char *)malloc (sizeof (char) * (1 + tempLen + strlen (rzExtension)));
  strcpy (outputfile, temp);
  strcat (outputfile, rzExtension);

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
