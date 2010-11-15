#ifndef _READ_BITS_BUF_H_
#define _READ_BITS_BUF_H_
#include <bitsio/read_bits.h>

#include <stdlib.h>
#ifdef __GNUC__
#include <alloca.h>
#endif

#ifndef u_char
#define u_char unsigned char
#endif

#ifndef _READ_BITS_C_
extern const size_t read_bits_bufreader_size;
#endif

struct READER_STRUCT *read_bits_bufreader_init(struct READER_STRUCT *,char *buf, size_t size);

#define read_bits_bufreader_stack_new(buf,size)  read_bits_bufreader_init((struct READER_STRUCT *)alloca(read_bits_bufreader_size),buf,size)

#define read_bits_bufreader_new(buf) read_bits_bufreader_init(malloc(read_bits_bufreader_size),buf,size)
#define read_bits_bufreader_delete(w) free(w)

#endif
