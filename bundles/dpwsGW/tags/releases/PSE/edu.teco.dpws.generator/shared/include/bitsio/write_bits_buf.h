#ifndef _WRITE_BITS_BUF_H_
#define _WRITE_BITS_BUF_H_
#include <bitsio/write_bits.h>
#include <stdlib.h>
#ifdef __GNUC__
#include <alloca.h>
#endif

#ifndef u_char
#define u_char unsigned char
#endif

#ifndef _WRITE_BITS_C_
extern const size_t write_bits_bufwriter_size;
#endif

struct WRITER_STRUCT *write_bits_bufwriter_init(struct WRITER_STRUCT *, char *buf, size_t size);

#define write_bits_bufwriter_stack_new(buf,size)  write_bits_bufwriter_init((struct WRITER_STRUCT *)alloca(write_bits_bufwriter_size),buf,size)

#define write_bits_bufwriter_new(w,buf,size) write_bits_bufwriter_init(malloc(write_bits_bufwriter_size),buf)
#define write_bits_bufwriter_delete(w) free(w)

ssize_t write_buf_finish(struct WRITER_STRUCT* writer);

#endif
