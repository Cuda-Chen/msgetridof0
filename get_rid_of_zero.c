#include <stdio.h>

#include "libmseed.h"

#include "get_rid_of_zero.h"

static void
recordHandler (char *record, int reclen, void *handlerData)
{
  MS3Record *msr = NULL;

  if (!msr3_parse (record, reclen, &msr, 0, 0))
  {
    msr3_print (msr, 0);
  }
  else
  {
    fprintf (stderr, "%s() Error parsing record\n", __func__);
  }

  msr3_free (&msr);
}

int
getRidOfZero (const char *inputfile, const char *outputfile)
{
  MS3Record *inputmsr      = NULL;
  MS3TraceList *outputmstl = NULL;
  uint32_t flags           = 0;
  int8_t verbose           = 0;
  int rv;

  int64_t psamples;
  int precords;
  int reclen = 512;
  //uint8_t encoding = DE_INT32;
  uint8_t encoding = DE_STEIM2;

  flags |= MSF_VALIDATECRC;
  flags |= MSF_UNPACKDATA;
  flags |= MSF_MAINTAINMSTL;
  flags |= MSF_PACKVER2;

  outputmstl = mstl3_init (NULL);
  if (outputmstl == NULL)
  {
    ms_log (2, "Cannot allocate memory for tracelist.\n");
    return -1;
  }

  /* Loop over the input file record by record */
  while ((rv = ms3_readmsr (&inputmsr, inputfile, NULL, NULL,
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
      if (inputmsr->sampletype == 'i')
      {
        value = (double)(*(int32_t *)sptr);
      }
      else if (inputmsr->sampletype == 'f')
      {
        value = (double)(*(float *)sptr);
      }
      else if (inputmsr->sampletype == 'd')
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
      if (mstl3_addmsr (outputmstl, inputmsr, 0, 1, flags, NULL) == NULL)
      {
        ms_log (2, "mstl3_addmsr() had problems\n");
        break;
      }

      /*ms_log (0, "Calling mstl3_pack() to generate records\n");
      precords = mstl3_pack (outputmstl,
                             recordHandler,
                             NULL,
                             reclen,
                             encoding,
                             &psamples,
                             flags,
                             verbose,
                             NULL);
      ms_log (0, "mstl3_pack() created %d records containing %" PRId64 " samples\n",
              precords, psamples);*/
    }
  }

  /* flush data */
  //ms_log (0, "Calling mstl3_pack() with MSF_FLUSHDATA flag\n");
  ms_log (0, "Calling mstl3_pack()\n");
  precords = mstl3_pack (outputmstl,
                         recordHandler,
                         NULL,
                         reclen,
                         encoding,
                         &psamples,
                         (flags | MSF_FLUSHDATA),
                         verbose,
                         NULL);
  ms_log (0, "Final mstl3_pack() created %d records containing %" PRId64 " samples\n",
          precords, psamples);

  /* Output the tracelist. */
  precords = mstl3_writemseed (outputmstl, outputfile, 1, reclen,
                               encoding, flags, verbose);

  /* To make sure everything is clean up. */
  if (inputmsr)
  {
    msr3_free (&inputmsr);
  }
  if (outputmstl)
  {
    mstl3_free (&outputmstl, 0);
  }

  return 0;
}
