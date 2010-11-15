#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef unix
# include <unistd.h>
# include <utime.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <direct.h>
# include <io.h>
#endif

#include "zlib.h"

#ifdef WIN32
#define USEWIN32IOAPI
#include "ziplib-files/iowin32.h"
#include "ziplib-files/ioapi.h"
#endif

#include "fileio.h"
#include "unzip.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

#ifdef WIN32
#  define SNPRINTF _snprintf
#  pragma warning(disable : 4996)       /* Remove snprintf Secure Warnings */
#else
#define SNPRINTF snprintf
#endif

#define BYTESCHUNK 4096

int
readfile (const char *filename, unsigned char *bytes)
{
  FILE *f = fopen (filename, "rb");
  int res = 0, nread;
  if (!f)
    return -3;
  while ((nread = fread (bytes + res, 1, BYTESCHUNK, f)) == BYTESCHUNK)
    {
      res += nread;
    }
  if (nread >= 0 && feof (f))
    res += nread;
  else
    res = -2;
  fclose (f);
  return res;
}

int
writefile (const char *filename, unsigned char *bytes, size_t len)
{
  FILE *f = fopen (filename, "wb");
  int nwritten = 0;
  if (!f)
    return -1;
  while (1)
    {
      int n = len - nwritten;
      if (n > BYTESCHUNK)
        {
          fwrite (bytes + nwritten, 1, BYTESCHUNK, f);
          nwritten += BYTESCHUNK;
        }
      else
        {
          fwrite (bytes + nwritten, 1, n, f);
          fclose (f);
          return 0;
        }
    }
}

/*
   minizip.c
   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant
*/


/* calculate the CRC32 of a file,
   because to encrypt a file, we need known the CRC32 of the file before */
int
getFileCrc (const char *filenameinzip, void *buf, unsigned long size_buf,
            unsigned long *result_crc)
{
  unsigned long calculate_crc = 0;
  int err = Z_OK;
  FILE *fin = fopen (filenameinzip, "rb");
  unsigned long size_read = 0;
  unsigned long total_read = 0;
  if (fin == NULL)
    {
      err = Z_ERRNO;
    }

  if (err == Z_OK)
    do
      {
        err = Z_OK;
        size_read = (int) fread (buf, 1, size_buf, fin);
        if (size_read < size_buf)
          if (feof (fin) == 0)
            {
              err = Z_ERRNO;
            }

        if (size_read > 0)
          calculate_crc = crc32 (calculate_crc, buf, size_read);
        total_read += size_read;

      }
    while ((err == Z_OK) && (size_read > 0));

  if (fin)
    fclose (fin);

  *result_crc = calculate_crc;
  return err;
}



