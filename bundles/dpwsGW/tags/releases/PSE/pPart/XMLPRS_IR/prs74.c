/**
* APPLICATION FILE PRS
* waits for information on when to transmit which sensor values and sends them.
*
* Authors: Sabin Wendhack <wendhack@teco.edu>, Christian Decker <cdecker@teco.edu>, 
* Till Riedel <riedel@teco.edu>
*
*/
#define NDEBUG 1
#define CCSC 1

#include <bitsio/read_bits_buf.c>
#include <bitsio/write_bits_buf.c>

#define __attribute__(X)
#include <sens_types.h>

//#include "debug_dom.h"


#include <Sample_dom2bin.c>

#include <Status_dom2bin.c>
#include <StatusControl_bin2dom.c>

#include <SensorValues_operations.h>

#include "sensorvalues_service.c"

// macros.

// globals.
#define IR_DATA_BIT_COUNT	8
 
uint16_t encoded_data;
unsigned int transfer_count;
unsigned int ir_counter = 0;

// functions.
void PRSInit();
void PRSRun();
void PRSSendSensorValues();

int sendEvent( int  timeout );
int sendPacket (int timeout, uint16_t svc, uint8_t op, char *buf, size_t len);


int sendPacket( int  timeout );

/* dy */
volatile sens_SSimpSample sensors={}; /* {} seems neccessary??? */

static void encode( uint8_t data )
{
	int hamming_exp = 0, p_data_bits = 0, transfer_bit_count, i, j, parity, pos;
	encoded_data = 0;

	for (transfer_bit_count = 0; p_data_bits < IR_DATA_BIT_COUNT; transfer_bit_count++)
	{
		if ((transfer_bit_count + 1) == (1<<hamming_exp))
			hamming_exp++;
		else
		{
			encoded_data |= ((uint16_t) (data>>p_data_bits)&1) <<transfer_bit_count;
			p_data_bits++;
		}
	}
//assert(hamming_exp<=4)
	for (i = 0; i < hamming_exp; i++)
	{
		pos = (1<<i) - 1;
		encoded_data |= 1L<<pos;
		for (j = pos + 1; j < transfer_bit_count; j++)
		{
			if (((j + 1) & (1<<i)) && (encoded_data>>j) & 1)
				encoded_data ^= 1L<<pos;
		}
	}

	parity = 0;
	for (i = 0; i < transfer_bit_count; i++)
		parity ^= encoded_data >> i;

	encoded_data |= ((uint16_t) (parity & 1)) << transfer_bit_count;
	transfer_bit_count++;
	transfer_count = 2 * transfer_bit_count + 3;
}

static int getIRProtocolBit(uint8_t index)
{
	uint16_t ret;
	
	if (index == 0 || index == 1)
		return 1;
	else if ((index % 2) == 0)
		return 0;	
	else
	{
	    ret=(encoded_data>>((index - 3)/2));
		return ret&1;
    }		
}

void PRSInit()
{
init_sensors();
init_time();
ACLSubscribe(ACL_TYPE_ALPHA_ARG('S','V','C'));
encode(ACLGetLocalId()[7]);
}

int has_dat=0,send_enough=0;
unsigned long  PRS_slot_counter=0;
unsigned long  PRS_last_slot_sent=-1;

void PRSRun()
{
	if ((PRS_slot_counter & 1) == 1)
	{
		if (getIRProtocolBit( ir_counter ))
			output_high( PIN_LED_ONE_POWER );
		else
			output_low( PIN_LED_ONE_POWER );

		ir_counter++;
		ir_counter = ir_counter % transfer_count;	
	}

   ++PRS_slot_counter;
   if((PRS_slot_counter&0xff)==0)
	   update_time(); //every 3 seconds, later may need wakeup...

   if(has_dat) PCLedBlueOn();

 has_dat=!get_sensor_values(&sensors, PRS_slot_counter%(1<<15));

#ifdef PIN_LED_AMBER_POWER
   if(has_dat) SSimpLEDAmberOn();
   else  SSimpLEDAmberOff();
#else
    PCLedBlueOff();
#endif
}

void PRSSendSensorValues()
{
   unsigned int timeout;


   if(ACLAddressedDataIsNew()){
	   char *dat;

	  // BuzzerTone(TONE_E5,100);
	   
	   dat=ACLGetReceivedSubjectData();

  	   if(ACLGetType(dat)==ACL_TYPE_ALPHA('S','V','C') && (*(uint16_t*) dat)==0)
	   {


		   dat=ACLGetNextTupleData(dat);
		   if(dat && ACLGetType(dat)==ACL_TYPE_ALPHA('O','P','?'))
		   {
			   uint8_t op;
			   op=*dat;
		   switch(op)
		   {
			   case  OP_SensorValues_GetSensorValues:
				   {
					   sens_SSimpSample out={};
					   char buf[64];
					   ssize_t dat_len;


					   get_sensor_values(&out,-1);
					   {
						   struct WRITER_STRUCT writer;

						   write_bits_bufwriter_init(&writer,buf,64);
						   Sample_dom2bin_run(&out, &writer);
						   dat_len=write_buf_finish(&writer);
					   }
					  sendPacket (20, 0 ,op, buf, dat_len);
					 // BuzzerTone(TONE_E5,100);
				   }
				   break;
			   case  OP_SensorValues_Config:
				   {
					   char buf[64];
					   size_t dat_len;
					   sens_SSimpControl in={};
					   sens_SSimpStatus out={};
				       //BuzzerTone(TONE_A5,100);
					   {
						   struct READER_STRUCT reader;
						   dat=ACLGetNextTupleData(dat);
						   read_bits_bufreader_init(&reader,dat,ACLGetDataLength(dat));
						   StatusControl_bin2dom_run(&reader,&in);
						   //dat_len=*write_buf_finish(&reader);
					   }
					   //BuzzerTone(TONE_H5,100);
					   configure_sensors(&in,&out);
					   //BuzzerTone(TONE_C5,100);
					   {
						   struct WRITER_STRUCT writer;
						   write_bits_bufwriter_init(&writer,buf,64);
						   Status_dom2bin_run(&out, &writer);
						   dat_len=write_buf_finish(&writer);
					   }
					  // BuzzerTone(TONE_D5,100);
					   sendPacket (20, 0 ,op, buf, dat_len);
				   }
				   break;
			   default:
				  // BuzzerTone(TONE_E4,100);
		   }
		   }
	   }
  	   ACLSetDataToOld();
   }

   if(has_dat && (PRS_last_slot_sent != PRS_slot_counter))
   {
	   if(has_dat)
	   {
		   PRS_last_slot_sent = PRS_slot_counter;

		   PCLedRedOn();
		   sendEvent( timeout );
		   has_dat=0;
	   }
	   else if((PRS_slot_counter&0xff)==0)
	   {

		   if(!ACLSendingBusy()&&!ACL_payload_length&&!send_enough)
		   {
			   ACLAddNewType(ACL_TYPE_ALPHA_ARG('A','C','M'));
			   ACLAddNewType(ACL_TYPE_ALPHA_ARG('C','E','H'));
			   {int i; for(i=0;i<8;i++)ACLAddData(255);}
			   ACLSendPacket(1);
		   }
		   else
			   send_enough=0;
	   }
   }
}



int sendPacket (int timeout, uint16_t svc, uint8_t op, char *buf, size_t len)
{
   	PCLedBlueOn();

	ACLAddNewType(ACL_TYPE_ALPHA_ARG('S','V','C'));
	ACLAddDataN(&svc,sizeof(svc));

	ACLAddNewType(ACL_TYPE_ALPHA_ARG('O','P','?'));
	ACLAddDataN(&op,sizeof(op));

	ACLAddNewType(ACL_TYPE_ALPHA_ARG('X','M','L'));
	ACLAddDataN(buf,len);

	ACLSendPacket(timeout);
	while(ACLSendingBusy());
   	PCLedBlueOff();
	send_enough=1;
	return ACLGetSendSuccess();
}

int sendEvent( int  timeout )
{
	ssize_t dat_len=0;
	char buf[64];
	int ret;

	{
		struct WRITER_STRUCT writer;

		write_bits_bufwriter_init(&writer,buf,64);
		Sample_dom2bin_run(&sensors, &writer);
		dat_len=write_buf_finish(&writer);
	}
	PCLedRedOff();
	memset(&sensors,0,sizeof(sens_SSimpSample));




	ret=sendPacket (20, 0 ,OP_SensorValues_SensorValuesEvent, buf, dat_len);

#if 0
	{
		int i;
		for(i=0;i<sizeof(s);i+=48)
		{
			ACLAddNewType(ACL_TYPE_ALPHA_ARG('D','O','M'));
			ACLAddDataN(((char *)&s)+i,((sizeof(s)-i)<48)?(sizeof(s)-i):48);
			ACLSendPacket(20);
			while(ACLSendingBusy());
		}
	}
#endif


	return ret;
}

