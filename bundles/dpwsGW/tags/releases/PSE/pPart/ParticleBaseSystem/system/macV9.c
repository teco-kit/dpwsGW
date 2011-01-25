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
//********************************************************
//	RF stack for TecO Smart-Its/Particle Computer
//  listen before talk
//
//	Albert Krohn				(krohn@teco.edu)
//  sabin wendhack				(wendhack@teco.edu)
//	Telecooperation Office
//  University of Karlsruhe
// 	Germany
//  www.teco.edu
//
#00 : awareconv9#000.c bases on awarecon#90.c

//********************************************************
*/



// these are for fixed variables (timing)
#reserve 0x20	// rx1
#reserve 0x21	// rx2
#reserve 0x22	// rx3
#reserve 0x23	// rx4
#reserve 0x24	// drx1
#reserve 0x25	// drx2
#reserve 0x26	// bit_count
#reserve 0x27	// shift
#reserve 0x28	// good
#reserve 0x29	// rf_last_err
#reserve 0x2a
#reserve 0x2b
#reserve 0x2c
#reserve 0x2d
#reserve 0x2e
#reserve 0x2f
#reserve 0x30	// rf_random
#reserve 0x31	// rf_random_sum
#reserve 0x32	// rftx



typedef unsigned byte  	UInt8;
typedef unsigned long  	UInt16;


// this is debug stuff
#define SENDER			0
#define DEBUG			0
#define SYNC_DEBUG		0		// set this to one: on pin 4 is the sync time visible to compare with other devices
//#define LL_PACKET_SENDER 0


#define VERSION			9
#define MY_MAC_ID		141					//141

// State Machine Timings
#define RF_FSM_TIME_SYNC		0			//one count is one bit or TIMER1H = 50,1 us
#define RF_FSM_TIME_SYNC_RECEIVE  88
#define RF_FSM_TIME_SLOT_END		255
#define RF_FSM_TIME_EARLY_SLOT_END (RF_FSM_TIME_SLOT_END-2)

#define LBT_FSM_TIME_SYNC_SEND  	0
#define LBT_FSM_TIME_SYNC_RECEIVE  	88

// State Machine
#define FSM_STATE_START 1
#define FSM_STATE_SYNC	2
#define FSM_STATE_STATISTIC  3
#define FSM_STATE_ARBI  4
#define FSM_STATE_SEND_DATA  5
#define FSM_STATE_REC_DATA  8
#define FSM_STATE_ALONE 6
#define FSM_STATE_LISTEN 7					// LBT
#define FSM_STATE_DEBUG 99
#define FSM_STATE_ERROR 10
#define FSM_STATE_END	20

// RF_TRAILING, SYMBOL
#define RF_INITIAL_LISTEN_SLOTS_DEFAULT 			2				// duration in slot that an unsynced new smartit uses to listen
#define RF_ARBITRATION_MINIMUM_SAMPLES 				7				// no of (uart)samples with which a slot is assumed "busy"

// RF_SYMBOLS            all end with "0" for turning off the tx pin on rf module
#define RF_SYMBOL_SYNC1 		0b11100011
#define RF_SYMBOL_SYNC2			0b10001110
#define RF_SYMBOL_SYNC3			0b00111000
#define RF_SYMBOL_SYNC4			0b11100011
#define RF_SYMBOL_SYNC5			0b10001111
#define RF_SYMBOL_SYNC6			0b11000000
#define RF_SYMBOL_SYNC7			0b11100000

#define RF_SYMBOL_SYNC			0b10110010

#define RF_SYMBOL_SYNCRECEIVED  0b00110010  			// the received Sync symbol (is the "sync"-symbol, but 3 times as fast
#define RF_SYMBOL_FASTTRAILER	0b10101010				// trailer for data packets
#define RF_SYMBOL_FASTTRAILER1	0b11100011				// trailer for data packets
#define RF_SYMBOL_FASTTRAILER2	0b10001110				// trailer for data packets
#define RF_SYMBOL_FASTTRAILER3	0b00111000				// trailer for data packets
#define RF_SYMBOL_SOP			0b11001010
#define RF_SYMBOL_EOP			0b00000000				// to reset the spi
#define	RF_SYMBOL_RSSI			0b11111111
#define	RF_SYMBOL_RSSI_START	0b11000111

// rssi macros, if you change them, you have to ajust timing in RSSIMeasure().
#define RF_RSSI_SAMPLES			36
#define RF_RSSI_BYTES			32

// RF stati
#define RF_STATUS_SENDING			0b00000001
#define	RF_STATUS_SEND_SUCCESS		0b00000010
#define RF_STATUS_NEW_PACKET		0b00000100
#define RF_STATUS_RECEIVE_ERROR		0b00001000
#define RF_STATUS_LAYER_ON			0b10000000		// one if stack is running
#define RF_STATUS_INSERTED_CTRLMSG	0b01000000		// is being set if waiting normal msg was interrupted due t incoming control/management msg
#define RF_STATUS_NEW_DATA			0b00010000		// new data arrived
#define RF_STATUS_JUST_SENT			0b00100000		// data send in this slot

// LBT
#define LBT_PREAMBLE_LENGTH			236
#define LBT_PREAMBLE_MIN_RECOGNIZED	        5
#define	LBT_NOISELEVEL				9
#define LBT_TOLERANCE				5	// tolerance of wrongly received bits of the preamble, at minimum 3 because sync produces two or three bad bits....
#define LBT_WAITING_TIME_CONSTANT	24000 // = 4,8 ms => arbitrary waiting time ranges between 0 and 8,3ms (13,1-4,8ms)
#define LBT_TIME_GOOK_LOOP			33	// measured miliseconds (to check timeout)
#define LBT_TIME_CHECK_LOOP			8	// measured miliseconds (to check timeout)
#define LBT_TIME_CHECK_IN_LOOP		24	// measured miliseconds (to check timeout)
#define LBT_LISTENING_TIME			500 // 500 tested

#define LBT_SYMBOL_SYNC1			0b10101111
#define LBT_SYMBOL_SYNC2			0b11000000
#define LBT_SYMBOL_SYNC3			0b11100000

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








// function declarations
//-------------------------------------------------------------------------------------
// pic layer... discontinued

/*
void PICInit();
int PICCombineTris(int sys,mask,user);
*/


// RF Layer
#inline
void RFSetModeSleep();
#inline
void RFSetModeAsk();
#inline
void RFSetModeOok();
#inline
void RFSetModeReceive();
#inline
void RFSetModeSensitive();
#inline
void RFSpiSendByte();
#inline
void RFDeleteTimer1Overflow();
void RFInit();
UInt8 RFGetRandom();
#inline
void RFPowerModuleOn();
#inline
void RFPowerModuleOff();
#separate
int DebugGiveOut(int i);


#inline
void RFInitUart();
#inline
void RFUartStartContinuous();
#inline
void RFUartClearFifo();
#inline
int RFUartGetByte();
#inline
void RFSpiWait();
#inline
void RFUartOff();
#inline
void RFUartOn();
#inline
void RFSpiOff();
#inline
void RFSpiOn();
#inline
void RFWaitForTimer1Exact(int timeout);
#inline
void RFWaitForTimer1ExactNew(int timeout);	// runtime of function ends exactly before TIMER1_H = timeout
#inline
void RFSpiClear();
#inline
void RFWaitForNextBitTimeExact();
int RFSendSync();
int RFSynchronize(int Timeout,int wait_slots);
void RFInitRandom();
#separate
int RFStatistic(boolean receive);
int RFArbitration(long rf_arbi_pos);
int RFSendData();
int RFReceiveData();
#inline
void RFScramble();
int RFCreateAliveSymbol();
void RFNewRandom();
void RFStop();
void RFSetFieldStrength(int power);
int RFGetFieldStrength();
#inline
void RFStart();
void RFReceiveScrambledByte();
void RFReceiveByte();
void RFSetSyncRate(int rate);
void RFSetSyncMode(int mode);
void RFSetInitialListenSlots(int slots);
#separate
int RFAlone();
#separate
void RFCheckSubscriptions();
void RFShowSynchronization();
long RFMicroSecondsUntilNextSlot();
#separate
void RFEvaluateStatistic();
int RF100usLeft();
int RF200usLeft();
int RF500usLeft();
void RFDelaySlots(int);


enum lbt_listen{LBT_NOISE, LBT_RECEIVE, LBT_FREE};
lbt_listen LBTListen( unsigned long timeout );	// listens  and returns RECEIVE, if preamble and syncsymbol detected, NOISE, if channel busy and FREE. timeout in ms.
int LBTSend(); //sends a packet (preamble, syncsymbol & packet)
#inline
long LBTGetWaitingTime(); // calculates arbitrary waiting time within a range determined by LBT_WAITING_TIME_CONSTANT



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
int rf_fsm_state;
int rf_status;																// holds the state of rf layer like busy, receiving succes , err, etc


long rf_slotcounter;
int rf_symbol_am_alive;											// always make sure that the lowest bit is zero
int rf_statistic_am_alive;										// hold the last valid situation
int rf_field_strength;
int rf_initial_listen_slots;									// the number of listenslots before a lonely note sends a sync
int rf_LED_style;
int rf_random_8;



int rf_statistic_values[9];									// statistics only on 18f platforms 0..7 holds bits, 8 the counter

boolean	lbt_time_set;
boolean lbt_state_sending;


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

// RSSI globals.
unsigned int 	rssi_send_on;
unsigned int	rssi_receive_on;





int ACL_subscriptions[ACL_SUBSCRIPTIONLIST_LENGTH][2];
boolean ACL_subscribe_all;

boolean ACL_ignore_control_messages;
boolean ACL_pass_control_messages;







#byte rf_rx1			=0x20
#byte rf_rx2			=0x21
#byte rf_buff			=0x22
#byte rf_thisbyte		=0x23
#byte rf_pos			=0x24

#byte lbt_noiselevel	=0x25

#byte rf_duty_cycle		=0x26
#byte rf_bit_count		=0x27
#byte rf_shift			=0x28
#byte rf_transition0	=0x29
#byte rf_transition1	=0x2a
#byte rf_transition2	=0x2b
#byte rf_transition3	=0x2c
#byte rf_limit			=0x2d
#byte rf_random_l 		=0x2e		//bits: 0: send request, bits1:arbi won,
#byte rf_random_h		=0x2f

#byte lbt_tolerance		=0x30

#byte rf_timeout    	=0x31	// current random number. used for generating "random" numbers in RFGetRandom()
#byte rf_tx				=0x32






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




//***********
//
// das ist die neue state machine
//
//**********
#int_timer1
int fsm()
{
int rf_agc_mode=0;
lbt_listen result;
//unsigned int i;


goto RfFsmStart;		// start of state machine

//**********************************************************************

RfFsmStart:

	rf_fsm_state=FSM_STATE_START;

//	output_high( PIN_CONN_18 );
//	output_low( PIN_CONN_18 );
//	output_high( PIN_CONN_18 );
//	output_low( PIN_CONN_18 );
//	output_high( PIN_CONN_18 );

	lbt_time_set = FALSE;

	rf_status&= ~RF_STATUS_JUST_SENT;				//delete the just sent flag
	LL_received_packet_in_this_slot=false;			//unset the reception flag
	LL_last_data=LL_NONE;

	rf_symbol_am_alive=RFCreateAliveSymbol();		// prepare symbol

	RFDeleteTimer1Overflow();

	SPIInit(SPI_MODE_TIMER2);
	RFSpiOn();

	RFInitUart();							// set correct baudrate etc
	RFUartOn();

	RFSetModeReceive();								// to wake up from sleep (this takes typ. 20us)


#ifdef __RSSI_H__
	if( rssi_receive_on ) RSSIPrepare( rssi_receive_on );
#endif
//	bit_set( TRIS_RSSI_RES );
//	bit_set( TRIS_RSSI_CAP );
//	bit_clear( TRIS_RSSI_VDD );
//	output_high( PIN_RSSI_VDD );

	goto RfFsmListen;


//**********************************************************************

RfFsmListen:
    delay_us(20);

    //XXXX PCLedRedOn();					//debug
	rf_fsm_state=FSM_STATE_LISTEN;


	RFSetModeReceive();

	result = LBTListen( LBT_LISTENING_TIME );

	if( result == LBT_RECEIVE )
	{
		lbt_state_sending = FALSE;
		goto RfFsmReceiveData;
	}
	else if(( rf_status & RF_STATUS_SENDING ) && ( result == LBT_FREE ))  //something to send and channel free
	{
		lbt_state_sending = ~lbt_state_sending;

		if( !lbt_state_sending ) goto RfFsmSend; 	// send
		else
		{
#ifdef RELATE_BRICK
                    if(relate_no_desync==1)
				set_timer1(65000);	// listen again after an arbitrary waiting time
                    else
#endif
				set_timer1( LBTGetWaitingTime() );	// listen again after an arbitrary waiting time
				goto RfFsmEnd;
		}
	}
	else // result == LBT_NOISE
	{
		if(rf_agc_mode==0)
		{
			rf_agc_mode++;

				bit_clear( TRIS_RSSI_RES );
				bit_set( TRIS_RSSI_CAP );
				bit_set( TRIS_RSSI_VDD );
				output_low( PIN_RSSI_RES );

			goto RfFsmListen;
		}
		else
		{

			lbt_state_sending = FALSE;
			goto RfFsmEnd;
		}
	}

	goto RfFsmEnd;


//**********************************************************************

RfFsmSend:
        //XXXX PCLedRedOff();					//debug
	rf_fsm_state=FSM_STATE_SEND_DATA;

	RFSetModeAsk();

	LBTSend();

	LLSetSendingSuccess();

	goto RfFsmEnd;

//**********************************************************************

RfFsmReceiveData:

	rf_fsm_state=FSM_STATE_REC_DATA;

	lbt_state_sending = FALSE;

	//XXXX PCLedBlueOn();					//debug
	if (RFReceiveData() != 0)
	{
//		PCLedBlueOn();					//debug
//		output_high( PIN_CONN_18 );
	}

	goto RfFsmEnd;

//**********************************************************************

RfFsmEnd:
//XXXX PCLedRedOff();					//debug
//XXXX        PCLedBlueOff();					//debug
	rf_fsm_state=FSM_STATE_END;

	RFSetModeReceive();
	rf_tx=0;RFSpiSendByte();					// for a secure zero state
	RFUartOff();									// save energy
	RFSpiOff();										// save energy

	RFSetModeSleep();							// switch off the rf module

	rf_slotcounter++;

	AppSetLEDs();						// sets the leds

	RFDeleteTimer1Overflow();

	if (rf_status & RF_STATUS_LAYER_ON)
	{
		enable_interrupts(INT_TIMER1);		// make sure to keep timer 1 running
	}
	else
	{
		RFStop();							// if layer was switched off; make sure the rf stack stops here
	}

	LLSlotEnd();

	RFNewRandom();
#ifdef USE_OTAP
	OtapAnswerRepeat();						//repeat answer packets if no answer from return
#endif

	if(!selftest_active) SlotEndCallBack();	// call CallBack, but not if still in selftest

/*	if( lbt_time_set )						//debug
	{
		RFWaitForTimer1ExactNew( 200 );
		output_high( PIN_CONN_17 );
		output_low( PIN_CONN_17 );
	}
*/
//	PCLedBlueOff();							//debug
//	output_low( PIN_CONN_18 );

	return 0;										//back to application main - level

//**********************************************************************

RfFsmError:

		rf_fsm_state=FSM_STATE_ERROR;

		PCLedRedOn();
		PCLedRedOff();

	goto RfFsmEnd;

//**********************************************************************

}






// RFMODES of tr1001

#inline
void RFSetModeSleep()
{
#if 1
	output_low(PIN_RF_CTR0);
	output_low(PIN_RF_CTR1);
#endif
}

#inline
void RFSetModeAsk()
{
	output_low(PIN_RF_CTR0);
	output_high(PIN_RF_CTR1);
}

#inline
void RFSetModeOok()
{
	output_high(PIN_RF_CTR0);
	output_low(PIN_RF_CTR1);
}

#inline
void RFSetModeReceive()
{
	output_high(PIN_RF_CTR0);
	output_high(PIN_RF_CTR1);
	output_high(PIN_RF_SENSITIVE);
}

#inline
void RFSetModeSensitive()
{
	output_high(PIN_RF_CTR0);
	output_high(PIN_RF_CTR1);
	output_low(PIN_RF_SENSITIVE);
}




#inline
void RFInitUart()
{
	#asm
		movlw 	0x04
		movwf 	RF_SPBRG		// set correct baudrate   // ehemals PI_SPBRG
		bcf		RF_TXSTA,2		// set low speed
		bsf		RF_TXSTA,4		// set synchronous
		bsf		RF_TXSTA,7		// set synchronus mode MASTER
		bcf		RF_TXSTA,6		// 8 bit transmit

		bsf		RF_UART_SPEN		// enable serial port (power on)
		bcf		RF_UART_SREN		// disable single receive
		bcf		RF_RCSTA,6		// 8 bit reception
		bcf     RF_UART_CREN		// disable continuous receive

	#endasm

	bit_clear(RF_TXSTA,5);		//transmit on pin_c6 disable

}


// one bit of buff is one sample of spi (no oversampling!!)
// reuse of rf_rx1 as trash buffer
#inline
void RFSpiSendByte()
{
	#asm
			MOVF	RF_SSPBUF, W	// read SSBUF for clear -> Akku (every byte is also received)
			MOVWF	rf_rx1			// load to nirvana (must do this, otherwise compiler kills command)
			MOVF	rf_tx, W			// load rf_shift Register
			MOVWF	RF_SSPBUF		// Akku -> SSBUF
	#endasm
}


#inline
void RFSpiWait()
{
	//
	#asm
		LOOP3:
			BTFSS 	RF_SSPSTAT_BF		// sspstat<bf> wait for free
			GOTO 	LOOP3
	#endasm
	//
}


#inline
void RFSpiClear()
{

	int nirvana;
	#asm
			MOVF	RF_SSPBUF, W	// read SSBUF for clear -> Akku (every byte is also received)
			MOVWF	nirvana			// load to nirvana (must do this, otherwise compiler kills command)
	#endasm
}



//sends out sync at time sync
/*int RFSendSync()
{
	int i;
	// first wait for correct time
	if (RF_TIMER1H>= RF_FSM_TIME_SYNC) return 0;
//	RFWaitForTimer1Exact(RF_FSM_TIME_SYNC);
	RFWaitForNextBitTimeExact();


    rf_tx=RF_SYMBOL_SYNC1;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC2;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC3;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC4;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC5;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC6;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC7;RFSpiSendByte();RFSpiWait();


	return 1;
}
*/



#inline
void RFDeleteTimer1Overflow()
{
	bit_clear(RF_TIMER1_IF);		// clear TMR1IF i
}




void RFSetInitialListenSlots(int slots)
{
	rf_initial_listen_slots=slots;
}



void RFInit()
{


	//set my own pins to the necessary values


	bit_clear(TRIS_RF_CTR0);		//set to output
	bit_clear(TRIS_RF_CTR1);
	bit_clear(TRIS_RF_TX);
	bit_set(TRIS_RF_RX);
	bit_clear(TRIS_RF_POWER);
	bit_clear(TRIS_RF_SENSITIVE);


	RFSetModeSleep();
	RFPowerModuleOn();											// turn rf module to sleep


	//PICInit();
	// setup timers
	setup_timer_1( T1_INTERNAL | T1_DIV_BY_1);
	bit_clear(RF_TIMER1_CON,7);								// no 16bit read
	setup_timer_2( T2_DIV_BY_1,19,2);					// 8.0 µS
	bit_clear(RF_TIMER1_CON,3);								// turn off external oszi



	//RFSetModeSleep();											// set rf module asleep
	//	RFPowerModuleOff();											// turn off rf module

	// setup serial ports
	RFInitUart();
	RFUartOff();
	//setup_spi( SPI_MASTER | SPI_L_TO_H | SPI_CLK_T2);
	SPIInit(SPI_MODE_TIMER2);
	rf_tx=0;
	RFSpiSendByte();											//  to have bit BF set
	RFSpiWait();rf_tx=0;RFSpiSendByte();						//****new to have output definitive low
	RFSpiOff();

	RFInitRandom();

	rf_status=RF_STATUS_LAYER_ON|0;								// init the status
	//rf_sync_mode=RF_SYNC_MODE_ACTIVE;							// normal sync mode with looking for and syncing
	//rf_sync_rate=RF_SYNC_RATE_DEFAULT;							// normal 50% sync rate
	rf_initial_listen_slots=RF_INITIAL_LISTEN_SLOTS_DEFAULT;	// see default def

	rf_field_strength=32;										// init the fieldstrength to max
	rf_slotcounter=0;
	rf_statistic_am_alive=0;

	for (rf_pos=0;rf_pos<=8;rf_pos++) rf_statistic_values[rf_pos]=0;	// clear statistics
#ifdef USE_SDJS
	sdjs_type = 0;   // switch off sdjs.
#endif
}



#inline
void RFStart()
{

	RFSetModeSleep();									//****new

	RFPowerModuleOn();									// turn on rf module
	DelayMs(90);										//*** for waiting until tr1001 capacity is charged


	RFSetFieldStrength(rf_field_strength);

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
	rf_field_strength=power;								// update layer variable

		// if poti is the MAX5161
		{
			output_high(PIN_POTI_UD);							// set direction up
			for(rf_shift=32;rf_shift>0;rf_shift--)				// count till zero
			{
				output_high(PIN_POTI_INC);						// cycle
				output_low(PIN_POTI_INC);
			}
			// now poti has max resistance
			output_low(PIN_POTI_UD);							// set direction downwards
			for(;power>0;power--)
			{
				output_high(PIN_POTI_INC);						// cycle
				output_low(PIN_POTI_INC);
			}
		}

}

int RFGetFieldStrength()
{
  return rf_field_strength;
}

// listens  and returns RECEIVE, if preamble and syncsymbol detected, NOISE, if channel busy and FREE. timeout in ms.
lbt_listen LBTListen( unsigned long timeout )
{

	// use of globals:
	// rf_transition0 	minimum number of preamble-bits recognized
	// rf_transition1	timeout low byte in ms
	// rf_transition2	timeout high byte in ms
	// rf_shift			preamble is shifted in to detect sync symbol
	// rf_thisbyte		byte read from uart (either 8samples respectively one bit, or one byte read by RFUartGetByte()
	// rf_buff			recognized bit
	// rf_pos			recognized bit
	// rf_timeout		timeout occurred if set
	// lbt_tolerance	counting bad bits of preamble
	// lbt_noiselevel	incrementing with found ones



	unsigned int noiselevel;

//	output_high( PIN_CONN_18 );		//debug

	rf_transition1 = lo( timeout );
	rf_transition2 = hi( timeout );

	lbt_tolerance = LBT_TOLERANCE;


	noiselevel = 0;


	RFUartStartContinuous();
	RFUartClearFifo();

	lbt_noiselevel = 0;
	rf_timeout = 0;

	goto gook_new;

	gook_from_check:

		lbt_noiselevel += (( 3 * ( LBT_PREAMBLE_MIN_RECOGNIZED - rf_transition0 )) >> 1 ); // add average detected bytes from last check_loop to noiselevel
		if( lbt_noiselevel > LBT_NOISELEVEL ) goto noise;

		if( rf_timeout ) goto time_out;

	gook_new:
		rf_shift = 0;
		rf_transition0 = LBT_PREAMBLE_MIN_RECOGNIZED;

	gook:
			#asm
			nop
	//		bsf 	PIC_PORTD, 5
		BIT_WAIT1:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT1
			nop
//			bcf 	PIC_PORTD, 5

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

		adjust:								// switch off uart, wait for 9 mue
			bcf		RF_RCSTA, 4
			movlw	11
			movwf	rf_transition3
		loop:
			decfsz	rf_transition3
			goto loop
			bsf		RF_RCSTA, 4

		BIT_WAIT2:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT2
			nop
			//			bsf 	PIC_PORTD, 5

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

	//		btfsc	rf_thisbyte, 0			// test three bits of last byte for noiselevel
	//		incf	lbt_noiselevel
	//		btfsc	rf_thisbyte, 1
	/*		incf	lbt_noiselevel
			btfsc	rf_thisbyte, 2
			incf	lbt_noiselevel
	*/		btfsc	rf_thisbyte, 3
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 4
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 5
			incf	lbt_noiselevel
	/*		btfsc	rf_thisbyte, 6
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 7
			incf	lbt_noiselevel
	*/
			movlw	LBT_TIME_GOOK_LOOP						// decrement timeout 33 mue for one gook-loop
			subwf	rf_transition1
			btfss	PIC_STATUS, 0
			decf	rf_transition2
			btfss	PIC_STATUS, 0
			goto	time_out


			clrf	rf_pos					// check middle SIX bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b10000001
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_pos, 0
			movlw	0b01111110
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_pos, 6
			btfsc	rf_pos, 0
			bcf		rf_pos, 6

		BIT_WAIT3:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT3
			nop
//			bcf 	PIC_PORTD, 5

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

	//		btfsc	rf_thisbyte, 0			// test three bits of last byte for noiselevel
	//		incf	lbt_noiselevel
	//		btfsc	rf_thisbyte, 1
	/*		incf	lbt_noiselevel
			btfsc	rf_thisbyte, 2
			incf	lbt_noiselevel
	*/		btfsc	rf_thisbyte, 3
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 4
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 5
			incf	lbt_noiselevel
	/*		btfsc	rf_thisbyte, 6
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 7
			incf	lbt_noiselevel
	*/
			clrf	rf_buff					// check middle SIX bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b10000001
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_buff, 0
			movlw	0b01111110
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_buff, 6
			btfsc	rf_buff, 0
			bcf		rf_buff, 6

			movf	rf_buff, w				// check if last two bytes make 'one' together
			addwf	rf_pos, 1
			decfsz	rf_pos, w
			goto 	gook

			movff  rf_buff, rf_pos			// save last byte in rf_pos

	check:
			nop
//			bsf 	PIC_PORTD, 5
		BIT_WAIT4:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT4
//			bcf PIC_PORTD, 5
			nop

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

			clrf	rf_buff					// check middle SIX bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b10000001
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_buff, 0
			movlw	0b01111110
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_buff, 6
			btfsc	rf_buff, 0
			bcf		rf_buff, 6

			movlw	LBT_TIME_CHECK_LOOP		// decrement timeout 8 mue for one check-loop
			subwf	rf_transition1
			btfss	PIC_STATUS, 0
			decf	rf_transition2
			btfss	PIC_STATUS, 0
			bsf		rf_timeout, 0

			movf	rf_buff, w				// check if last two bytes make 'one' together
			addwf	rf_pos
			decfsz	rf_pos
			goto 	gook_from_check

			movff  rf_buff, rf_pos			// save last byte in rf_pos

			decfsz	rf_transition0			// check if RF_PREAMBLE_MIN_RECOGNIZED times zero or one
			goto	check

	check_in:
//			bsf 	PIC_PORTD, 5
			nop
		BIT_WAIT5:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT5
			nop
//			bcf 	PIC_PORTD, 5

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

			clrf	rf_buff					// check middle FOUR bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b11000011
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_buff, 0
			movlw	0b00111100
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_buff, 6
			btfsc	rf_buff, 0
			bcf		rf_buff, 6
			#endasm

		rf_shift = rf_shift << 1;			// queue bytes in rf_shift to match with syncsymbol

		if( rf_buff == 1 ) bit_set( rf_shift, 0 );
		else if( rf_buff == 0 ) bit_clear( rf_shift, 0 );
		else
		{
			--lbt_tolerance;				// if not, decrement RF_LBT_TOLERANCE
			if( !lbt_tolerance ) goto noise;
		}

		rf_shift = rf_shift & 0b00111111;

		rf_thisbyte = RFUartGetByte();

		#asm
		movlw	LBT_TIME_CHECK_IN_LOOP							// decrement timeout 24 mue for one check_in-loop
		subwf	rf_transition1
		btfss	PIC_STATUS, 0
		decf	rf_transition2
		btfss	PIC_STATUS, 0
		bsf		rf_timeout, 0
		#endasm

		rf_pos += rf_buff;					// check if last two bytes make 'one' together

		if( rf_pos != 1 )
		{
			--lbt_tolerance;				// if not, decrement RF_LBT_TOLERANCE
			if( !lbt_tolerance ) goto noise;
		}

		rf_pos = rf_buff;

		rf_thisbyte = RFUartGetByte();

		if( rf_shift != RF_SYMBOL_SYNCRECEIVED ) goto check_in;		// check for syncsymbol

                do
                {
                    rf_thisbyte = RFUartGetByte();
                }
		while((rf_thisbyte | 0b11000011 ) != 255 );	// wait for first 'one' after syncsymbol, to determine the point at which timer1 is set.

		set_timer1( LBT_FSM_TIME_SYNC_RECEIVE );
		lbt_time_set = TRUE;

		return LBT_RECEIVE;

	noise:
		++noiselevel;

	time_out:

/*		output_low( PIN_CONN_18 );		//debug
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
*/
		if( noiselevel || ( lbt_noiselevel > LBT_NOISELEVEL ))
		{
/*			output_high( PIN_CONN_18 );		//debug
			output_low( PIN_CONN_18 );
			output_high( PIN_CONN_18 );
			output_low( PIN_CONN_18 );
			output_high( PIN_CONN_18 );
			output_low( PIN_CONN_18 );
*/			return LBT_NOISE;
		}
		else return LBT_FREE;
}




#inline
void RFUartStartContinuous()
{
	#asm
		bcf		RF_UART_SREN		// clear SREN = disable single receive
		bcf		RF_UART_CREN		// clear CREN = disable continuous receive
		bsf		RF_UART_CREN		// set	CREN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	#endasm

}



#inline
void RFUartClearFifo()
{
	#asm
		btfsc	RF_UART_RCIF			// skip next (readout) if RCIF is clear (=no data there)
		movf	RF_RCREG,W
		btfsc	RF_UART_RCIF			// two times
		movf	RF_RCREG,W
	#endasm
}

#inline
int RFUartGetByte()
{
	#asm
		BIT_WAIT:
		btfss	RF_UART_RCIF			// test rcif
		goto 	BIT_WAIT
	#endasm
	return RF_RCREG;


}





// turn uart immideately off (a byte being received is killed!)
#inline
void RFUartOff()
{
	#asm
		bcf	RF_UART_SPEN				//clear SPEN
	#endasm
}
#inline
void RFUartOn()
{
	#asm
		bsf	RF_UART_SPEN				//set SPEN
	#endasm
}


// turns spi immideately off!! (a byte being transmitted is killed!), the pin must then be set
#inline
void RFSpiOff()
{
	output_low(PIN_RF_TX);
	#asm
		bcf	RF_SSPCON_SSPEN			// clear SSPEN
	#endasm

}

#inline
void RFSpiOn()
{

	#asm

			bsf	RF_SSPCON_SSPEN				// set SSPEN
			//MOVLW	0						// load 0, init the rf_shift reg with zero (otherwise tx line is high for long time)
			//MOVWF	RF_SSPBUF				// Akku -> SSBUF

	#endasm


}


#inline
void RFWaitForTimer1Exact(int timeout)
{
	//use global rf_buff as buffer, rf_timeout
		rf_timeout=timeout;
		RFDeleteTimer1Overflow();
		if (rf_timeout>RF_FSM_TIME_EARLY_SLOT_END) rf_timeout=RF_FSM_TIME_EARLY_SLOT_END;	// for stable reason (otherwise the break condition may not occur)
		while (RF_TIMER1H<rf_timeout);
		while (!bit_test(RF_TIMER1L,3)) ;

			rf_buff=RF_TIMER1L;			// read Timer 2
		#asm

			BTFSS	rf_buff,0			// if buff,0 ==0 then wait 2
			goto    weiter1
			goto    weiter2
		weiter1:					// buff,0 was 0 0> wait
			nop
			nop						// one extra wait
		weiter2:
			BTFSS	rf_buff,1
			goto weiter3
			goto weiter4
		weiter3:
			nop
			nop						//two extra wait
			nop
		weiter4:
			BTFSS  rf_buff,2
			goto weiter5
			goto weiter6
		weiter5:
			nop
			nop						// four extra wait
			nop
			nop
			nop
		weiter6:
			nop

		#endasm
		set_timer2(0);
}



// runtime of function ends exactly before TIMER1_H = timeout
#inline
void RFWaitForTimer1ExactNew(int timeout)
{
	//use global rf_buff as buffer, rf_timeout
		rf_timeout = timeout-1;
		RFDeleteTimer1Overflow();
		if( rf_timeout > RF_FSM_TIME_EARLY_SLOT_END ) rf_timeout = RF_FSM_TIME_EARLY_SLOT_END;	// for stable reason (otherwise the break condition may not occur)
		if( RF_TIMER1H >= rf_timeout ) return;
		while( RF_TIMER1H < rf_timeout );

		#asm
	seven:
		btfss	RF_TIMER1L, 7
		goto	seven
	six:
		btfss	RF_TIMER1L, 6
		goto	six
	five:
		btfss	RF_TIMER1L, 5
		goto	five
	four:
		btfss	RF_TIMER1L, 4	// 1st possibility: 11110000, 2nd possibility: 11110001, 3rd possibility: 11110010
		goto	four			// + 2 cycles

		movf	RF_TIMER1L, w	// 1st poss: 11110011, 2nd poss: 11110100, 3rd poss: 11110101
								// 1st poss: 12 cyc to go, 2nd poss: 11 cyc to go, 3rd poss: 10 cyc to go
		movwf	rf_buff			// + 1 cycle

		btfss	rf_buff, 2		// + 1 cycle for all
		goto 	first			// + 2 cycles for all
								// = 4 cycles gone for all possibilities
		btfss	rf_buff, 0		// + 1 cycle for 2nd and 3rd poss
		goto 	second			// + 2 cycles for 2nd and 3rd poss
								// = 3 cycles gone for 2nd and 3rd poss
		goto	third			// + 2 cycles for 3rd poss
								// = 2 cycles gone for 3rd poss
								// ---------------------------------------
								// = 4 cycles gone for 1st poss -> 12 - 4 = 8 to go
								// = 7 cycles gone for 2nd poss -> 11 - 7 = 4 to go
								// = 9 cycles gone for 3rd poss -> 10 - 9 = 1 to go
	first:
		nop
		nop
		nop
		nop


	second:
		nop
		nop
		nop

	third:
		nop
/*		tstfsz	RF_TIMER1L
		bsf		PIC_PORTD, 4
		bcf		PIC_PORTD, 4
		movf	RF_TIMER1H, w
		subwf	200, w
		btfss	PIC_STATUS, 2
		bsf		PIC_PORTD, 4
		bcf		PIC_PORTD, 4
*/		#endasm
}


#inline
void RFWaitForNextBitTimeExact()
{

	// use global rf_buff



		#asm
			bcf		RF_TIMER2_TMR2IF
		SYNC_L1:
			btfss	RF_TIMER2_TMR2IF		// test tmr21f overflow
			goto 	SYNC_L1

		#endasm
			rf_buff=RF_TIMER2;			// read Timer 2
		#asm

			BTFSS	rf_buff,0			// if rf_buff,0 ==0 then wait 2
			goto    weiter1
			goto    weiter2
		weiter1:					// buff,0 was 0 0> wait
			nop
			nop						// one extra wait
		weiter2:
			BTFSS	rf_buff,1
			goto weiter3
			goto weiter4
		weiter3:
			nop
			nop						//two extra wait
			nop
		weiter4:
			BTFSS  rf_buff,2
			goto weiter5
			goto weiter6
		weiter5:
			nop
			nop						// four extra wait
			nop
			nop
			nop
		weiter6:
			nop

		#endasm


}




void RFInitRandom()
{
	UInt8 i;
	rf_random_h = 0;
	for (i=0;i<8;i++)
	{
		rf_random_h = rf_random_h ^ ReadIntEeprom(i);
	}
	if (rf_random_h==0) rf_random_h = MY_MAC_ID;

	rf_random_8=rf_random_h;						// init the 8bit random function as well

}



//sends a packet (preamble, syncsymbol & packet)
int LBTSend()
{
	unsigned int i;



		i = LBT_PREAMBLE_LENGTH - 1;
		rf_tx = RF_SYMBOL_FASTTRAILER;
		RFSpiSendByte();
		while (--i)
		{
			RFSpiWait();
			RFSpiSendByte();
		}

		rf_tx=LBT_SYMBOL_SYNC1;RFSpiWait();RFSpiSendByte();
		rf_tx=LBT_SYMBOL_SYNC2;RFSpiWait();RFSpiSendByte();
		rf_tx=LBT_SYMBOL_SYNC3;RFSpiWait();RFSpiSendByte();

		rf_tx=RF_SYMBOL_FASTTRAILER;
		RFSpiWait();
		RFSpiSendByte();	// now you have 64us time to start the data transmission

		set_timer1( LBT_FSM_TIME_SYNC_SEND );
		lbt_time_set = TRUE;

		rf_tx=RF_SYMBOL_FASTTRAILER;
		RFSpiWait();
		RFSpiSendByte();	// now you have 64us time to start the data transmission

#ifdef USE_SDJS
		// SDJS: if sdjs start type found in sendbuffer and sdjs is switched on sdjs_start is set.
		if (( LL_payload_send[0]==ACL_TYPE_SJS_HI) && (LL_payload_send[1]==ACL_TYPE_SJS_LO) && sdjs_type ) sdjs_start = 1;
		else sdjs_start = 0;
#endif

		// send RF Header
		rf_tx=RF_SYMBOL_SOP;RFSpiWait();RFSpiSendByte();					// Send SOP		(PHY Header)
		if (bit_test(rf_random_l,2)) rf_tx=23; else rf_tx=116;				// if swcrambler was 0 ´change it to none zero
		RFSpiWait();RFSpiSendByte();										// Send Scrambler init (PHY Header)
		rf_transition0=rf_tx;												// init RFScrambler with  rf_transition0 is the RFScrambler rf_shift reg
		rf_tx=LL_payload_length+LL_HEADER_SIZE+LL_TAIL_SIZE;RFSpiWait();RFSpiSendByte();					// Send LL payload length



		// this is   for acl subscriptions
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^LL_payload_send[0];RFSpiWait();RFSpiSendByte();	//artefact high
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^LL_payload_send[1];RFSpiWait();RFSpiSendByte();	//artefact low

		// give 4 bytes time for subscription chekc in receiver
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time

#ifdef USE_SDJS
		// SDJS: if sdjs_start is set sdjs is started here.
		if ( sdjs_start )
		{
			SDJSRF();
			rf_status|=RF_STATUS_JUST_SENT;

			return 1;
		}
#endif


		// send LL_header
		for (rf_shift=0;rf_shift<=11;rf_shift++)
		{
			for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler

			rf_tx=rf_transition0^LL_header_send[rf_shift];					// scramble header data with scrambler
			RFSpiWait();
			RFSpiSendByte();
		}

		//send ACL_Data;
		rf_limit=LL_payload_length;
		for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
		{
			for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler

			rf_tx=rf_transition0^LL_payload_send[rf_shift];					// scramble header data with scrambler
			RFSpiWait();
			RFSpiSendByte();
		}


		//send LL_tail1
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler
		rf_tx=rf_transition0^LL_tail_send[0];
		RFSpiWait();
		RFSpiSendByte();

		//send LL_tail2
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler
		rf_tx=rf_transition0^LL_tail_send[1];
		RFSpiWait();
		RFSpiSendByte();

		// rssi
		if( rssi_send_on )
		{
			rf_tx=RF_SYMBOL_RSSI_START;
			RFSpiWait();RFSpiSendByte();

			// rssi: send zeros
			for( rf_pos = 0; rf_pos < RF_RSSI_BYTES; ++rf_pos )
			{
				rf_tx=0;
				RFSpiWait();RFSpiSendByte();
			}

			// rssi: send ones
			for( rf_pos = 0; rf_pos < RF_RSSI_BYTES; ++rf_pos )
			{
				rf_tx=RF_SYMBOL_RSSI;
				RFSpiWait();RFSpiSendByte();
			}
		}

		// send eop
		rf_tx=RF_SYMBOL_EOP;
		RFSpiWait();RFSpiSendByte();

		// that's it
		RFSpiWait();


		rf_status|=RF_STATUS_JUST_SENT;
		// this is debug
		/*
		for (rf_shift=0;rf_shift<LL_HEADER_SIZE;rf_shift++)
			DebugGiveOut(LL_header_send[rf_shift]);
		for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
			DebugGiveOut(LL_payload_send[rf_shift]);
		for (rf_shift=0;rf_shift<LL_TAIL_SIZE;rf_shift++)
			DebugGiveOut(LL_tail_send[rf_shift]);

		*/


		return 1;
}



// returns result in rf_pos
#separate
void RFCheckSubscriptions()
{
	//search through subscriptions for early shutdown
	for (rf_shift=0;rf_shift<ACL_SUBSCRIPTIONLIST_LENGTH;rf_shift++)
	{
		RFUartGetByte();	//trash one bit
		RFScramble();

		if (rf_transition2==ACL_subscriptions[rf_shift][0])
		{
			RFUartGetByte();	//trash 2nd bit
			RFScramble();

			if (rf_transition3==ACL_subscriptions[rf_shift][1]) rf_pos=1;
		}
		else
		{
			RFUartGetByte();	//trash 2nd bit
			RFScramble();
		}

	}
	for (rf_shift=0;rf_shift<(32-(ACL_SUBSCRIPTIONLIST_LENGTH*2));rf_shift++)
	{
		RFUartGetByte();
		RFScramble();	//trash the rest of the time

#ifdef USE_SDJS
		// SDJS: if sdjs start type is found in receivebuffer and sdjs is switched on sdjs_start is set.
		if (( rf_transition2==ACL_TYPE_SJS_HI) && (rf_transition3==ACL_TYPE_SJS_LO) && sdjs_type )
		{
				sdjs_start = TRUE;
				rf_pos=1;
		}
#endif
	}
	// till here check subscription
}




int RFReceiveData()
{
	int ll_receive_len;
	long crc16;


	RFUartClearFifo();
	rf_buff=RFUartGetByte();				//trash one bit

	rf_rx1=0;
	rf_shift=40; 		// 40 bit timeout
	// receive SOP Symbol in rf_rx1
		while ((rf_shift) && (rf_rx1!=RF_SYMBOL_SOP))
	{
		rf_rx1<<=1;
		rf_buff=RFUartGetByte();
		if ((rf_buff|0b11100111)==255) bit_set(rf_rx1,0);else bit_clear(rf_rx1,0);
		rf_shift--;

	}

	if (rf_rx1!=RF_SYMBOL_SOP) return 0;

	// receive Scrambler init byte
	RFReceiveByte();
	rf_transition0=rf_rx1;	// init RFScrambler, rf_transition0 is rf_shiftreg of RFScrambler


	//receive the len of the ll packet
	RFReceiveByte();
	ll_receive_len=rf_rx1;


	//check the subscrisptions, trash 16 bit during process
	RFReceiveScrambledByte();		// get ARtefact high
	rf_transition2=rf_rx1;
	RFReceiveScrambledByte();		// get artefact low
	rf_transition3=rf_rx1;
	rf_pos=0;						// reuse for result of subscriptions

#ifdef USE_SDJS
	// SDJS: clear sdjs_start as it is set if SDJS start type is found in RFCheckSubscriptions().
	sdjs_start = 0;
#endif
	RFCheckSubscriptions();  		// result is in rf_pos


	if (ACL_subscribe_all) rf_pos=1;		// if all subscribe then set the "found" state





	if (!rf_pos)
	{
		// no subscription matched the types
		//DebugGiveOut(0b11000011);
		return 0;		//early shut off
	}




	// receive the LL header
	for (rf_shift=0;rf_shift<LL_HEADER_SIZE;rf_shift++)
	{
		RFReceiveScrambledByte();										// gets one byte and stores it in rx1
		LL_header_receivebuffer[rf_shift]=rf_rx1;					// copy received byte into receive buffer
	}






	// receive the LL payload
	rf_limit=ll_receive_len-LL_HEADER_SIZE-LL_TAIL_SIZE;
	if (rf_limit>LL_PAYLOAD_SIZE) return 0;					// length is error
#ifdef USE_SDJS
	// SDJS: if sdjs_start is set sdjs is started.
	if ( sdjs_start == 1 )
	{
		SDJSRF();
		rf_limit=64;
		return 0;
	}
#endif

	for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
	{
		RFReceiveScrambledByte();		// gets one byte and stores it in rx1
		LL_payload_receivebuffer[rf_shift]=rf_rx1;					// copy received byte into receive buffer
	}


	// receive the LL tail
	for (rf_shift=0;rf_shift<LL_TAIL_SIZE;rf_shift++)
	{
		RFReceiveScrambledByte();		// gets one byte and stores it in rx1
		LL_tail_receivebuffer[rf_shift]=rf_rx1;					// copy received byte into receive buffer
	}

	// rssi
	if( rssi_receive_on )
	{
		//check for rssi symbol
		RFReceiveByte();
#ifdef __RSSI_H__
		if( rf_rx1 == RF_SYMBOL_RSSI_START ) RSSIMeasure();
#endif
	}

	// check the CRC
	crc16=LLCalcCRC16(LL_header_receivebuffer,LL_payload_receivebuffer,rf_limit);


	rf_shift=LL_tail_receivebuffer[0]^(crc16>>8);	//CRCh (implizit typecast; ´takes low byte from long
 	rf_shift|=LL_tail_receivebuffer[1]^crc16;		//CRCl
	if (rf_shift)		// if anything is unlike zero, then crc error was there
	{
		//
		//DebugGiveOut(255);
		//DebugGiveOut(rf_transition2);DebugGiveOut(rf_transition3);	// the artefact type

		//
		//PCLedBlueOn();
		LL_last_data=LL_CRC_ERROR;
		return 0;

	}



	// here the coplete packet is there, crc is ok, subscription is positive now process the packet
	// transition2,transition3 still holds the artefact type, rf_pos holds the status control or not


	if ((LL_payload_receivebuffer[0]==ACL_TYPE_ACM_H) && (LL_payload_receivebuffer[1]==ACL_TYPE_ACM_L) && (ACL_ignore_control_messages==false))
	{
		LL_last_data=LL_ACM;
		ACLProcessControlMessages();	// control message verarbeiten
	}

	if ((LL_payload_receivebuffer[0]!=ACL_TYPE_ACM_H) || (LL_payload_receivebuffer[1]!=ACL_TYPE_ACM_L) || (ACL_pass_control_messages==true))
	{
		LL_last_data=LL_GOOD;
		// if there was not a control msg, but subscription was good anyhow, or pass flag is set, then copy to received
		if (!LLReceiveBufferLocked())
		{
			//copy the header
			for (rf_shift=0;rf_shift<LL_HEADER_SIZE;rf_shift++)
				LL_header_received[rf_shift]=LL_header_receivebuffer[rf_shift];
			//copy the payload
			for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
				LL_payload_received[rf_shift]=LL_payload_receivebuffer[rf_shift];
			//copy the tail
			for (rf_shift=0;rf_shift<LL_TAIL_SIZE;rf_shift++)
				LL_tail_received[rf_shift]=LL_tail_receivebuffer[rf_shift];
			LLSetDataToNew();		//indicate that new data packet has arrived
			LL_received_packet_in_this_slot=true;		//set the trigger that a packet is there
		}
	}

return 1;
}


// receives byte (must be synchronized and descrambles it
//#inline
void RFReceiveScrambledByte()
{
		rf_rx1=0;
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,7);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,6);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,5);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,4);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,3);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,2);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,1);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,0);	// RFScramble the bit and set it into
}


void RFReceiveByte()
{
		rf_rx1=0;
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,7);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,6);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,5);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,4);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,3);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,2);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,1);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,0);
}




// reuse of rf_transition1,
#inline
void RFScramble()
{

	//rf_transition0 is the rf_shift register of the RFScrambler
	rf_transition1=0;
	if (bit_test(rf_transition0,3)) rf_transition1++;
	if (bit_test(rf_transition0,6)) rf_transition1++;
	rf_transition0<<=1;
	if (bit_test(rf_transition1,0)) bit_set(rf_transition0,0);

	//debug
	//rf_transition0=0;	// no scrambling


}



// set a one in the rf_pos 1 with prob. 0.5, in the next slot with 0.25 then with 0.125 and so on
int RFCreateAliveSymbol()
{


	/*
	if (!bit_test(rf_random_l,0)) return 0b00000001;
	if (!bit_test(rf_random_l,1)) return 0b00000011;
	if (!bit_test(rf_random_l,2)) return 0b00000111;
	if (!bit_test(rf_random_l,3)) return 0b00001111;
	if (!bit_test(rf_random_l,4)) return 0b00011111;
	if (!bit_test(rf_random_l,5)) return 0b00111111;
	if (!bit_test(rf_random_l,6)) return 0b01111111;
	if (!bit_test(rf_random_l,7)) return 0b11111111;
	return 0b11111111;

	*/

	if (!bit_test(rf_random_8,0)) return 0b10000000;
	if (!bit_test(rf_random_8,1)) return 0b11000000;
	if (!bit_test(rf_random_8,2)) return 0b11100000;
	if (!bit_test(rf_random_8,3)) return 0b11110000;
	if (!bit_test(rf_random_8,4)) return 0b11111000;
	if (!bit_test(rf_random_8,5)) return 0b11111100;
	if (!bit_test(rf_random_8,6)) return 0b11111110;
	if (!bit_test(rf_random_8,7)) return 0b11111111;
	return 0b11111111;


}


//reuse of rf_pos

void RFNewRandom()
{


	// first the 16bit randomfunction
	rf_pos=0;rf_shift=0;
	if (bit_test(rf_random_l,3)) rf_pos=1;
	if (bit_test(rf_random_h,4)) rf_pos^=1;
	if (bit_test(rf_random_h,6)) rf_shift=1;
	if (bit_test(rf_random_h,7)) rf_shift^=1;
	rf_pos=rf_pos^rf_shift;

	// this makes a 16 bit shift left with the random_l& h registers (they might not be next to each other)
	rf_shift=0;
	if (bit_test(rf_random_l,7)) rf_shift=1;	// save carry bit
	rf_random_l<<=1;							//shift one left
	rf_random_h<<=1;							//shift one left
	if (rf_shift==1) bit_set(rf_random_h,0);	// set carry bit
	// this was 16 bit shift
	if(rf_pos) bit_set(rf_random_l,0);



	// now the 8 bit random funciotn
	/*
	#asm
		RLF     rf_random_8,W
		RLF     rf_random_8,W
		BTFSC   rf_random_8,4
		XORLW   1
		BTFSC   rf_random_8,5
		XORLW   1
		BTFSC   rf_random_8,3
		XORLW   1
		MOVWF   rf_random_8
	#endasm
		rf_random_8 += MY_MAC_ID;				// -> no identical sequence random numbers on any two boards
	*/

	//reuse of rf_pos;
	rf_pos=0;
	/*
	if (bit_test(rf_random_8,2)) rf_pos++;
	if (bit_test(rf_random_8,4)) rf_pos++;
	if (bit_test(rf_random_8,5)) rf_pos++;
	if (bit_test(rf_random_8,7)) rf_pos++;

	rf_random_8=rf_random_8<<1;
	if (bit_test(rf_pos,0)) bit_set(rf_random_8,0); else bit_clear(rf_random_8,0);	//
	*/


		// This calculates parity on the selected bits (mask = 0xb4).
		if(rf_random_8 & 0x80)
			rf_pos = 1;

		if(rf_random_8 & 0x20)
			rf_pos ^= 1;

		if(rf_random_8 & 0x10)
			rf_pos ^= 1;

		if(rf_random_8 & 0x04)
			rf_pos ^= 1;

		rf_random_8 <<= 1;

		rf_random_8 |= rf_pos;

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
		RFSetModeSleep();									// turn off rf module

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


// Calculates arbitrary waiting time within a range determined by LBT_WAITING_TIME_CONSTANT
#inline
long LBTGetWaitingTime()
{
	unsigned long waiting_time;
	unsigned long waiting_time_sum;

		waiting_time = (long)rf_random_h << 8;
		waiting_time += (long)rf_random_l;

		waiting_time_sum = 0;

		waiting_time >>= 3;

		waiting_time_sum = waiting_time + waiting_time + waiting_time + waiting_time + waiting_time;

		return waiting_time_sum + LBT_WAITING_TIME_CONSTANT;
}
