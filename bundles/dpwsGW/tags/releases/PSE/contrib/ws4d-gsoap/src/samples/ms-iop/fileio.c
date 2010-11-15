/* <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2007  University of Rostock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#include "fileio.h"
#include <string.h>

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
    return -1;
  while ((nread = fread (bytes + res, 1, BYTESCHUNK, f)) == BYTESCHUNK)
    {
      res += nread;
    }
  if (nread >= 0 && feof (f))
    res += nread;
  else
    res = -1;
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

int
comparefiles (const char *filename1, const char *filename2)
{
  FILE *f1 = fopen (filename1, "rb");
  FILE *f2 = fopen (filename2, "rb");
  int res = 0, nread1, nread2;
  unsigned char bytes1[BYTESCHUNK], bytes2[BYTESCHUNK], *b1, *b2;
  while ((nread1 = fread (bytes1, 1, BYTESCHUNK, f1)) == (nread2 =
                                                          fread (bytes2, 1,
                                                                 BYTESCHUNK,
                                                                 f2)))
    {

      b1 = bytes1;
      b2 = bytes2;

      if (nread1 == 0)
        {
          res = 0;
          goto end;
        }
      while ((b1 < bytes1 + nread1) && *b1++ == *b2++);
      if (b1 < bytes1 + nread1)
        {
          res += (b1 - bytes1);
          goto end;
        }
      else
        res += nread1;
    }
  b1 = bytes1;
  b2 = bytes2;
  while ((b1 < bytes1 + nread1) && (b2 < bytes2 + nread2) && *b1++ == *b2++);
  res += (b1 - bytes1);
end:
  fclose (f1);
  fclose (f2);
  return res;
}
