/*
 * USB Bridge
 *
 * Bridges data from RF to USB and vice versa using serial communication
 *
 * TR: !!!!!!!!!!!!!!!WATCH OUT!!!!!!!!!!!!! this version
 * *  tries to eliminate dups via multiple bridges
 * *  filters SNW packets
 * *  uses dutycycle for sending
 * *  has now higher priority for sending
 *
 * Author: Christian Decker (cdecker@teco.edu), Till Riedel (riedel@teco.edu)
 * Created: 2004-04-26
 * version: 002
 *
 */

#define this(F) F

#case

#opt 3

#define byte	BYTE
#define boolean BOOLEAN
#define true	TRUE
#define false	FALSE
#define global	GLOBAL

#define INC(X) #X
#define ACLAnyDataIsNew ACLDataIsNew

#include "system/acltypes.h"	// the acl types

#include "boards/18f6720.h"
#include "boards/18f6720_R.h"

#include INC(boards/BOARD.h)

#include "boards/wdt18fxx20.h"
#include "boards/18f6720.c"

// We use V9 to be more robust !?

#include "boards/wdt18fxx20.c"

#if BOARD_ID_LOW > 229
#include "boards/ballswitch.c"
#include "boards/ds2431.c"		//id chip
#include "boards/AT45DB041.c"	//flash
#include "boards/owmb.c"		//speaker
#else
#include "boards/pci2c.c"
#include "boards/pceeprom.c"	//flash
#endif

#include INC(boards/BOARD.c)

#include "system/awarecon.h"			
#include "system/awarecon.c"		

#ifndef BOARD_ID_LOW
#error NO BOARD DEFINED
#endif

#fuses hs,noprotect,nobrownout,nolvp,put,nowdt


// change the a/d port for the application if you want
//#define PIC_ADCON0_USER_VALUE 0b01010101
//#define PIC_ADCON1_USER_VALUE 0b10101010

#define BRIDGE_COMM_TIME_OUT		210

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


// delay in slots
#define USB_REQUEST_MAXDELAY		5

// function prototypes
void BridgeRun();
int LLSendPacketSpecial(int slot_limit);
void Particle2USB();
void USB2Particle();


// global variable
unsigned int usbRequestMaxDelay;

// this function is called from the fsm at the end of an rf slot
// make sure that it terminates in time
#separate
void SlotEndCallBack() {
	BridgeRun();
}

void BridgeRun() {
	if (LLSendingBusy()
		&& ACLDataIsNew()
		&& LL_tail_received[0] == LL_tail_send[0]
		&& LL_tail_received[1] == LL_tail_send[1]) {
		LLAbortSending();
		ACLSetDataToOld();
	}
	//Uart1Init(SERIAL_625000, SERIAL_ASYNCHRONOUS, 8);

	//DelayUs(30);//!!
	Uart1CheckError();
	Uart1FlushReceive();
	Uart1CheckError();



	if (!input(RTS_UART1) && !ACLSendingBusy())	// RTS is low
	{
		USB2Particle();
	} else {
		if (ACLDataIsNew()) {

                        Particle2USB();

			ACLSetDataToOld();
		}
	}
}

//
// forwards data in escaped form from Particle to USB
//
void Particle2USB() {
	unsigned int i, n;
	byte c;

	Uart1SendByte(0);

	//HEADER
	for (i = 0; i < LL_HEADER_SIZE; i++) {
		c = LL_header_received[i];
		if ((c == 0) || (c == 192) || (c == 219)) {
			Uart1SendByte(219);
			c = c + 1;
		}
		Uart1SendByte(c);
	}

	//PAYLOAD
	n = ACLGetReceivedPayloadLength();

	if (n > LL_PAYLOAD_SIZE)
		return;					// error; but what to do then?

	for (i = 0; i < n; i++) {
		c = LL_payload_received[i];
		if ((c == 0) || (c == 192) || (c == 219)) {
			Uart1SendByte(219);
			c = c + 1;
		}
		Uart1SendByte(c);
	}

	//TAIL
	for (i = 0; i < LL_TAIL_SIZE; i++) {
		c = LL_tail_received[i];
		if ((c == 0) || (c == 192) || (c == 219)) {
			Uart1SendByte(219);
			c = c + 1;
		}
		Uart1SendByte(c);
	}

	Uart1SendByte(192);			//end escaped packet
}

//
// Forwards data from USB to Particle
//
void USB2Particle() {
	unsigned long i;
	unsigned int8 j;
	byte c;

	// allow transmission
	output_low(CTS_UART1);

	i = 1300;
	while (--i) {
		if (bit_test(PIC_PIR1, 5))	// Uart1DataIsThere()
		{
			c = PIC_RCREG1;
			if (!c) {
				continue;
			} else {
				// LL_header (12 bytes)
				LL_header_send[0] = c;
				
				for (j=1; j<12; j++)
				{
					while (!bit_test(PIC_PIR1, 5)) {;}
					LL_header_send[j] = PIC_RCREG1;
				}

				// payload 64 byte
				for (j=0; j<64; j++)
				{
					while (!bit_test(PIC_PIR1, 5)) {;}
					LL_payload_send[j] = PIC_RCREG1;
				}

				// Tail (2 byte)
				for (j=0; j<2; j++)
				{
					while (!bit_test(PIC_PIR1, 5)) {;}
					LL_tail_send[j] = PIC_RCREG1;
				}

				if (!(ACLDataIsNew()
					  && LL_tail_received[0] == LL_tail_send[0]
					  && LL_tail_received[1] == LL_tail_send[1])
					)
					LLSendPacketSpecial(10);	// send, it just sets some states, but still is special (e.g. no CRC calc)
				break;
			}					// end else
		}						// end if (Uart1DataIsThere())
	}							// end while

	// end transmission
	output_high(CTS_UART1);

}								// end USB2Particle


// this is like normal packet send but without extra crc and so on
int LLSendPacketSpecial(int slot_limit) {
	//giveoutdata();

	if (slot_limit == 0)
		slot_limit = 1;			// zero makes no sense

	if (!(rf_status & RF_STATUS_LAYER_ON))
		return 0;

	if (rf_status & RF_STATUS_SENDING)
		return 0;

	// what about fieldstrength??
	// finalize the LLPacket
	// what about ID???

	LL_payload_length = LL_header_send[1];	// vise versa to normal

	LL_slots_left = slot_limit;
	rf_status |= RF_STATUS_SENDING;

	return 1;
}


void main() {
	/*
	   unsigned int stund;
	   unsigned int minut;
	   unsigned int sekund;
	   unsigned long millisekund;
	   unsigned int tag;
	   unsigned int monat;
	   unsigned int jahr;
	   unsigned int i; 
	 */


	byte test[10];

	usbRequestMaxDelay = USB_REQUEST_MAXDELAY;

	//TYPICAL STARTUP FLOW
	//*****************************************************************



	if (PCSelfTestBoardIsThere())
		PCSelfTest(test);		// check if board is there, doesn't harm any settings, those are restored
	PCInit();					// is not dangerous, because all pins are set correct . bport is input, i2c and eeprom are initianlized as well
	ACLInit();					// init the stack and start it
	// must be done before lifesign and

	//*****************************************************************
	//now start your code here

	bit_clear(TRIS_CTS_UART1);
	bit_set(TRIS_RTS_UART1);

	Uart1Init(SERIAL_625000, SERIAL_ASYNCHRONOUS, 8);
	AppSetLEDBehaviour(LEDS_NORMAL);

	ACLSubscribeAll();
	ACLSetControlMessagesBehaviour(false, true);

	enable_interrupts(global);

	ACLStart();

	for (;;) ;
}
