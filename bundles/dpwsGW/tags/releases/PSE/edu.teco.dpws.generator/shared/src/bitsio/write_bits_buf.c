/* ========================================================================
 *
 *        Filename   : write_bits_buf.c
 *        Description:
 *        Version    : 1.0
 *        (C)        : Dimitar Yordanov (domidimi [at] gmail [dot] com), Till Riedel
 *        ---- History ---------------------------------------------------
 *        05.04.2008 21:43    dy    initial version
 *        25.08.2008 21:43    tr 	complete rewrite
 * ========================================================================
 */
#define  _WRITE_BITS_C_
#include <stdint.h>
#include <stddef.h>
#include <assert.h>


struct WRITER_STRUCT {
	size_t byte_pos; /*byte byte_position*/
	uint8_t bit_pos; /*sub-byte byte_position*/
	char *buf; /*byte buf*/
	char lastByte; /*sub-byte buf*/
	size_t size;/*TODO: unchecked*/
};

#include <bitsio/write_bits.h>

/* ========================================================================
 *
 *
 * ========================================================================*/
size_t write_bits_bufwriter_size = sizeof(struct WRITER_STRUCT);

struct WRITER_STRUCT * write_bits_bufwriter_init(struct WRITER_STRUCT *writer,
		char *buf, size_t size) {
	write_bits_bufwriter_size=write_bits_bufwriter_size; /*eliminate warning*/
	if (writer) {
		writer->buf = buf;
		writer->lastByte = 0;

		writer->byte_pos = 0;
		writer->bit_pos = 0;
		writer->size=size;
	}
	return writer;
}



static void write_full(uint8_t *out, uint8_t *rest, uint8_t *in, uint8_t offset)
{
	uint16_t temp;           	    /*             rest  | in		*/
							        /*          [XXXXX000|87654321] */
	temp= (uint16_t)*in << (8-offset);     	/* offset=5 [00000876|54321000] */
	*out = *rest | (temp>>8);      /*           [XXXXX876|54321000] */
    *rest=(temp&0xff);       	    /*			[54321000|        ] */
}

static int write_part(uint8_t *out, uint8_t *rest,  uint8_t *in, uint8_t offset, uint8_t len)
{
	if(offset+len>=8)
	{
		uint16_t temp;				      /*             rest  | in		  */
										  /*          [YYYYY000|00654321] */
		temp= (uint16_t)*in << ((8-offset)+(8-len));/* o=5,l=6  [00000654|32100000] */
		*out = *rest | (temp>>8);        /*          [YYYYY654|32100000] */
		*rest=(temp & 0xff);             /*          [32100000]*/
	}
	else
	{
		*rest |= *in << (8- offset - len); /*o=1,l=6  [Y000000|00654321] -> [Y6543210] */
	}

	return offset+len; /* o=5,l=6 -> 8 + 3 /// o=1, l=7 -> 0 + 7*/
}


/* ========================================================================
 *
 * shift consecutive bits into buffer high first...
 * TODO: Fix for big endian!!
 * @return the number of bytes written
 *
 * ======================================================================*/
#define write_bits_buf_little write_bits


ssize_t write_bits_buf_little(struct WRITER_STRUCT* writer, void *buf,
		size_t bits_len) {
	int old_byte_pos;

	old_byte_pos= writer->byte_pos;

	{
		signed int i;
		i=((bits_len-1)/8);
		/*unroll for last bits*/

		if(bits_len%8)
		{
			int offset;
			offset=write_part((uint8_t *)writer->buf+writer->byte_pos,(uint8_t *)&writer->lastByte,(uint8_t *)buf+i, writer->bit_pos,bits_len%8);

			writer->byte_pos+=offset/8;
			writer->bit_pos=offset%8;
			i--;
		}

		if (writer->bit_pos == 0)
			for(;i>=0;i--)
			{
				writer->buf[writer->byte_pos]=((char *)buf)[i];
				writer->byte_pos++;
			}
		else
			for(;i>=0;i--)
			{
				write_full((uint8_t *)writer->buf+writer->byte_pos,(uint8_t *)&writer->lastByte,(uint8_t *)buf+i,writer->bit_pos);
				writer->byte_pos++;
			}
	}


	return writer->byte_pos-old_byte_pos;
}

ssize_t write_bit(struct WRITER_STRUCT *writer, uint8_t bit)
{
  return write_bits(writer,&bit,1);
}

/* ========================================================================
 * Flush
 * ========================================================================*/

ssize_t write_finish(struct WRITER_STRUCT* writer) {
	ssize_t ret = 0;

	if (writer->bit_pos) {
		writer->buf[ writer->byte_pos]=writer->lastByte;
		ret = 1;
	}

	return ret;
}

ssize_t write_buf_finish(struct WRITER_STRUCT* writer) {
	return writer->byte_pos + write_finish(writer);
}
