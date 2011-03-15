int LLGetIDFromHardware()
{
	int buff;

	//return;
	//FlashInit();
	//RFPowerModuleOn();
	//RFSetModeSleep();

	//check if valid ID in Eeprom:

	// the new stuff: get the id from the 1-wire


	buff=PCGetID(&LL_header_send[3]);
	return buff;


	/* storage of id is discontinued at the moment...
	//

	result=0;
	for (i=0;i<=7;i++)
	{
		buff=ReadIntEeprom(EEPROM_ADDRESS_ID+i);
		LL_header_send[3+i]=buff;
		if (buff==255) result++;
	}
	if (result<8)
	{
		//ID in internal EEProm was valid!
		//now write it to external eeprom and exit


		FlashWriteSequence(FLASH_ID_ADDRESS, &LL_header_send[3],8);
		FlashFlush();
		return 1;
	}


	// id in internal EEPROM was invalid; try external eeprom

		FlashReadSequence(FLASH_ID_ADDRESS,&LL_header_send[3],8);
		result=0;
		for(i=0;i<=7;i++)
		{
			buff=LL_header_send[3+i];
			if (buff==255) result++;
		}
		if (result<8)
		{
			//external was ok, write it to internal
			for(i=0;i<=7;i++)
			{
				WriteIntEeprom(EEPROM_ADDRESS_ID+i,LL_header_send[i+3]);
			}
			return 1;
		}


	return 0;		//both were invalid: there's no valid id
	*/
}






void LLInit()
{
	LL_sequence_no=0;


	//prepare standard header with Ids



	LL_header_send[0]=VERSION;



	/*
	LL_header_send[3]=ReadIntEeprom(EEPROM_ADDRESS_ID+0);
	LL_header_send[4]=ReadIntEeprom(EEPROM_ADDRESS_ID+1);
	LL_header_send[5]=ReadIntEeprom(EEPROM_ADDRESS_ID+2);
	LL_header_send[6]=ReadIntEeprom(EEPROM_ADDRESS_ID+3);
	LL_header_send[7]=ReadIntEeprom(EEPROM_ADDRESS_ID+4);
	LL_header_send[8]=ReadIntEeprom(EEPROM_ADDRESS_ID+5);
	LL_header_send[9]=ReadIntEeprom(EEPROM_ADDRESS_ID+6);
	LL_header_send[10]=ReadIntEeprom(EEPROM_ADDRESS_ID+7);
	*/


	LL_payload_length=0;	// set used ll payload to zero
	LL_receive_buffer_locked=false;
	LL_received_packet_in_this_slot=false;
	LL_last_data=LL_NONE;

	RFInit();

	LLGetIDFromHardware();	//take id from internal or external eeprom and set ll_header_send
}




void LLStart()
{
	RFStart();
}


void LLStop()
{
	RFStop();	// handles the states
}



void LLSlotEnd()
{
	// check if timeout for waiting packet
	if (LLSendingBusy())
	{
		LL_slots_left--;
		if (LL_slots_left==0)
		{
			rf_status&=~RF_STATUS_SENDING;					// busy is over

			if (rf_status & RF_STATUS_INSERTED_CTRLMSG)
			{
				rf_status&=~RF_STATUS_INSERTED_CTRLMSG;		// in case it was switched on, switch it off and do NOT update the success state
			}
			else
			{
				rf_status&=~RF_STATUS_SEND_SUCCESS;
			}

			LL_payload_length=0;							// set used payload to zero
		}
	}

	// if msg received in last slot set appropr. bit

}



void LLAbortSending()
{
	if (LLSendingBusy())
	{
		if (rf_status & RF_STATUS_INSERTED_CTRLMSG)
		{
			rf_status&=~RF_STATUS_INSERTED_CTRLMSG;
			rf_status&=~RF_STATUS_SENDING;
		}
		else
		{
			rf_status&=~RF_STATUS_SEND_SUCCESS;
			rf_status&=~RF_STATUS_SENDING;
		}
		LL_payload_length=0;
	}
}


void LLSetSendingSuccess()
{
	//update the states
	if (rf_status & RF_STATUS_INSERTED_CTRLMSG)
	{
		// no update of success flag
		rf_status&= ~RF_STATUS_INSERTED_CTRLMSG;	// clear the control management bit
		rf_status&= ~RF_STATUS_SENDING;				// busy is over
		LL_payload_length=0;						// set used payload to zero
	}
	else
	{
		rf_status|= RF_STATUS_SEND_SUCCESS;			// succesfull sent
		rf_status&= ~RF_STATUS_SENDING;				// busy is over
		LL_payload_length=0;						// set used payload to zero
	}
}


int LLSendingBusy()
{
	if (rf_status & RF_STATUS_SENDING) return 1;
	return 0;
}

// !! this returns the last(!) result, also if sending is active at the moment
int LLGetSendSuccess()
{
	if (rf_status & RF_STATUS_SEND_SUCCESS) return 1;
	else return 0;
}

int LLIsActive()
{
	if (rf_status& RF_STATUS_LAYER_ON) return 1;
	else return 0;
}


void LLLockReceiveBuffer()
{
	LL_receive_buffer_locked=true;

//	LLSetDataToOld();
}

int LLReceiveBufferLocked()
{
	if (LL_receive_buffer_locked==true) return 1;
	return 0;
}


void LLReleaseReceiveBuffer()
{
	LL_receive_buffer_locked=false;
//	LLSetDataToOld();
}


void LLSetDataToOld()
{
	rf_status&=~RF_STATUS_NEW_DATA;
}

void LLSetDataToNew()
{
	rf_status|=RF_STATUS_NEW_DATA;
}


int LLDataIsNew()
{
	if (rf_status & RF_STATUS_NEW_DATA) return 1;
	return 0;
}





int LLGetFieldStrength()
{
	return RFGetFieldStrength();
}

void LLSetFieldStrength(int value)
{
	RFSetFieldStrength(value);
}


long LLCalcCRC16(byte *header_data,byte *payload_data,byte payload_size)
{
	//calcs a crc on LL_header, ACL_data, LL_tail
	int hb,lb,i,tmp;
	lb=0;
	hb=0;

	for(i=0;i<LL_HEADER_SIZE;i++)
	{
		tmp = lb;
		lb = hb;
		hb = tmp;
		lb ^= header_data[i];
		lb ^= lb >> 4;
		hb ^= lb << 4;
		hb ^= lb >> 3;
		lb ^= (lb << 4) << 1;
	}

	//len=header_data[1]-LL_HEADER_SIZE-LL_TAIL_SIZE;
	for(i=0;i<payload_size;i++)
	{
		tmp = lb;
		lb = hb;
		hb = tmp;
		lb ^= payload_data[i];
		lb ^= lb >> 4;
		hb ^= lb << 4;
		hb ^= lb >> 3;
		lb ^= (lb << 4) << 1;
	}


	return hb*256+lb;
}







// prepares the header and sends the packet out
int LLSendPacket(int slot_limit)
{
	//long crc16; // makes RAM too full if local var is active



	if (slot_limit==0) slot_limit=1;	// zero makes no sense

	if (!(rf_status & RF_STATUS_LAYER_ON)) return 0;
	if (rf_status & RF_STATUS_SENDING) return 0;


	// finalize the LLPacket

	//DebugGiveOut(LL_payload_length);


	LL_header_send[1]=LL_payload_length;
	LL_header_send[2]=RFGetFieldStrength();
	LL_header_send[11]=LL_sequence_no;


	LL_longbuffer=LLCalcCRC16(LL_header_send,LL_payload_send, LL_payload_length);

	LL_tail_send[0]=(LL_longbuffer>>8);	//CRCh (implizit typecast; ´takes low byte from long
	LL_tail_send[1]=LL_longbuffer;		//CRCl

	LL_slots_left=slot_limit;
	LL_sequence_no++;

	rf_status|= RF_STATUS_SENDING;

	return 1;
}


int LLGetRemainingPayloadSpace()
{

	return (LL_PAYLOAD_SIZE-LL_payload_length);

}


int LLSentPacketInThisSlot()
{
	if (rf_status & RF_STATUS_JUST_SENT) return 1;
	return 0;
}

