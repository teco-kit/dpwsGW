#include <gtest/gtest.h>

extern "C"
{
#include <asm/byteorder.h>
#include <bitsio/write_bits.h>
#include <bitsio/write_bits_buf.h>

#include <bitsio/read_bits.h>
#include <bitsio/read_bits_buf.h>
}

#ifdef __LITTLE_ENDIAN
#define get(i,n)  ((n-1)-i)
#else
#define get(i,n) i
#endif

TEST(BitsIO,basicWriterCheck)
{

	char buf[64];
	memset(buf, 0xaa, sizeof(buf));

	struct WRITER_STRUCT *writer =
			write_bits_bufwriter_stack_new(buf,sizeof(buf));

	ASSERT_TRUE(NULL!=writer);

	char buf2[]=
	{	0,1,2,3,4,5,6,7};
	ssize_t written;

	written=write_bits(writer,buf2,sizeof(buf2)*8);
	written+=write_finish(writer);

	EXPECT_EQ(sizeof(buf2),written);

	for(int i=0;i<sizeof(buf2);i++)
	{
		EXPECT_EQ(buf2[i],buf[get(i,sizeof(buf2))]);
	}

}

TEST(BitsIO,basicWriterCheckUnalligned)
{
	char buf[64];
	struct WRITER_STRUCT *writer =
			write_bits_bufwriter_stack_new(buf,sizeof(buf));

	memset(buf, 0xaa, sizeof(buf));

	ssize_t written = 0;
	int one = 1;

	for (int i = 0; i < 8; i++)
	{
		written += write_bits(writer, (char *) &one, 7);
	}
	written += write_finish(writer);

	EXPECT_EQ((8*7)/8,written);

	for(int i=0;i<6;i++)
	{
		EXPECT_EQ(1<<(i+1),(uint8_t)buf[i]);
	}

	EXPECT_EQ((1<<(6+1))+1,(uint8_t)buf[6]);
}

TEST(BitsIO,basicReaderCheck)
{
	char buf[64];
	char buf2[] =
	{ 0, 1, 2, 3, 4, 5, 6, 7 };
	struct READER_STRUCT *reader =
			read_bits_bufreader_stack_new(buf2,sizeof(buf2));

	memset(buf, 0xaa, sizeof(buf));

	ASSERT_TRUE(NULL!=reader);

	ssize_t read;

	read=read_bits(reader,buf,sizeof(buf2)*8);

	EXPECT_EQ(sizeof(buf2),read);

	for(int i=0;i<sizeof(buf2);i++)
	{
		EXPECT_EQ(buf2[get(i,sizeof(buf2))],buf[i]);
	}
}

TEST(BitsIO,basicReaderUnalignedCheck)
{
	char buf2[] =
	{ 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, (1 << 7) + 1 };

	struct READER_STRUCT *reader =
			read_bits_bufreader_stack_new(buf2,sizeof(buf2));

	ssize_t read = 0;

	for (int i = 0; i < 8; i++)
	{
		int one = 0xAA;
		read += read_bits(reader, (char*) &one, 7);
		EXPECT_EQ(1,one);
	}

	EXPECT_EQ(7,read);
}

TEST(BitsIO,basicTypesReaderWriter)
{
	char buf[64];
	memset(buf, 0xcc, sizeof(buf));

	uint8_t one = 1, one_ = 0xAA;
	uint8_t two = 2, two_ = 0xAA;
	uint16_t otwo = 2002, otwo_ = 0xAAAA;
	uint32_t deadbeaf = 0xdeadbeaf, deadbeaf_ = 0xAAAAAA, deadbeaf2_ = 0xBBBBBB;
	{
		struct WRITER_STRUCT *writer =
				write_bits_bufwriter_stack_new(buf,sizeof(buf));

		write_bits(writer, &one, 1);

		write_bits(writer, &two, 3);

		write_bits(writer, &otwo, 12);

		write_bits(writer, &deadbeaf, 32);

		write_true(writer);

		write_false(writer);

		write_true(writer);

		write_bits(writer, &deadbeaf, 32);

		write_finish(writer);
	}

	{
		struct READER_STRUCT *readr =
				read_bits_bufreader_stack_new(buf,sizeof(buf));

		read_bits(readr, &one_, 1);
		EXPECT_EQ((int)one,(int)one_);

		read_bits(readr,&two_,3);
		EXPECT_EQ((int)two,(int)two_);

		read_bits(readr,&otwo_,12);
		EXPECT_EQ((int)otwo,(int)otwo_);

		read_bits(readr,&deadbeaf_,32);
		EXPECT_EQ((int)deadbeaf,(int)deadbeaf_);

		EXPECT_TRUE(read_bit(readr));

		EXPECT_FALSE(read_bit(readr));

		EXPECT_TRUE(read_bit(readr));

		read_bits(readr,&deadbeaf2_,32);

		EXPECT_EQ((int)deadbeaf,(int)deadbeaf2_);
	}
}
