#ifndef _READ_BITS_H_
#define _READ_BITS_H_

#include <stdlib.h>
#ifndef CCSC
#include <sys/types.h>
#endif

#ifndef  _READ_BITS_C_
struct READER_STRUCT;
#endif

void    read_init   (struct READER_STRUCT * reader, int fd);
ssize_t read_bits   (struct READER_STRUCT * reader, void *dst_buf, size_t bits_len);

int read_bit( struct READER_STRUCT *reader);

#endif
