/* ========================================================================
 *
 *        Filename   : read_bits_buf.c
 *        Description:
 *        Version    : 1.0
 *        (C)        : Dimitar Yordanov (domidimi [at] gmail [dot] com)
 *        ---- History ---------------------------------------------------
 *        05.04.2008 21:43    dy    initial version
 * ========================================================================
 */
#define  _READ_BITS_C_ 1
#include <stdint.h>
#ifndef CCSC
#include <sys/types.h>
#else
#include <stdlib.h>
#endif


struct READER_STRUCT {
   int byte_pos;
   uint8_t bit_pos;
   char * buf;
   size_t size;/*TODO: unchecked*/
};

#include <bitsio/read_bits.h>

 size_t read_bits_bufreader_size=sizeof(struct READER_STRUCT);
/* ========================================================================
 *
 *
 * ========================================================================*/
struct READER_STRUCT * read_bits_bufreader_init(struct READER_STRUCT *reader, char *src_buf, size_t size)
{
   reader->buf  = (char *)src_buf;
   reader->byte_pos    = 0;
   reader->bit_pos = 0;
   reader->size=size;
   read_bits_bufreader_size=read_bits_bufreader_size; /*to make read_bits_bufreader_size used*/
   return reader;
}

static void read_full( uint8_t *in, uint8_t *rest, uint8_t *out, uint8_t offset)
{
	uint16_t temp;             					/*              rest     |  in      */
	temp= ((uint16_t)*rest<<8)|(*in);   					/* offset=5 -> 	[XXXXX876 | 54321YYY] */
	*out= (uint8_t) (temp >> (8-offset)); 	   	/* 				[00000XXX | 87654321] */
}

static int read_part( uint8_t *in, uint8_t *rest, uint8_t *out, uint8_t offset, uint8_t len)
{
	{
		uint16_t temp;                       /*              rest     |  in      */
		temp= ((uint16_t)*rest<<8)|(*in);     		 /* o=5, l=6 -> [XXXXX654 |321YYYYY] */
		*out= (uint8_t) (temp >> ((8-offset)%8+(8-len)));/* [000XXXXX |XX654321] */
		*out&=(1<<len)-1;					         /* [          00111111] */
	}

	return offset+len;
}

/* ========================================================================
 *
 *
 * ========================================================================*/
#define read_bits_buf_little read_bits
ssize_t read_bits_buf_little(struct READER_STRUCT *reader, void *buf, size_t bits_len)
{
	int old_byte_pos;

	old_byte_pos= reader->byte_pos;

	{
		signed int i;
		i=((bits_len-1)/8);
		/*unroll for last bits*/
		if(bits_len%8)
		{
			int ret;
			ret=read_part((uint8_t*) reader->buf+reader->byte_pos,(uint8_t*) reader->buf+(reader->byte_pos-((reader->byte_pos==0)?0:1)),(uint8_t*)buf+i, reader->bit_pos,bits_len%8);

			if(reader->bit_pos==0) /*pos before read!*/
				reader->byte_pos++;
			else if(ret>8)
				reader->byte_pos++;

			reader->bit_pos=ret%8;
			i--;
		}

		if (reader->bit_pos == 0)
		{	for(;i>=0;i--)
			{
				((char *)buf)[i]=reader->buf[reader->byte_pos];
				reader->byte_pos++;
			}
		}
		else
			for(;i>=0;i--)
			{
				read_full((uint8_t*) reader->buf+reader->byte_pos,(uint8_t*) reader->buf+(reader->byte_pos-((reader->byte_pos==0)?0:1)),(uint8_t*) buf+i,reader->bit_pos);
				reader->byte_pos++;
			}
	}

	return  reader->byte_pos-old_byte_pos;
}

int read_bit( struct READER_STRUCT *reader)
{
   int val=0;
   read_bits(reader, &val, 1);
   return val;
}
