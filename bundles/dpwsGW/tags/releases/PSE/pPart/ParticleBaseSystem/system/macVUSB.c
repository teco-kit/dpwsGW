/**
  * Copyright (c) 2004, Telecooperation Office (TecO),
  * Universitaet Karlsruhe (TH), Germany.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions
  * are met:
  *
  *   * Redistributions of source code must retain the above copyright
  *     notice, this list of conditions and the following disclaimer.
  *   * Redistributions in binary form must reproduce the above
  *     copyright notice, this list of conditions and the following
  *     disclaimer in the documentation and/or other materials provided
  *     with the distribution.
  *   * Neither the name of the Universitaet Karlsruhe (TH) nor the names
  *     of its contributors may be used to endorse or promote products
  *     derived from this software without specific prior written
  *     permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/


/*
********************************************************
*/



// this is debug stuff
#define SENDER			0
#define DEBUG			0
#define SYNC_DEBUG		0		// set this to one: on pin 4 is the sync time visible to compare with other devices
//#define LL_PACKET_SENDER 0


#define VERSION			9
#define MY_MAC_ID		141					//141

// RF stati
#define RF_STATUS_SENDING			0b00000001
#define	RF_STATUS_SEND_SUCCESS		0b00000010
#define RF_STATUS_NEW_PACKET		0b00000100
#define RF_STATUS_RECEIVE_ERROR		0b00001000
#define RF_STATUS_LAYER_ON			0b10000000		// one if stack is running
#define RF_STATUS_INSERTED_CTRLMSG	0b01000000		// is being set if waiting normal msg was interrupted due t incoming control/management msg
#define RF_STATUS_NEW_DATA			0b00010000		// new data arrived
#define RF_STATUS_JUST_SENT			0b00100000		// data send in this slot


// LL
#define LL_HEADER_SIZE			12
#define LL_PAYLOAD_SIZE			64
#define LL_TAIL_SIZE			2
#define LL_FRAME_MAX_SIZE		(LL_HEADER_SIZE+LL_PAYLOAD_SIZE+LL_TAIL_SIZE)

// ACL
#define ACL_SUBSCRIPTIONLIST_LENGTH	8
#define ACL_CONTROL_MESSAGES_TIMEOUT	30
#define ACL_TYPE_ACM_H		ACL_TYPE_ACM_HI
#define ACL_TYPE_ACM_L		ACL_TYPE_ACM_LO

// for LED
#define LEDS_NORMAL			1
#define LEDS_OFF			0	//leds off and stack doesnt touch them
#define LEDS_ON_SEND		2
#define LEDS_ON_RECEIVE		3
#define LEDS_ONLY_ONE		5
#define LEDS_ON_CRC_ERROR	6	// blue led on crc error
#define LEDS_ON_TRAFFIC		7	// blue led on send/receive








void RFStop();
void RFStart();
int RF100usLeft();
int RF200usLeft();
int RF500usLeft();
void RFDelaySlots(int);

#separate
void SlotEndCallBack();


#ifdef USE_OTAP
//DECLARE EXTERNAL DEPENDENCIES:		(dirty hack)
void OtapInit();
void OtapSetID();
void OtapReceive();
void OtapAnswerRepeat();
void OtapReset();
#endif


//---------------------------------------------------------------------------------------------------------
// constants


// global variables
//
//
int rf_status;																// holds the state of rf layer like busy, receiving succes , err, etc


long rf_slotcounter;

int LL_slots_left;
int LL_payload_length;											// hold the actual size of LL frame (not includes ll header, lltail)
int LL_sequence_no;
boolean LL_receive_buffer_locked;
boolean LL_received_packet_in_this_slot;	//the reincarnation of the good old mac_msg_recieved_in_last_slot
long LL_longbuffer;

int LL_payload_received[LL_PAYLOAD_SIZE];
int LL_payload_send[LL_PAYLOAD_SIZE];
int LL_payload_receivebuffer[LL_PAYLOAD_SIZE];

int LL_header_received[LL_HEADER_SIZE];
int LL_header_send[LL_HEADER_SIZE];
int LL_header_receivebuffer[LL_HEADER_SIZE];

int LL_tail_received[LL_TAIL_SIZE];
int LL_tail_send[LL_TAIL_SIZE];
int LL_tail_receivebuffer[LL_TAIL_SIZE];

ll_data_in_last_slot	LL_last_data;

int ACL_subscriptions[ACL_SUBSCRIPTIONLIST_LENGTH][2];
boolean ACL_subscribe_all;

void newlifesign()
{
	int i;
	int old_state;

	old_state=rf_LED_style;
	AppSetLEDBehaviour(LEDS_OFF);

	for (i=0;i<3;i++)
	{
		PCLedRedOn();
		PCLedBlueOff();
		RFDelaySlots(8);
		PCLedRedOff();
		PCLedBlueOn();
		RFDelaySlots(8);
	}
	AppSetLEDBehaviour(old_state);

}




// This line is used by the MPU to control,
// when the USB chip is allowed to send data to the MPU
// If the MPU sets it low, the USB chip will start to transmit data
#define CTS_UART1			PIN_CONN_08
#define TRIS_CTS_UART1			TRIS_CONN_08

// this line is used to signal the MPU, that are available on the USB chip
// If the USB chip sets it low, the MPU will *later* set its CTS
// low in order to allow the transmission. This *later* is influenced by the
// macro USB_REQUEST_MAXDELAY.
#define RTS_UART1			PIN_CONN_09
#define TRIS_RTS_UART1			TRIS_CONN_09

//***********
//
// das ist die neue state machine
//
//**********
int USBSend() ;
int USBReceive()
{
  return -1;
}
#int_timer1
int fsm()
{  
  if(rf_status&RF_STATUS_LAYER_ON)
  {
    if (!input(RTS_UART1))	// RTS is low
    {
      USBReceive();
    } else {
      if (rf_status & RF_STATUS_SENDING)
      {
        if(USBSend())
        {
          rf_status|= (RF_STATUS_SEND_SUCCESS);
        }

        rf_status&= (~RF_STATUS_SENDING);
      }
    }
  }
  rf_slotcounter++;
  SlotEndCallBack();
}

int USBSend() {
	unsigned int i, n;
	byte c;

	Uart1SendByte(0);

	//HEADER
	for (i = 0; i < LL_HEADER_SIZE; i++) {
		c = LL_header_send[i];
		if ((c == 0) || (c == 192) || (c == 219)) {
			Uart1SendByte(219);
			c = c + 1;
		}
		Uart1SendByte(c);
	}

	//PAYLOAD
	n = LL_header_send[1]; 

	if (n > LL_PAYLOAD_SIZE)
		return 0;					// error; but what to do then?

	for (i = 0; i < n; i++) {
		c = LL_payload_send[i];
		if ((c == 0) || (c == 192) || (c == 219)) {
			Uart1SendByte(219);
			c = c + 1;
		}
		Uart1SendByte(c);
	}

	//TAIL
	for (i = 0; i < LL_TAIL_SIZE; i++) {
		c = LL_tail_send[i];
		if ((c == 0) || (c == 192) || (c == 219)) {
			Uart1SendByte(219);
			c = c + 1;
		}
		Uart1SendByte(c);
	}

	Uart1SendByte(192);			//end escaped packet
        return(false);
}




void RFInit()
{
	bit_clear(TRIS_CTS_UART1);
	bit_set(TRIS_RTS_UART1);

	Uart1Init(SERIAL_625000, SERIAL_ASYNCHRONOUS, 8);
}



#inline
void RFStart()
{
	PCLedRedOn();
	DelayMs(200);
	PCLedRedOff();
	DelayMs(200);


	rf_status|=RF_STATUS_LAYER_ON;

	RFDeleteTimer1Overflow();
	set_timer1(65000);										// start rf frame
	set_timer2(0);
	enable_interrupts(INT_TIMER1);						// enable rf interrupt
}




void RFSetFieldStrength(int power)
{

}

int RFGetFieldStrength()
{
  return 0;
}



void RFStop()
{
		disable_interrupts(INT_TIMER1);
		rf_status=rf_status&(~RF_STATUS_LAYER_ON);
		if (rf_status & RF_STATUS_SENDING)
		{
			rf_status&= (~RF_STATUS_SENDING);
			rf_status&= (~RF_STATUS_SEND_SUCCESS);
			rf_status&= (~RF_STATUS_INSERTED_CTRLMSG);
		}
		//PCLedRedOff();
		//PCLedBlueOff();

}



void RFDelaySlots(int slots)
{

	slots+=lo(rf_slotcounter);
        while(lo(rf_slotcounter)!=slots);	// block until time is reached (is this safe?);
}


//return the remaining time until next rf slot
long RFMicroSecondsUntilNextSlot()
{
	long value;
	value=(long)(255-PIC_TMR1H)*51;
	return value;
}
// returns true, when more than 100 us are left
int RF100usLeft()
{
	if (RF_TIMER1H<253) return 1;
	return 0;
}
// returns true, when more than 200 us are left
int RF200usLeft()
{
	if (RF_TIMER1H<251) return 1;
	return 0;
}

// returns true, when more than 500 us are left
int RF500usLeft()
{
	if (RF_TIMER1H<245) return 1;
	return 0;
}
