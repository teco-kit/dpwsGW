#define NDEBUG 1
#define CCSC 1

#include <STDLIB.H>
#include <bitsio/read_bits_buf.c>
#include <bitsio/write_bits_buf.c>


#define __attribute__(X)
#include <sens_types.h>

#include <Sample_dom2bin.c>
#include <Sample_bin2dom.c>

#define TEST_NOT_RUN -1
#define TEST_SUCCESS 0
#define TEST_FAIL 1
char test_buf[64]={};

int _test_Sample_in_out(sens_SSimpSample *in)
{
  sens_SSimpSample _out={};
  sens_SSimpSample *out;
  
  ssize_t dat_len,out_len;
  memset(test_buf,0,64);
  
  out=&_out;
  {
	   struct WRITER_STRUCT writer;
	   write_bits_bufwriter_init(&writer,test_buf,64);
	   Sample_dom2bin_run(in, &writer);
       dat_len=write_buf_finish(&writer);
  }

  {
     struct READER_STRUCT reader;
     read_bits_bufreader_init(&reader,test_buf,64);
     Sample_bin2dom_run(&reader,out);
	 //out_len=*read_buf_finish(&reader);
  }

 { 
   int i;
   char *a,*b;
   a=(char*)out;
   b=(char*)in;
   for(i=0;i<sizeof(sens_SSimpSample);i++)
   {
	 char ac,bc;
 	 ac=a[i];
	 bc=b[i];  
     if(ac!=bc) return TEST_FAIL+1;
   }
 }

   return TEST_SUCCESS;
}

int test_Sample_in_out1 ()
{
   sens_SSimpSample empty={};
   return _test_Sample_in_out(&empty);
}

int test_Sample_in_out2 ()
{
   sens_SSimpSample s={};
	{
		sens_ADXL210Sample *accl;
	    accl	= sens_SSimpSample_add_accelleration(&s);

		accl->x = 0x2AAA;
		accl->y = 0x7FFF;
		accl->z = -0x7F01;
		}
   
   return _test_Sample_in_out(&s);
}



#define test_init(NAME) int ret_##NAME=TEST_NOT_RUN
#define test_run(NAME) ret_##NAME=test_##NAME()

void PRSTest1()
{
   test_init(Sample_in_out1);
   test_init(Sample_in_out2);
   test_run(Sample_in_out1);
   test_run(Sample_in_out2);
   for(;;);
}