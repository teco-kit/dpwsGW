/**
* Over The Air Programming functions
*
*
* @author: Gregor Schwartz
* @date: 2003-12-10 (yyyy-mm-dd)
*
*	version 002: 2004-11-17 sabin (wendhack@teco.edu)
*		- left-sided hi() and lo() replaced.
*
*
*/



#if PROCESSOR_TYPE == 18452

//	this is for 18f452
//Area in Program Flash Memory that has to be reserved for Otap_loader
#define OTAP_LOADER_START 			0x7980
#define OTAP_LOADER_END				0x7FFF

//Otap_loader main() address
#define OTAP_LOADER_MAIN			0x7D00

//Area in external eeprom that can be used for saving OTAP data
#define OTAP_EXT_EEPROM_FIRST_PAGE	0
#define OTAP_EXT_EEPROM_LAST_PAGE	485			//yes, at least one page in external Eeprom!!

#define OTAP_MAX_NO_LINES			1940		//maximum number of lines that can be stored

#define OTAP_THIS_PROCESSOR			452
#define OTAP_PROGRAM_FLASH_START	4
#define OTAP_PROGRAM_FLASH_END		52
#define OTAP_THIS_VERSION			1

#endif


#if PROCESSOR_TYPE == 186620

//this is for 18f6620
//Area in Program Flash Memory that has to be reserved for Otap_loader
#define OTAP_LOADER_START 			0xB980
#define OTAP_LOADER_END				0xBFFF

//Otap_loader main() address
#define OTAP_LOADER_MAIN			0xBD00

//Area in external eeprom that can be used for saving OTAP data
#define OTAP_EXT_EEPROM_FIRST_PAGE	0
#define OTAP_EXT_EEPROM_LAST_PAGE	485			//yes, at least one page in external Eeprom!!

//Area in Program Flash Memory that can be used for saving OTAP data
#define OTAP_PROGRAM_FLASH_START	0xC000
#define OTAP_PROGRAM_FLASH_END		0xFFFF

#define OTAP_MAX_NO_LINES			2964		//maximum number of lines that can be stored

#define OTAP_THIS_PROCESSOR			6620
#define OTAP_THIS_PROCESSOR_HI		66
#define OTAP_THIS_PROCESSOR_LO		20
#define OTAP_THIS_VERSION			1

#endif


#if PROCESSOR_TYPE == 186720

//this is for 18f6620
//Area in Program Flash Memory that has to be reserved for Otap_loader
#define OTAP_LOADER_START 			0x14000
#define OTAP_LOADER_END				0x146FF

//Otap_loader main() address
#define OTAP_LOADER_MAIN			0x142D0
#define OTAP_LOADER_MAIN_U			0x1			//for "GOTO": 	Upper byte
#define OTAP_LOADER_MAIN_H			0x42		//				High byte
#define OTAP_LOADER_MAIN_L			0xD0		//				Low byte

//Area in external eeprom that can be used for saving OTAP data
#define OTAP_EXT_EEPROM_FIRST_PAGE	0
#define OTAP_EXT_EEPROM_LAST_PAGE	485			//yes, at least one page in external Eeprom!!

//Area in Program Flash Memory that can be used for saving OTAP data
#define OTAP_PROGRAM_FLASH_START	0x14700
#define OTAP_PROGRAM_FLASH_END		0x1FFFF

#define OTAP_MAX_NO_LINES			4900		//maximum number of lines that can be stored

#define OTAP_THIS_PROCESSOR			6720
#define OTAP_THIS_PROCESSOR_HI		67
#define OTAP_THIS_PROCESSOR_LO		20
#define OTAP_THIS_VERSION			1

#endif




//global variables for OTAP
long otap_TotalNumberOfLines;			//the number of coming Otap Packets times 2
long otap_ProgramLines;					//the number of Lines of the new Program
long otap_ExpectingLineNumber;			//next expected Otap Sequence Number
long otap_LastReceivedLineNumber;		//last received Otap Sequence Number
long otap_EepromWriteAddr;				//current Eeprom Write Address
int otap_DelaySlotCounter;				//counts the slots until the Acknowledge is repeated
int otap_BreakCounter;					//counts the slots until Particle gives up Otap
int otap_EndCounter;					//counts how many times the last Acknowledge has been sent
int otap_ProgramCRCh;					//store the Program CRC for later check
int otap_ProgramCRCl;
boolean otap_CRCok;						//stores the check result
boolean otap_packet_just_received;		//Otap packet was received in this slot?
#ifdef OTAP_PROGRAM_FLASH_START
int32 otap_FlashWriteAddr;				//current Program Flash Write Address
#endif

//function prototypes
int PCEepromBytesInThisSlot();
void OtapReset();
void OtapInit();
void OtapSetID();
int OtapIsRunning();
void  OtapCallLoader();
void OtapCRC16(unsigned int ser_data, unsigned int *crc_hb, unsigned int *crc_lb);
boolean OtapCheckProgramCRC();
void OtapReceive();
void OtapAnswerRepeat();






//-------------------------------------------------------
//---------OTAP    functions-----------------------------
//-------------------------------------------------------
//-------------------------------------------------------

/**
* Calculates the number of bytes that can be read or written
* into Eeprom in the current slot
*
* @return number of bytes that can be read or written in current slot
*/
int PCEepromBytesInThisSlot()
{
	long timerest;
	timerest=RFMicroSecondsUntilNextSlot();
	return (int)((timerest-200)/50);			//each byte takes 50 microseconds plus ID/write/read and address
}



/**
* Initializes the Otap global variables.
*
*/
void OtapInit()
{
	OtapReset();
}

/**
* Checks if Otap is running
*
* @return 1: Otap is running. 0: Otap is not running.
*/
int OtapIsRunning()
{
	if(otap_TotalNumberOfLines>0) return 1;
	else return 0;
}

/**
* Breaks Otap by resetting its global variables.
*
*/
void OtapReset()
{
	otap_TotalNumberOfLines=0;
	otap_ProgramLines=0;
	otap_ExpectingLineNumber=0;
	otap_LastReceivedLineNumber=0;
	otap_EepromWriteAddr=OTAP_EXT_EEPROM_FIRST_PAGE*64;
#ifdef OTAP_PROGRAM_FLASH_START
	otap_FlashWriteAddr=OTAP_PROGRAM_FLASH_START;
#endif
	otap_DelaySlotCounter=0;
	otap_BreakCounter=0;
	otap_EndCounter=0;
	otap_CRCok=FALSE;
	otap_ProgramCRCh=0;
	otap_ProgramCRCl=0;
	otap_packet_just_received=false;
}


/**
* Writes the Particle ID from a CID paket into internal Eeprom.
* Sends a CID paket back for testing the new ID.
*
*/
void OtapSetID()
{
	int i;

	for(i=0;i<8;i++)	//write the ID into internal Eeprom
	{
		WriteIntEeprom(EEPROM_ADDRESS_ID+i,LL_payload_receivebuffer[i+6]);
		LL_header_send[3+i]=ReadIntEeprom(EEPROM_ADDRESS_ID+i);
	}
	if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff
	// now set the whole packet by hand:
	LL_payload_send[0]=ACL_TYPE_ACM_HI;
	LL_payload_send[1]=ACL_TYPE_ACM_LO;
	LL_payload_send[2]=0;
	LL_payload_send[3]=ACL_TYPE_CID_HI;
	LL_payload_send[4]=ACL_TYPE_CID_LO;
	LL_payload_send[5]=0;
	LL_payload_length=6;								// give the correct len
	if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
	{
		rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
	}
}


/**
* Calls the Otap Loader.
* The running code will be stopped and new received code will be burned by the Otap Loader.
*
*/
void  OtapCallLoader()
{
	ACLStop();						// halt
	disable_interrupts(global); 	// !!
	#if OTAP_THIS_PROCESSOR == 6720
		#asm						//GOTO OTAP_LOADER_MAIN does not work for OTAP_LOADER_MAIN>0xFFFF, so set the Program Counter by hand
		MOVLW	OTAP_LOADER_MAIN_U
		MOVWF	PIC_PCLATU
		MOVLW	OTAP_LOADER_MAIN_H
	 	MOVWF  	PIC_PCLATH
		MOVLW	OTAP_LOADER_MAIN_L
		MOVWF	PIC_PCL
		#endasm
	#else
		#asm
		GOTO OTAP_LOADER_MAIN			// jump to Otap_loader, which will burn the new program, tested for addresses<0x10000
		#endasm
	#endif
}

/**
* Calculates 16bit CRC from one byte
*
* @param ser_data byte to be CRCed
* @param crc_hb high byte of 16bit CRC output
* @param crc_lb low byte of 16bit CRC output
*/
void OtapCRC16(unsigned int ser_data, unsigned int *crc_hb, unsigned int *crc_lb)
{
	unsigned int hb, lb;
	unsigned int tmp, tmp2;
	hb = *crc_hb;
	lb = *crc_lb;

	tmp = lb;
	lb = hb;
	hb = tmp;

	lb ^= ser_data;
	lb ^= lb >> 4;
	hb ^= lb << 4;

	hb ^= lb >> 3;
	lb ^= (lb << 4) << 1;

	*crc_hb = hb;
	*crc_lb = lb;

}

/**
* Checks the CRC of the whole received and saved program data
*
* @return TRUE: CRC is ok. FALSE: CRC is wrong.
*/
boolean OtapCheckProgramCRC()
{

	int i,crch,crcl;
	long k,lines;
	int otap_load[16];

	ACLStop();		//the check takes several hundred milliseconds
	PCLedRedOn();
	PCLedBlueOn();

	crch=0;crcl=0;
	otap_EepromWriteAddr=OTAP_EXT_EEPROM_FIRST_PAGE*64+64;	//skip the Otap tables page

	#ifdef OTAP_PROGRAM_FLASH_START
	otap_FlashWriteAddr=OTAP_PROGRAM_FLASH_START;
	#endif

	PCEepromInit();						//initialize and power on eeprom module
	PCEepromReadSequence(0,otap_load,16);		//get number of ProgramLines
	lines=((long)otap_load[2]) << 8;
	lines+=(long)otap_load[3];

	for(k=0;k<lines;k++)
	{
		if(k<OTAP_EXT_EEPROM_LAST_PAGE*4)	//first read from Eeprom
		{
			PCEepromReadSequence(otap_EepromWriteAddr, otap_load, 16);
			otap_EepromWriteAddr+=16;
		}

		#ifdef OTAP_PROGRAM_FLASH_START
		else								//then read from Program Flash
		{
			read_program_memory(otap_FlashWriteAddr,otap_load,16);
			otap_FlashWriteAddr+=16;
		}
		#endif

		for(i=0;i<16;i++)
		{
			OtapCRC16(otap_load[i],&crch,&crcl);	//calculate the CRC
		}
	}

	OtapCRC16(otap_ProgramCRCh,&crch,&crcl);
	OtapCRC16(otap_ProgramCRCl,&crch,&crcl);		//check the CRC
	ACLStart();										//restart stack
	if(crch==0 && crcl==0) return TRUE;
	else return FALSE;

}


/**
* Checks the received Otap data.
* If it was the right paket (next Otap sequence number), the data will be stored
* and an acknowledge will be send back
*
*/
void OtapReceive()
{

	int otap_load[58];			//64-3-3
	int enoughspace=FALSE;
	{
		memcpy(otap_load,&(LL_payload_receivebuffer[6]),58);
		if(ACLMatchesMyIP(otap_load,0))			//I must be reprogrammed
		{

			if(otap_load[8]==0 && otap_load[9]==0 && otap_ExpectingLineNumber==0)		//Otap init received
			{
				otap_ProgramLines=((long)otap_load[17]) << 8;
				otap_ProgramLines+=(long)otap_load[18];

				if(otap_ProgramLines<=OTAP_MAX_NO_LINES) enoughspace=TRUE;

				if(otap_load[10]==OTAP_THIS_VERSION && otap_load[11]==OTAP_THIS_PROCESSOR_HI && otap_load[12]==OTAP_THIS_PROCESSOR_LO && enoughspace)
				{
					OtapReset();
					otap_TotalNumberOfLines=((long)otap_load[13]) << 8;
					otap_TotalNumberOfLines+=(long)otap_load[14];
					otap_ProgramCRCh=otap_load[15];
					otap_ProgramCRCl=otap_load[16];




					PCEepromInit();						//initialize and power on eeprom module
					if(PCEepromBytesInThisSlot()>37 && !PCEepromWriteSequence(otap_EepromWriteAddr,&(otap_load[15]),37))	//2+2+2+1+32
							//store Otap tables into Eeprom
					{
						//writing successful, request next line
						otap_EepromWriteAddr+=64;
						otap_LastReceivedLineNumber=1;		//init ack
						otap_ExpectingLineNumber=2;

						if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff

						// now set the whole packet by hand:
						LL_payload_send[0]=ACL_TYPE_ACM_H;
						LL_payload_send[1]=ACL_TYPE_ACM_L;
						LL_payload_send[2]=0;
						LL_payload_send[3]=198;
						LL_payload_send[4]=88;
						LL_payload_send[5]=2;
						LL_payload_send[6]=0;
						LL_payload_send[7]=1;
						LL_payload_length=8;								// give the correct len

						if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
						{
							rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
						}
					}
				}
				else			//Version or Processortype error or not enough space
				{
					//send an error message
					LL_payload_send[0]=ACL_TYPE_ACM_H;
					LL_payload_send[1]=ACL_TYPE_ACM_L;
					LL_payload_send[2]=0;
					LL_payload_send[3]=198;
					LL_payload_send[4]=88;
					LL_payload_send[5]=6;
					LL_payload_send[6]=0xFF;
					LL_payload_send[7]=0xFF;
					LL_payload_send[8]=OTAP_THIS_VERSION;
					LL_payload_send[9]=OTAP_THIS_PROCESSOR_HI;
					LL_payload_send[10]=OTAP_THIS_PROCESSOR_LO;
					LL_payload_send[11]=enoughspace;
					LL_payload_length=12;								// give the correct len

					if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
					{
						rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
					}
				}

			}


			else if(otap_load[8]==hi(otap_ExpectingLineNumber) && otap_load[9]==lo(otap_ExpectingLineNumber))	//next line received
			{
				if(otap_EepromWriteAddr<=OTAP_EXT_EEPROM_LAST_PAGE*64+32)	//still space in Eeprom
				{
					PCEepromInit();
					if(PCEepromBytesInThisSlot()>32 && !PCEepromWriteSequence(otap_EepromWriteAddr,&(otap_load[10]),32))	//this takes 2ms
					{

						otap_EepromWriteAddr+=32;
						otap_LastReceivedLineNumber=otap_ExpectingLineNumber;
					}
				}
				#ifdef OTAP_PROGRAM_FLASH_START
					else			//if no more space in Eeprom, store into Program Flash
					{
						write_program_memory(otap_FlashWriteAddr,&(otap_load[10]),32);		//this takes 10ms,interrupts are disabled !! (This is a CCSC function only available since PCH 3.158)
						otap_FlashWriteAddr+=32;
						otap_LastReceivedLineNumber=otap_ExpectingLineNumber;
					}
				#endif
				if(otap_LastReceivedLineNumber<otap_TotalNumberOfLines)		//acknowledge all packets but the last one here
				{
					if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff
					// now set the whole packet by hand:
					LL_payload_send[0]=ACL_TYPE_ACM_H;
					LL_payload_send[1]=ACL_TYPE_ACM_L;
					LL_payload_send[2]=0;
					LL_payload_send[3]=198;
					LL_payload_send[4]=88;
					LL_payload_send[5]=2;

					LL_payload_send[6]=hi(otap_ExpectingLineNumber);
					LL_payload_send[7]=lo(otap_ExpectingLineNumber);
					LL_payload_length=8;								// give the correct len

					if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
					{
						rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
					}

				}
				otap_ExpectingLineNumber+=2;
				otap_BreakCounter=0;
				otap_DelaySlotCounter=0;
			}
		}
	}
	otap_packet_just_received=true;
}//otap receive



/**
* Repeats sending the last Otap acknowledge packet,
* if the received Otap packet has the wrong Otap sequence number.
*
*/
void OtapAnswerRepeat()
{

	if (otap_packet_just_received==false)
	{
		if((otap_ExpectingLineNumber>otap_TotalNumberOfLines) && !ACLSendingBusy())
		{
			if(otap_EndCounter<15)
			{

				if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff

				if(otap_EndCounter==0) otap_CRCok=OtapCheckProgramCRC();	//check the CRC

				//start new control packet
				LL_payload_send[0]=ACL_TYPE_ACM_H;
				LL_payload_send[1]=ACL_TYPE_ACM_L;
				LL_payload_send[2]=0;
				LL_payload_send[3]=198;
				LL_payload_send[4]=88;
				LL_payload_send[5]=3;

				if(otap_CRCok)		//send a success message, the acknowledge of the last packet
				{
					LL_payload_send[6]=hi(otap_LastReceivedLineNumber);
					LL_payload_send[7]=lo(otap_LastReceivedLineNumber);
				}
				else				//send an CRC error message
				{
					LL_payload_send[6]=0xCC;
					LL_payload_send[7]=0xCC;
				}

				LL_payload_length=8;								// give the correct len
				if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
				{
					rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
				}

				otap_EndCounter++;
			}
			else if(otap_CRCok) OtapCallLoader();
			else OtapReset();
		}

		else if(otap_ExpectingLineNumber!=0)
		{
			if(otap_BreakCounter>100) OtapReset();					//break ota session
			else if(otap_DelaySlotCounter>5 && !ACLSendingBusy())	//repeat the last acknowledge
			{
				if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff
				// now set the whole packet by hand:
				LL_payload_send[0]=ACL_TYPE_ACM_H;
				LL_payload_send[1]=ACL_TYPE_ACM_L;
				LL_payload_send[2]=0;
				LL_payload_send[3]=198;
				LL_payload_send[4]=88;
				LL_payload_send[5]=2;
				LL_payload_send[6]=hi(otap_LastReceivedLineNumber);
				LL_payload_send[7]=lo(otap_LastReceivedLineNumber);
				LL_payload_length=8;								// give the correct len
				if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
				{
					rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
				}

				otap_DelaySlotCounter=0;
				otap_BreakCounter++;
			}
			else if(!ACLSendingBusy()) otap_DelaySlotCounter++;

		}
		otap_packet_just_received=false;
	}
	else
	{
		otap_packet_just_received=false;
	}

}




//The OTAP loader

#org OTAP_LOADER_START,OTAP_LOADER_END {}			// is necessary to preserve memory

#IF OTAP_THIS_PROCESSOR == 452

//this is for 18F452
	//the Otap loader
	#rom 0x7980 = {
	  0xFFFF, 0x50F2, 0x6E03, 0x9EF2, 0x9CA6, 0x8EA6, 0x84A6, 0x010F
	, 0x0E55, 0x6EA7, 0x0EAA, 0x6EA7, 0x82A6, 0x0000, 0x5003, 0x12F2
	, 0x0100, 0x0C00, 0xFFFF, 0xFFFF, 0x8CD0, 0x6AF8, 0xC0BF, 0xFFF7
	, 0xC0BE, 0xFFF6, 0x50F6, 0x0B07, 0x6E00, 0x0EF8, 0x16F6, 0x000A
	, 0xFFFF, 0xC0C1, 0xFFEA, 0xC0C0, 0xFFE9, 0xC0C2, 0xF002, 0xC002
	, 0xF001, 0x5000, 0x2602, 0x0402, 0x0907, 0x6E02, 0x2A02, 0x50F6
	, 0x6600, 0xEFFF, 0xF03C, 0x0F01, 0x0B3F, 0xA4D8, 0xEFFF, 0xF03C
	, 0x88A6, 0x0009, 0xFFFF, 0xECC0, 0xF03C, 0x000A, 0xFFFF, 0x6600
	, 0xEF0C, 0xF03D, 0xCFEE, 0xFFF5, 0x5201, 0xB4D8, 0xEF0D, 0xF03D
	, 0x000F, 0x0601, 0xEF10, 0xF03D, 0x0600, 0x000B, 0xFFFF, 0x000C
	, 0x0E07, 0x14F6, 0x0A07, 0xA4D8, 0xEF18, 0xF03D, 0xECC0, 0xF03C
	, 0x2E02, 0xEFEF, 0xF03C, 0x6AF8, 0x0C00, 0xFFFF, 0xFFFF, 0x0E08
	, 0x6E01, 0x0000, 0x0E04, 0x6E00, 0x2E00, 0xEF24, 0xF03D, 0x9A8C
	, 0x9A95, 0x0E06, 0x6E00, 0x2E00, 0xEF2B, 0xF03D, 0x37B6, 0x9883
	, 0xB0D8, 0x8895, 0xA0D8, 0x9895, 0x8A95, 0xAA83, 0xEF35, 0xF03D
	, 0x2E01, 0xEF21, 0xF03D, 0x0E04, 0x6E00, 0x2E00, 0xEF3D, 0xF03D
	, 0x9A8C, 0x9A95, 0x0000, 0x8895, 0x0E06, 0x6E00, 0x2E00, 0xEF46
	, 0xF03D, 0x0E06, 0x6E00, 0x2E00, 0xEF4B, 0xF03D, 0x8A95, 0xAA83
	, 0xEF4F, 0xF03D, 0x6A01, 0x0E06, 0x6E00, 0x2E00, 0xEF55, 0xF03D
	, 0xB883, 0x8001, 0x9A8C, 0x9A95, 0x9883, 0x9895, 0x0C00, 0xFFFF
	, 0xFFFF, 0x8895, 0x0E04, 0x6E00, 0x2E00, 0xEF64, 0xF03D, 0x8A95
	, 0x0E06, 0x6E00, 0x2E00, 0xEF6A, 0xF03D, 0x9883, 0x9895, 0x0E04
	, 0x6E00, 0x2E00, 0xEF71, 0xF03D, 0x9A8C, 0x9A95, 0x0EA0, 0x6FB6
	, 0xEC1E, 0xF03D, 0x6A03, 0x0EB3, 0x6EE9, 0xC003, 0xFFEA, 0xCFEF
	, 0xF0B4, 0xC0B4, 0xF0B6, 0xEC1E, 0xF03D, 0x6A03, 0x0EB2, 0x6EE9
	, 0xC003, 0xFFEA, 0xCFEF, 0xF0B4, 0xC0B4, 0xF0B6, 0xEC1E, 0xF03D
	, 0x8895, 0x0E04, 0x6E00, 0x2E00, 0xEF93, 0xF03D, 0x8A95, 0x0E06
	, 0x6E00, 0x2E00, 0xEF99, 0xF03D, 0x9883, 0x9895, 0x0E04, 0x6E00
	, 0x2E00, 0xEFA0, 0xF03D, 0x9A8C, 0x9A95, 0x0C00, 0xFFFF, 0xFFFF
	, 0x0E08, 0x6FB9, 0xC000, 0xF0BA, 0x8895, 0x0E06, 0x6E00, 0x2E00
	, 0xEFAF, 0xF03D, 0x8A95, 0xAA83, 0xEFB3, 0xF03D, 0xB883, 0x80D8
	, 0xA883, 0x90D8, 0x3601, 0x0E04, 0x6E00, 0x2E00, 0xEFBD, 0xF03D
	, 0x9A95, 0x9A8C, 0x2FB9, 0xEFAC, 0xF03D, 0x8895, 0x0E06, 0x6E00
	, 0x2E00, 0xEFC8, 0xF03D, 0x9883, 0x51BA, 0xA4D8, 0x9895, 0x0000
	, 0x8A95, 0xAA83, 0xEFD1, 0xF03D, 0x0E04, 0x6E00, 0x2E00, 0xEFD6
	, 0xF03D, 0x9A8C, 0x9A95, 0x0E06, 0x6E00, 0x2E00, 0xEFDD, 0xF03D
	, 0x9883, 0x9895, 0x0C00, 0xFFFF, 0xFFFF, 0x8895, 0x0E04, 0x6E00
	, 0x2E00, 0xEFE8, 0xF03D, 0x8A95, 0x0E06, 0x6E00, 0x2E00, 0xEFEE
	, 0xF03D, 0x9883, 0x9895, 0x0E04, 0x6E00, 0x2E00, 0xEFF5, 0xF03D
	, 0x9A8C, 0x9A95, 0x0EA1, 0x6FB6, 0xEC1E, 0xF03D, 0x6BB5, 0x0E01
	, 0x5DB4, 0x5DB5, 0xB0D8, 0xEF14, 0xF03E, 0x51B5, 0x25B2, 0x6EE9
	, 0x0E00, 0x21B3, 0x6EEA, 0x0E01, 0x6E00, 0xECA7, 0xF03D, 0x5001
	, 0x6EEF, 0x2BB5, 0xEFFF, 0xF03D, 0x51B5, 0x25B2, 0x6EE9, 0x0E00
	, 0x21B3, 0x6EEA, 0x6A00, 0xECA7, 0xF03D, 0x5001, 0x6EEF, 0x9895
	, 0x0000, 0x8A95, 0xAA83, 0xEF22, 0xF03E, 0x0E04, 0x6E00, 0x2E00
	, 0xEF27, 0xF03E, 0x0000, 0x0000, 0x0000, 0x8895, 0x0E04, 0x6E00
	, 0x2E00, 0xEF30, 0xF03E, 0x0C00, 0xFFFF, 0xFFFF, 0xC0B5, 0xF003
	, 0x51B4, 0x6EE9, 0xC003, 0xFFEA, 0x50EF, 0x6FB8, 0xC0B7, 0xF003
	, 0x51B6, 0x6EE9, 0xC003, 0xFFEA, 0x50EF, 0x6FB9, 0xC0B9, 0xF0BA
	, 0xC0B8, 0xF0B9, 0xC0BA, 0xF0B8, 0x51B3, 0x1BB9, 0x39B9, 0x6E00
	, 0x0E0F, 0x1600, 0x5000, 0x1BB9, 0x39B9, 0x6E00, 0x0EF0, 0x1600
	, 0x5000, 0x1BB8, 0x31B9, 0x6E00, 0x3200, 0x3200, 0x0E1F, 0x1600
	, 0x5000, 0x1BB8, 0x39B9, 0x6E00, 0x0EF0, 0x1600, 0x5000, 0x90D8
	, 0x3600, 0x5000, 0x1BB9, 0xC0B5, 0xF003, 0x51B4, 0x6EE9, 0xC003
	, 0xFFEA, 0xC0B8, 0xFFEF, 0xC0B7, 0xF003, 0x51B6, 0x6EE9, 0xC003
	, 0xFFEA, 0xC0B9, 0xFFEF, 0x0C00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
	, 0xFFFF, 0x6AF8, 0x9ED0, 0x6AEA, 0x6AE9, 0x0E06, 0x6EC1, 0x848D
	, 0x828D, 0x8E8C, 0x6BB1, 0x6BB0, 0x6BAD, 0x0E05, 0x6FAC, 0x6BAB
	, 0x0E40, 0x6FAA, 0x6BB3, 0x6BB2, 0xEC60, 0xF03D, 0x0EA1, 0x6FB6
	, 0xEC1E, 0xF03D, 0x0E01, 0x6E00, 0xECA7, 0xF03D, 0xC001, 0xF008
	, 0x0E01, 0x6E00, 0xECA7, 0xF03D, 0xC001, 0xF009, 0x6A03, 0x0EB1
	, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xECA7, 0xF03D, 0x5001
	, 0x6EEF, 0x6A03, 0x0EB0, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00
	, 0xECA7, 0xF03D, 0x5001, 0x6EEF, 0x6A00, 0xECA7, 0xF03D, 0xC001
	, 0xF006, 0x6BA7, 0x6BA6, 0x6A0B, 0x6A0C, 0x6A07, 0x5006, 0x5C07
	, 0xB0D8, 0xEFCF, 0xF03F, 0xC0AD, 0xF0B3, 0xC0AC, 0xF0B2, 0xEC60
	, 0xF03D, 0x0EA1, 0x6FB6, 0xEC1E, 0xF03D, 0x6A03, 0x0EA1, 0x6EE9
	, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xECA7, 0xF03D, 0x5001, 0x6EEF
	, 0x6A03, 0x0EA0, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xECA7
	, 0xF03D, 0x5001, 0x6EEF, 0x6A03, 0x0EA9, 0x6EE9, 0xC003, 0xFFEA
	, 0x0E01, 0x6E00, 0xECA7, 0xF03D, 0x5001, 0x6EEF, 0x6A03, 0x0EA8
	, 0x6EE9, 0xC003, 0xFFEA, 0x6A00, 0xECA7, 0xF03D, 0x5001, 0x6EEF
	, 0x51A0, 0x0BC0, 0x6FA2, 0x51A1, 0x6FA3, 0x51A0, 0x0B3F, 0x6FA4
	, 0x51A1, 0x0B00, 0x6FA5, 0x51A6, 0x5DA2, 0xA4D8, 0xEF15, 0xF03F
	, 0x51A7, 0x5DA3, 0xB4D8, 0xEF27, 0xF03F, 0x53A4, 0xA4D8, 0xEF1D
	, 0xF03F, 0x53A5, 0xB4D8, 0xEF27, 0xF03F, 0x8CD0, 0x6AF8, 0xC0A3
	, 0xFFF7, 0xC0A2, 0xFFF6, 0x88A6, 0xECC0, 0xF03C, 0x6AF8, 0xC0A3
	, 0xF0A7, 0xC0A2, 0xF0A6, 0x0E04, 0x27AC, 0x0E00, 0x23AD, 0xC0AB
	, 0xF0B3, 0xC0AA, 0xF0B2, 0xEC60, 0xF03D, 0x6BAF, 0x6BAE, 0x51AF
	, 0x5DA9, 0xA0D8, 0xEFCC, 0xF03F, 0xA4D8, 0xEF44, 0xF03F, 0x51A8
	, 0x5DAE, 0xB0D8, 0xEFCC, 0xF03F, 0x6BB3, 0x0E10, 0x6FB2, 0x0E10
	, 0x6FB4, 0xECE4, 0xF03D, 0x0E10, 0x27AA, 0x0E00, 0x23AB, 0x8CD0
	, 0x6AF8, 0xC0A1, 0xFFF7, 0xC0A0, 0xFFF6, 0x50F6, 0x0B07, 0x6E00
	, 0x0EF8, 0x16F6, 0x000A, 0xFFFF, 0x6AEA, 0x0E10, 0x6EE9, 0x0E10
	, 0x6E02, 0xC002, 0xF001, 0x5000, 0x2602, 0x0402, 0x0907, 0x6E02
	, 0x2A02, 0x50F6, 0x6600, 0xEF79, 0xF03F, 0x0F01, 0x0B3F, 0xA4D8
	, 0xEF79, 0xF03F, 0x88A6, 0x0009, 0xFFFF, 0xECC0, 0xF03C, 0x000A
	, 0xFFFF, 0x6600, 0xEF86, 0xF03F, 0xCFEE, 0xFFF5, 0x5201, 0xB4D8
	, 0xEF87, 0xF03F, 0x000F, 0x0601, 0xEF8A, 0xF03F, 0x0600, 0x000B
	, 0xFFFF, 0x000C, 0x0E07, 0x14F6, 0x0A07, 0xA4D8, 0xEF92, 0xF03F
	, 0xECC0, 0xF03C, 0x2E02, 0xEF69, 0xF03F, 0x6AF8, 0x6AF8, 0xC0A1
	, 0xFFF7, 0xC0A0, 0xFFF6, 0x6AEA, 0x0E10, 0x6EE9, 0x0E10, 0x6E00
	, 0x0009, 0xFFFF, 0xCFF5, 0xFFEE, 0x2E00, 0xEFA0, 0xF03F, 0x6AF8
	, 0x6A0E, 0x500E, 0x080F, 0xA0D8, 0xEFC3, 0xF03F, 0x0E10, 0x240E
	, 0x6EE9, 0x6AEA, 0xB0D8, 0x2AEA, 0xCFEF, 0xF0B2, 0xC0B2, 0xF0B3
	, 0x6BB5, 0x0E0B, 0x6FB4, 0x6BB7, 0x0E0C, 0x6FB6, 0xEC35, 0xF03E
	, 0x2A0E, 0xEFA9, 0xF03F, 0x0E10, 0x27A0, 0x0E00, 0x23A1, 0x2BAE
	, 0xB4D8, 0x2BAF, 0xEF37, 0xF03F, 0x2A07, 0xEFC6, 0xF03E, 0xC008
	, 0xF0B3, 0x6BB5, 0x0E0B, 0x6FB4, 0x6BB7, 0x0E0C, 0x6FB6, 0xEC35
	, 0xF03E, 0xC009, 0xF0B3, 0x6BB5, 0x0E0B, 0x6FB4, 0x6BB7, 0x0E0C
	, 0x6FB6, 0xEC35, 0xF03E, 0x6BAB, 0x0E40, 0x6FAA, 0x6BAD, 0x0E05
	, 0x6FAC, 0x520B, 0xB4D8, 0xEFF1, 0xF03F, 0x520C, 0xA4D8, 0xEFC1
	, 0xF03E, 0x9E8C, 0x948D, 0x928D, 0x00FF, 0x0003, 0xFFFF, 0xFFFF

	}

#ENDIF

#IF OTAP_THIS_PROCESSOR	== 6620

//this is for 18F6620
#rom 0xB980 = {
  0x0E08, 0x6E01, 0x0000, 0x0000, 0x968C, 0x9695, 0x0000, 0x37B8
, 0x9C83, 0xB0D8, 0x8C95, 0xA0D8, 0x9C95, 0x8695, 0xA683, 0xEFCE
, 0xF05C, 0x2E01, 0xEFC2, 0xF05C, 0x0000, 0x968C, 0x9695, 0x0000
, 0x8C95, 0x0000, 0x0000, 0x8695, 0xA683, 0xEFDC, 0xF05C, 0x6A01
, 0x0000, 0xBC83, 0x8001, 0x968C, 0x9695, 0x9C83, 0x9C95, 0x0C00
, 0x8C95, 0x0000, 0x8695, 0x0000, 0x9C83, 0x9C95, 0x0000, 0x968C
, 0x9695, 0x0EA0, 0x6FB8, 0xECC0, 0xF05C, 0x6A03, 0x0EB5, 0x6EE9
, 0xC003, 0xFFEA, 0xCFEF, 0xF0B6, 0xC0B6, 0xF0B8, 0xECC0, 0xF05C
, 0x6A03, 0x0EB4, 0x6EE9, 0xC003, 0xFFEA, 0xCFEF, 0xF0B6, 0xC0B6
, 0xF0B8, 0xECC0, 0xF05C, 0x8C95, 0x0000, 0x8695, 0x0000, 0x9C83
, 0x9C95, 0x0000, 0x968C, 0x9695, 0x0C00, 0x0E08, 0x6FBB, 0xC000
, 0xF0BC, 0x8C95, 0x0000, 0x8695, 0xA683, 0xEF1C, 0xF05D, 0xBC83
, 0x80D8, 0xAC83, 0x90D8, 0x3601, 0x0000, 0x9695, 0x968C, 0x2FBB
, 0xEF19, 0xF05D, 0x8C95, 0x0000, 0x9C83, 0x51BC, 0xA4D8, 0x9C95
, 0x0000, 0x8695, 0xA683, 0xEF32, 0xF05D, 0x0000, 0x968C, 0x9695
, 0x0000, 0x9C83, 0x9C95, 0x0C00, 0x50F2, 0x6E03, 0x9EF2, 0x9CA6
, 0x8EA6, 0x84A6, 0x010F, 0x0E55, 0x6EA7, 0x0EAA, 0x6EA7, 0x82A6
, 0x0000, 0x5003, 0x12F2, 0x0100, 0x0C00, 0x8C95, 0x0000, 0x8695
, 0x0000, 0x9C83, 0x9C95, 0x0000, 0x968C, 0x9695, 0x0EA1, 0x6FB8
, 0xECC0, 0xF05C, 0x6BB7, 0x0E01, 0x5DB6, 0x5DB7, 0xB0D8, 0xEF70
, 0xF05D, 0x51B7, 0x25B4, 0x6EE9, 0x0E00, 0x21B5, 0x6EEA, 0x0E01
, 0x6E00, 0xEC15, 0xF05D, 0x5001, 0x6EEF, 0x2BB7, 0xEF5B, 0xF05D
, 0x51B7, 0x25B4, 0x6EE9, 0x0E00, 0x21B5, 0x6EEA, 0x6A00, 0xEC15
, 0xF05D, 0x5001, 0x6EEF, 0x9C95, 0x0000, 0x8695, 0xA683, 0xEF7E
, 0xF05D, 0x0000, 0x0000, 0x0000, 0x0000, 0x8C95, 0x0000, 0x0C00
, 0x6AF8, 0xC0B5, 0xFFF7, 0xC0B4, 0xFFF6, 0xC0B7, 0xFFEA, 0xC0B6
, 0xFFE9, 0xC0B8, 0xF000, 0x0009, 0xCFF5, 0xFFEE, 0x2E00, 0xEF93
, 0xF05D, 0x6AF8, 0x0C00, 0x8CD0, 0x6AF8, 0xC0B5, 0xFFF7, 0xC0B4
, 0xFFF6, 0x50F6, 0x0B07, 0x6E00, 0x0EF8, 0x16F6, 0x000A, 0xC0B7
, 0xFFEA, 0xC0B6, 0xFFE9, 0xC0B8, 0xF002, 0xC002, 0xF001, 0x5000
, 0x2602, 0x0402, 0x0907, 0x6E02, 0x2A02, 0x50F6, 0x6600, 0xEFC3
, 0xF05D, 0x0F01, 0x0B3F, 0xA4D8, 0xEFC3, 0xF05D, 0x88A6, 0x0009
, 0xEC3C, 0xF05D, 0x000A, 0x6600, 0xEFD0, 0xF05D, 0xCFEE, 0xFFF5
, 0x5201, 0xB4D8, 0xEFD1, 0xF05D, 0x000F, 0x0601, 0xEFD3, 0xF05D
, 0x0600, 0x000B, 0x000C, 0x0E07, 0x14F6, 0x0A07, 0xA4D8, 0xEFDB
, 0xF05D, 0xEC3C, 0xF05D, 0x2E02, 0xEFB5, 0xF05D, 0x6AF8, 0x0C00
, 0xC0B7, 0xF003, 0x51B6, 0x6EE9, 0xC003, 0xFFEA, 0x50EF, 0x6FBA
, 0xC0B9, 0xF003, 0x51B8, 0x6EE9, 0xC003, 0xFFEA, 0x50EF, 0x6FBB
, 0xC0BB, 0xF0BC, 0xC0BA, 0xF0BB, 0xC0BC, 0xF0BA, 0x51B5, 0x1BBB
, 0x39BB, 0x6E00, 0x0E0F, 0x1600, 0x5000, 0x1BBB, 0x39BB, 0x6E00
, 0x0EF0, 0x1600, 0x5000, 0x1BBA, 0x31BB, 0x6E00, 0x3200, 0x3200
, 0x0E1F, 0x1600, 0x5000, 0x1BBA, 0x39BB, 0x6E00, 0x0EF0, 0x1600
, 0x5000, 0x90D8, 0x3600, 0x5000, 0x1BBB, 0xC0B7, 0xF003, 0x51B6
, 0x6EE9, 0xC003, 0xFFEA, 0xC0BA, 0xFFEF, 0xC0B9, 0xF003, 0x51B8
, 0x6EE9, 0xC003, 0xFFEA, 0xC0BB, 0xFFEF, 0x0C00, 0xFFFF, 0xFFFF

}
#rom 0xBD00 = {
  0x6AF8, 0x9ED0, 0x6AEA, 0x6AE9, 0x0E0F, 0x6EC1, 0x0E07, 0x6EB4
, 0x848B, 0x9C97, 0x8C8E, 0x8E8C, 0x6BB1, 0x6BB0, 0x6BAD, 0x0E05
, 0x6FAC, 0x6BAB, 0x0E40, 0x6FAA, 0x0EC0, 0x6FB3, 0x6BB2, 0x6BB5
, 0x6BB4, 0xECE8, 0xF05C, 0x0EA1, 0x6FB8, 0xECC0, 0xF05C, 0x0E01
, 0x6E00, 0xEC15, 0xF05D, 0xC001, 0xF008, 0x0E01, 0x6E00, 0xEC15
, 0xF05D, 0xC001, 0xF009, 0x6A03, 0x0EB1, 0x6EE9, 0xC003, 0xFFEA
, 0x0E01, 0x6E00, 0xEC15, 0xF05D, 0x5001, 0x6EEF, 0x6A03, 0x0EB0
, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xEC15, 0xF05D, 0x5001
, 0x6EEF, 0x6A00, 0xEC15, 0xF05D, 0xC001, 0xF006, 0x6BA7, 0x6BA6
, 0x6A0B, 0x6A0C, 0x6A07, 0x5006, 0x5C07, 0xB0D8, 0xEFAF, 0xF05F
, 0xC0AD, 0xF0B5, 0xC0AC, 0xF0B4, 0xECE8, 0xF05C, 0x0EA1, 0x6FB8
, 0xECC0, 0xF05C, 0x6A03, 0x0EA1, 0x6EE9, 0xC003, 0xFFEA, 0x0E01
, 0x6E00, 0xEC15, 0xF05D, 0x5001, 0x6EEF, 0x6A03, 0x0EA0, 0x6EE9
, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xEC15, 0xF05D, 0x5001, 0x6EEF
, 0x6A03, 0x0EA9, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xEC15
, 0xF05D, 0x5001, 0x6EEF, 0x6A03, 0x0EA8, 0x6EE9, 0xC003, 0xFFEA
, 0x6A00, 0xEC15, 0xF05D, 0x5001, 0x6EEF, 0x51A0, 0x0BC0, 0x6FA2
, 0x51A1, 0x6FA3, 0x51A0, 0x0B3F, 0x6FA4, 0x51A1, 0x0B00, 0x6FA5
, 0x51A6, 0x5DA2, 0xA4D8, 0xEF1A, 0xF05F, 0x51A7, 0x5DA3, 0xB4D8
, 0xEF2C, 0xF05F, 0x53A4, 0xA4D8, 0xEF22, 0xF05F, 0x53A5, 0xB4D8
, 0xEF2C, 0xF05F, 0x8CD0, 0x6AF8, 0xC0A3, 0xFFF7, 0xC0A2, 0xFFF6
, 0x88A6, 0xEC3C, 0xF05D, 0x6AF8, 0xC0A3, 0xF0A7, 0xC0A2, 0xF0A6
, 0x0E04, 0x27AC, 0x0E00, 0x23AD, 0xC0AB, 0xF0B5, 0xC0AA, 0xF0B4
, 0xECE8, 0xF05C, 0x6BAF, 0x6BAE, 0x51AF, 0x5DA9, 0xA0D8, 0xEFAC
, 0xF05F, 0xA4D8, 0xEF49, 0xF05F, 0x51A8, 0x5DAE, 0xB0D8, 0xEFAC
, 0xF05F, 0x51AF, 0x0807, 0xA0D8, 0xEF63, 0xF05F, 0xA4D8, 0xEF56
, 0xF05F, 0x51AE, 0x0893, 0xA0D8, 0xEF63, 0xF05F, 0x6BB5, 0x0E10
, 0x6FB4, 0x0E10, 0x6FB6, 0xEC4D, 0xF05D, 0x0E10, 0x27AA, 0x0E00
, 0x23AB, 0xEF72, 0xF05F, 0xC0B3, 0xF0B5, 0xC0B2, 0xF0B4, 0x6BB7
, 0x0E10, 0x6FB6, 0x0E10, 0x6FB8, 0xEC88, 0xF05D, 0x0E10, 0x27B2
, 0x0E00, 0x23B3, 0xC0A1, 0xF0B5, 0xC0A0, 0xF0B4, 0x6BB7, 0x0E10
, 0x6FB6, 0x0E10, 0x6FB8, 0xEC9B, 0xF05D, 0xC0A1, 0xF0B5, 0xC0A0
, 0xF0B4, 0x6BB7, 0x0E10, 0x6FB6, 0x0E10, 0x6FB8, 0xEC88, 0xF05D
, 0x6A0E, 0x500E, 0x080F, 0xA0D8, 0xEFA3, 0xF05F, 0x0E10, 0x240E
, 0x6EE9, 0x6AEA, 0xB0D8, 0x2AEA, 0xCFEF, 0xF0B4, 0xC0B4, 0xF0B5
, 0x6BB7, 0x0E0B, 0x6FB6, 0x6BB9, 0x0E0C, 0x6FB8, 0xECE0, 0xF05D
, 0x2A0E, 0xEF89, 0xF05F, 0x0E10, 0x27A0, 0x0E00, 0x23A1, 0x2BAE
, 0xB4D8, 0x2BAF, 0xEF3C, 0xF05F, 0x2A07, 0xEFCB, 0xF05E, 0xC008
, 0xF0B5, 0x6BB7, 0x0E0B, 0x6FB6, 0x6BB9, 0x0E0C, 0x6FB8, 0xECE0
, 0xF05D, 0xC009, 0xF0B5, 0x6BB7, 0x0E0B, 0x6FB6, 0x6BB9, 0x0E0C
, 0x6FB8, 0xECE0, 0xF05D, 0x6BAB, 0x0E40, 0x6FAA, 0x6BAD, 0x0E05
, 0x6FAC, 0x520B, 0xB4D8, 0xEFD1, 0xF05F, 0x520C, 0xA4D8, 0xEF8C
, 0xF05E, 0x9E8C, 0x948B, 0x9C97, 0x9C8E, 0x00FF, 0x0003, 0xFFFF

}

#org OTAP_PROGRAM_FLASH_START,OTAP_PROGRAM_FLASH_END {}			// preserve for saving OTAP data

#ENDIF


#IF OTAP_THIS_PROCESSOR	== 6720

//this is for 18F6720
#rom 0x14000 = {
  0x0E08, 0x6E01, 0x0000, 0x0000, 0x968C, 0x9695, 0x0000, 0x37C2
, 0x9C83, 0xB0D8, 0x8C95, 0xA0D8, 0x9C95, 0x8695, 0xA683, 0xEF0E
, 0xF0A0, 0x2E01, 0xEF02, 0xF0A0, 0x0000, 0x968C, 0x9695, 0x0000
, 0x8C95, 0x0000, 0x0000, 0x8695, 0xA683, 0xEF1C, 0xF0A0, 0x6A01
, 0x0000, 0xBC83, 0x8001, 0x968C, 0x9695, 0x9C83, 0x9C95, 0x0C00
, 0x8C95, 0x0000, 0x8695, 0x0000, 0x9C83, 0x9C95, 0x0000, 0x968C
, 0x9695, 0x0EA0, 0x6FC2, 0xEC00, 0xF0A0, 0x6A03, 0x0EBF, 0x6EE9
, 0xC003, 0xFFEA, 0xCFEF, 0xF0C0, 0xC0C0, 0xF0C2, 0xEC00, 0xF0A0
, 0x6A03, 0x0EBE, 0x6EE9, 0xC003, 0xFFEA, 0xCFEF, 0xF0C0, 0xC0C0
, 0xF0C2, 0xEC00, 0xF0A0, 0x8C95, 0x0000, 0x8695, 0x0000, 0x9C83
, 0x9C95, 0x0000, 0x968C, 0x9695, 0x0C00, 0x0E08, 0x6FC5, 0xC000
, 0xF0C6, 0x8C95, 0x0000, 0x8695, 0xA683, 0xEF5C, 0xF0A0, 0xBC83
, 0x80D8, 0xAC83, 0x90D8, 0x3601, 0x0000, 0x9695, 0x968C, 0x2FC5
, 0xEF59, 0xF0A0, 0x8C95, 0x0000, 0x9C83, 0x51C6, 0xA4D8, 0x9C95
, 0x0000, 0x8695, 0xA683, 0xEF72, 0xF0A0, 0x0000, 0x968C, 0x9695
, 0x0000, 0x9C83, 0x9C95, 0x0C00, 0x50F2, 0x6E03, 0x9EF2, 0x9CA6
, 0x8EA6, 0x84A6, 0x010F, 0x0E55, 0x6EA7, 0x0EAA, 0x6EA7, 0x82A6
, 0x0000, 0x5003, 0x12F2, 0x0100, 0x0C00, 0x8C95, 0x0000, 0x8695
, 0x0000, 0x9C83, 0x9C95, 0x0000, 0x968C, 0x9695, 0x0EA1, 0x6FC2
, 0xEC00, 0xF0A0, 0x6BC1, 0x0E01, 0x5DC0, 0x5DC1, 0xB0D8, 0xEFB0
, 0xF0A0, 0x51C1, 0x25BE, 0x6EE9, 0x0E00, 0x21BF, 0x6EEA, 0x0E01
, 0x6E00, 0xEC55, 0xF0A0, 0x5001, 0x6EEF, 0x2BC1, 0xEF9B, 0xF0A0
, 0x51C1, 0x25BE, 0x6EE9, 0x0E00, 0x21BF, 0x6EEA, 0x6A00, 0xEC55
, 0xF0A0, 0x5001, 0x6EEF, 0x9C95, 0x0000, 0x8695, 0xA683, 0xEFBE
, 0xF0A0, 0x0000, 0x0000, 0x0000, 0x0000, 0x8C95, 0x0000, 0x0C00
, 0xC0C0, 0xFFF8, 0xC0BF, 0xFFF7, 0xC0BE, 0xFFF6, 0xC0C3, 0xFFEA
, 0xC0C2, 0xFFE9, 0xC0C4, 0xF000, 0x0009, 0xCFF5, 0xFFEE, 0x2E00
, 0xEFD4, 0xF0A0, 0x6AF8, 0x0C00, 0x8CD0, 0xC0C0, 0xFFF8, 0xC0BF
, 0xFFF7, 0xC0BE, 0xFFF6, 0x50F6, 0x0B07, 0x6E00, 0x0EF8, 0x16F6
, 0x000A, 0xC0C3, 0xFFEA, 0xC0C2, 0xFFE9, 0xC0C4, 0xF002, 0xC002
, 0xF001, 0x5000, 0x2602, 0x0402, 0x0907, 0x6E02, 0x2A02, 0x50F6
, 0x6600, 0xEF05, 0xF0A1, 0x0F01, 0x0B3F, 0xA4D8, 0xEF05, 0xF0A1
, 0x88A6, 0x0009, 0xEC7C, 0xF0A0, 0x000A, 0x6600, 0xEF12, 0xF0A1
, 0xCFEE, 0xFFF5, 0x5201, 0xB4D8, 0xEF13, 0xF0A1, 0x000F, 0x0601
, 0xEF15, 0xF0A1, 0x0600, 0x000B, 0x000C, 0x0E07, 0x14F6, 0x0A07
, 0xA4D8, 0xEF1D, 0xF0A1, 0xEC7C, 0xF0A0, 0x2E02, 0xEFF7, 0xF0A0
, 0x6AF8, 0x0C00, 0xC0C1, 0xF003, 0x51C0, 0x6EE9, 0xC003, 0xFFEA
, 0x50EF, 0x6FC4, 0xC0C3, 0xF003, 0x51C2, 0x6EE9, 0xC003, 0xFFEA
, 0x50EF, 0x6FC5, 0xC0C5, 0xF0C6, 0xC0C4, 0xF0C5, 0xC0C6, 0xF0C4
, 0x51BF, 0x1BC5, 0x39C5, 0x6E00, 0x0E0F, 0x1600, 0x5000, 0x1BC5
, 0x39C5, 0x6E00, 0x0EF0, 0x1600, 0x5000, 0x1BC4, 0x31C5, 0x6E00
, 0x3200, 0x3200, 0x0E1F, 0x1600, 0x5000, 0x1BC4, 0x39C5, 0x6E00
, 0x0EF0, 0x1600, 0x5000, 0x90D8, 0x3600, 0x5000, 0x1BC5, 0xC0C1
, 0xF003, 0x51C0, 0x6EE9, 0xC003, 0xFFEA, 0xC0C4, 0xFFEF, 0xC0C3
, 0xF003, 0x51C2, 0x6EE9, 0xC003, 0xFFEA, 0xC0C5, 0xFFEF, 0x0C00
, 0x6AF8, 0x9ED0, 0x6AEA, 0x6AE9, 0x0E0F, 0x6EC1, 0x0E07, 0x6EB4
, 0x6A94, 0x6A97, 0x6A95, 0x6BB3, 0x6BB2, 0x6BB1, 0x6BB0, 0x848B
, 0x9C97, 0x8C8E, 0x8E8C, 0x6BBD, 0x6BBC, 0x6BB9, 0x0E05, 0x6FB8
, 0x6BB7, 0x0E40, 0x6FB6, 0x6BAF, 0x0E01, 0x6FAE, 0x0E47, 0x6FAD
, 0x6BAC, 0x6BBF, 0x6BBE, 0xEC28, 0xF0A0, 0x0EA1, 0x6FC2, 0xEC00
, 0xF0A0, 0x0E01, 0x6E00, 0xEC55, 0xF0A0, 0xC001, 0xF008, 0x0E01
, 0x6E00, 0xEC55, 0xF0A0, 0xC001, 0xF009, 0x6A03, 0x0EBD, 0x6EE9
, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xEC55, 0xF0A0, 0x5001, 0x6EEF
, 0x6A03, 0x0EBC, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xEC55
, 0xF0A0, 0x5001, 0x6EEF, 0x6A00, 0xEC55, 0xF0A0, 0xC001, 0xF006
, 0x6BAB, 0x6BAA, 0x6BA9, 0x6BA8, 0x6A0B, 0x6A0C, 0x6A07, 0x5006
, 0x5C07, 0xB0D8, 0xEFDA, 0xF0A2, 0xC0B9, 0xF0BF, 0xC0B8, 0xF0BE
, 0xEC28, 0xF0A0, 0x0EA1, 0x6FC2, 0xEC00, 0xF0A0, 0x6A03, 0x0EB1
, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00, 0xEC55, 0xF0A0, 0x5001
, 0x6EEF, 0x6A03, 0x0EB0, 0x6EE9, 0xC003, 0xFFEA, 0x0E01, 0x6E00
, 0xEC55, 0xF0A0, 0x5001, 0x6EEF, 0x6A03, 0x0EB5, 0x6EE9, 0xC003
, 0xFFEA, 0x0E01, 0x6E00, 0xEC55, 0xF0A0, 0x5001, 0x6EEF, 0x6A03
, 0x0EB4, 0x6EE9, 0xC003, 0xFFEA, 0x6A00, 0xEC55, 0xF0A0, 0x5001
, 0x6EEF, 0x51B0, 0x0BC0, 0x6FA0, 0x51B1, 0x6FA1, 0x51B2, 0x0B01
, 0x6FA2, 0x51B3, 0x0B00, 0x6FA3, 0x51B0, 0x0B3F, 0x6FA4, 0x51B1
, 0x0B00, 0x6FA5, 0x51B2, 0x0B00, 0x6FA6, 0x51B3, 0x0B00, 0x6FA7
, 0x51A8, 0x5DA0, 0xA4D8, 0xEF24, 0xF0A2, 0x51A9, 0x5DA1, 0xA4D8
, 0xEF24, 0xF0A2, 0x51AA, 0x5DA2, 0xA4D8, 0xEF24, 0xF0A2, 0x51AB
, 0x5DA3, 0xB4D8, 0xEF3F, 0xF0A2, 0x53A4, 0xA4D8, 0xEF34, 0xF0A2
, 0x53A5, 0xA4D8, 0xEF34, 0xF0A2, 0x53A6, 0xA4D8, 0xEF34, 0xF0A2
, 0x53A7, 0xB4D8, 0xEF3F, 0xF0A2, 0x8CD0, 0xC0A2, 0xFFF8, 0xC0A1
, 0xFFF7, 0xC0A0, 0xFFF6, 0x88A6, 0xEC7C, 0xF0A0, 0x6AF8, 0xC0A3
, 0xF0AB, 0xC0A2, 0xF0AA, 0xC0A1, 0xF0A9, 0xC0A0, 0xF0A8, 0x0E04
, 0x27B8, 0x0E00, 0x23B9, 0xC0B7, 0xF0BF, 0xC0B6, 0xF0BE, 0xEC28
, 0xF0A0, 0x6BBB, 0x6BBA, 0x51BB, 0x5DB5, 0xA0D8, 0xEFD7, 0xF0A2
, 0xA4D8, 0xEF60, 0xF0A2, 0x51B4, 0x5DBA, 0xB0D8, 0xEFD7, 0xF0A2
, 0x51BB, 0x0807, 0xA0D8, 0xEF7A, 0xF0A2, 0xA4D8, 0xEF6D, 0xF0A2
, 0x51BA, 0x0893, 0xA0D8, 0xEF7A, 0xF0A2, 0x6BBF, 0x0E10, 0x6FBE
, 0x0E10, 0x6FC0, 0xEC8D, 0xF0A0, 0x0E10, 0x27B6, 0x0E00, 0x23B7
, 0xEF91, 0xF0A2, 0xC0AF, 0xF0C1, 0xC0AE, 0xF0C0, 0xC0AD, 0xF0BF
, 0xC0AC, 0xF0BE, 0x6BC3, 0x0E10, 0x6FC2, 0x0E10, 0x6FC4, 0xECC8
, 0xF0A0, 0x0E10, 0x27AC, 0x0E00, 0x23AD, 0x0E00, 0x23AE, 0x0E00
, 0x23AF, 0xC0B3, 0xF0C1, 0xC0B2, 0xF0C0, 0xC0B1, 0xF0BF, 0xC0B0
, 0xF0BE, 0x6BC3, 0x0E10, 0x6FC2, 0x0E10, 0x6FC4, 0xECDC, 0xF0A0
, 0xC0B3, 0xF0C1, 0xC0B2, 0xF0C0, 0xC0B1, 0xF0BF, 0xC0B0, 0xF0BE
, 0x6BC3, 0x0E10, 0x6FC2, 0x0E10, 0x6FC4, 0xECC8, 0xF0A0, 0x6A0E
, 0x500E, 0x080F, 0xA0D8, 0xEFCA, 0xF0A2, 0x0E10, 0x240E, 0x6EE9
, 0x6AEA, 0xB0D8, 0x2AEA, 0xCFEF, 0xF0BE, 0xC0BE, 0xF0BF, 0x6BC1
, 0x0E0B, 0x6FC0, 0x6BC3, 0x0E0C, 0x6FC2, 0xEC22, 0xF0A1, 0x2A0E
, 0xEFB0, 0xF0A2, 0x0E10, 0x27B0, 0x0E00, 0x23B1, 0x0E00, 0x23B2
, 0x0E00, 0x23B3, 0x2BBA, 0xB4D8, 0x2BBB, 0xEF53, 0xF0A2, 0x2A07
, 0xEFBF, 0xF0A1, 0xC008, 0xF0BF, 0x6BC1, 0x0E0B, 0x6FC0, 0x6BC3
, 0x0E0C, 0x6FC2, 0xEC22, 0xF0A1, 0xC009, 0xF0BF, 0x6BC1, 0x0E0B
, 0x6FC0, 0x6BC3, 0x0E0C, 0x6FC2, 0xEC22, 0xF0A1, 0x6BB7, 0x0E40
, 0x6FB6, 0x6BB9, 0x0E05, 0x6FB8, 0x520B, 0xB4D8, 0xEFFC, 0xF0A2
, 0x520C, 0xA4D8, 0xEF7B, 0xF0A1, 0x9E8C, 0x948B, 0x9C97, 0x9C8E
, 0x00FF, 0x0003, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF

}


#org OTAP_PROGRAM_FLASH_START,OTAP_PROGRAM_FLASH_END {}			// preserve for saving OTAP data

#ENDIF


