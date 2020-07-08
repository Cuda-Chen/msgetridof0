
#include "libmseed.h"

#include "get_rid_of_zero.h"

int
getRidOfZero (const char *inputfile, const char *outputfile)
{
  MS3Record *inputmsr     = NULL;
  MS3TraceList *outputmstl = NULL;
  uint32_t flags     = 0;
  int8_t verbose     = 0;
  int rv;

  int64_t psamples;
  int precords;
  int reclen       = 1024;
  uint8_t encoding = DE_INT32;

  flags |= MSF_VALIDATECRC;
  flags |= MSF_UNPACKDATA;
  flags |= MSF_PACKVER2;

  /* Loop over the input file record by record */
  while ((rv = ms3_readinputmsr (&inputmsr, inputfile, NULL, NULL,
                            flags, verbose)) == MS_NOERROR)
  {
    double squareSum = 0.0, value;
    int idx, samplesize;
    void *sptr;

    if ((samplesize = ms_samplesize (inputmsr->sampletype)) == 0)
    {
      ms_log (2, "Unrecognized sample type: '%c'\n", inputmsr->sampletype);
      return -1;
    }

    for (idx = 0; idx < inputmsr->numsamples; idx++)
    {
      sptr = (char *)inputmsr->datasamples + (idx * samplesize);
      if (sampletype == 'i')
      {
        value = (double)(*(int32_t *)sptr);
      }
      else if (sampletype == 'f')
      {
        value = (double)(*(float *)sptr);
      }
      else if (sampletype == 'd')
      {
        value = (double)(*(double *)sptr);
      }
      else
      {
        ms_log (2, "Contains non-numerical data.\n");
        return -1;
      }

      squareSum += (value * value);
    }

    /* If the values of this record are all zero (i.e., the dumb of CWB workaround) */
    /* else add this record to output tracelist. */
    if (!(squareSum > 0.0))
    {
      continue;
    }
    else
    {
      if (outputmstl3_addinputmsr (outputmstl, inputmsr, 0, 1, flags, NULL) == NULL)
      {
        ms_log (2, "outputmstl3_addinputmsr() had problems\n");
        break;
      }

      precords = outputmstl3_pack (outputmstl,
                             NULL,
                             NULL,
                             reclen,
                             encoding,
                             &psamples,
                             flags,
                             verbose,
                             NULL);
    }
  }

  /* flush data */
  precords = outputmstl3_pack (outputmstl,
                         NULL,
                         NULL,
                         reclen,
                         encoding,
                         &psamples,
                         (flags | MSF_FLUSHDATA),
                         verbose,
                         NULL);

  /* Output the tracelist. */
  precords = outputmstl3_writemseed (outputmstl, outputfile, 1, reclen,
                               encoding, flags, verbose);

  /* To make sure everything is clean up. */
  if (inputmsr)
  {
    inputmsr3_free (&inputmsr);
  }
  if (outputmstl)
  {
    outputmstl3_free (&outputmstl);
  }

  return 0;
}
