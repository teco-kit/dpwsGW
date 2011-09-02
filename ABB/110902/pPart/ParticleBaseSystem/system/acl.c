int ACL_payload_send[LL_PAYLOAD_SIZE];
int ACL_payload_length;

boolean ACL_send_buffer_locked;
boolean ACL_ACM_answers;		// answer on ACM messages like helo and so on
int ACL_write_position;

#if 0
int ACL_subscriptions[ACL_SUBSCRIPTIONLIST_LENGTH][2];
boolean ACL_subscribe_all;
boolean ACL_ignore_control_messages;
boolean ACL_pass_control_messages;
#endif


void ACLStartUp()
{
	int result[10];
	int selftest_result,i;

	selftest_result=AppSelfTest(result);		//runs the selftest


	//!!
	//PCLedBlueOff();
	//PCLedRedOff();
	DelayMs(100);				// for stabilizing of power etc.

	ACLInit();					// sets the pins in pic init correct (looks at selftest_active

	if(selftest_result==1)
	{
		for(i=0;i<10;i++)			//send result ten times to get it
		{
			//send packet
			ACLAddNewType(ACL_TYPE_CST_HI,ACL_TYPE_CST_LO);		//MST Self test
			ACLAddData(result[0]);
			ACLAddData(result[1]);
			ACLAddData(result[2]);
			ACLAddData(result[3]);
			ACLAddData(result[4]);
			ACLAddData(result[5]);
			ACLAddData(result[9]);

			ACLSendPacket(30);
			while(ACLSendingBusy()) {/*//PCLedRedOn();//PCLedRedOff();*/}

		}

		while(selftest_active)				//wait for selftest board to be removed
		{
			output_high(PIN_D7);
			if (!input(PIN_B7)) selftest_active=false;
			output_low(PIN_D7);
			if (input(PIN_B7)) selftest_active=false;

		}

		ACLInit();					//restart the whole stack once more
	}


}




void ACLInit()
{

	ACLStop();	//hold stack if already running


	ACLFlushSubscriptions();
	ACL_send_buffer_locked=false;
 	ACL_write_position=0;
 	ACL_subscribe_all=false;
 	ACL_ACM_answers=true;
 	ACL_ignore_control_messages=false;
 	ACL_pass_control_messages=false;

	ACL_payload_length=0;
	ACL_write_position=0;
#ifdef USE_OTAP
	OtapInit();			//reset all global variables for otap
#endif

	//AppSetLEDBehaviour(LEDS_NORMAL);			//setdefault

	if (rf_LED_style!=LEDS_OFF)
	{
		PCLedRedOn();
		DelayMs(200);
		PCLedRedOff();
		DelayMs(200);
	}

	LLInit();
	ACLStart();


}






int ACLSubscribe(byte LL_type_h,LL_type_l)
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)		// first test if already there
	{
		if ((ACL_subscriptions[i][0]==LL_type_h) && (ACL_subscriptions[i][1]==LL_type_l)) return 1;
	}

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)
	{
		if ((ACL_subscriptions[i][0]==0) && (ACL_subscriptions[i][1]==0))
		{
			ACL_subscriptions[i][0]=LL_type_h;
			ACL_subscriptions[i][1]=LL_type_l;
			return 1;								// successful subscribed
		}
	}
	return 0;										// subsriptions failed
}

int ACLUnsubscribe(byte LL_type_h,LL_type_l)
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)		// if type is there, delete it
	{
		if ((ACL_subscriptions[i][0]==LL_type_h) && (ACL_subscriptions[i][1]==LL_type_l))
		{
			ACL_subscriptions[i][0]=0;
			ACL_subscriptions[i][1]=0;
			return 1;		// successufl unsubscribed
		}
	}
	return 0;		// not found
}

void ACLFlushSubscriptions()
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)
	{
		ACL_subscriptions[i][0]=0;
		ACL_subscriptions[i][1]=0;
	}

	ACL_subscribe_all=false;
	ACLSubscribeDefault();
	return;
}

void ACLSubscribeAll()
{
	ACL_subscribe_all=true;
}

void ACLAnswerOnACM()
{
 	ACL_ACM_answers=true;
}
void ACLNoAnswerOnACM()
{
	ACL_ACM_answers=false;
}






int ACLSubscribeDefault()
{
	int result;

	result=ACLSubscribe(ACL_TYPE_ACM_H,ACL_TYPE_ACM_L);		//165 14 is CMA (Control and Management virtual Artefact)

	return result;
}




int ACLVerifySubscription(int type_h,int type_l)
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)
	{
		if ((ACL_subscriptions[i][0]==type_h) && (ACL_subscriptions[i][1]==type_l)) return 1;
	}


	return 0;		//subscription not found
}





// retun one if there was a control msg // return not needed anymore
//#inline
int ACLProcessControlMessages()
{



	if ((LL_payload_receivebuffer[0]==ACL_TYPE_ACM_H) && (LL_payload_receivebuffer[1]==ACL_TYPE_ACM_L))
	// there was a control msg
	{


 		// remote shutdown: NEEDS THE SOFTWARE-WATCHDOG-DRIVER FOR PIC (only exists for 18fxx20 at this time!!!!)
 		// WDT18FXX20 has to be included before this STACK-File
 		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CRR_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CRR_LO))	// check a control msg, e.g. "1,1"
 		{
 			#define LONGESTWDT 7	//this is the longest wait-time which is accepted from the Software-WDT (2.304sec)
 			unsigned int oldwdtvalue = 0;
 			unsigned int oldwdttest = 0;
 			unsigned int shutdownminutes = 0;
 			unsigned int shutdownseconds = 0;
 			unsigned long allseconds = 0;
 			unsigned long i = 0;
 			unsigned long timercycles = 0;

 			if (ACLMatchesMyIP(LL_payload_receivebuffer+6))
 			{
 				shutdownminutes=LL_payload_receivebuffer[14];
 				shutdownseconds=LL_payload_receivebuffer[15];

 				// check shutdown-time - if the given time is 0, the device is reseted
 				if ( (shutdownminutes==0) && (shutdownseconds==0) )
 				{
 					reset_cpu();
 				}

 				// check shutdown-time - if the given time is both 255, ShutDown the whole Device, only wakes up after hardware-reset
 				if ( (shutdownminutes==255) && (shutdownseconds==255) )
 				{
 					ACLStop();
 					sleep();
 				}

 				// Check ShutDown-Time - time in minutes in higher byte and time in seconds in lower byte
 				if ( ( (shutdownminutes!=0) || (shutdownseconds!=0) ) && ( (shutdownminutes!=255) || (shutdownseconds!=255) ) )
 				{
 					oldwdttest = WDTTest();
 					oldwdtvalue = WDTReadValue();

 					allseconds = shutdownseconds + ( 60*shutdownminutes );
 					timercycles = (allseconds>>1);	//Division by 2, because the longest WDT is 2.304s and you need an integral value

 					WDTConfig(LONGESTWDT);
 					WDTEnable();

 					//PCLedBlueOff();
 					//PCLedRedOff();

 					for (i=0;i<timercycles;i++)
 						{
 							sleep();
 							#asm
 							 nop
 							 nop
 							 nop
 							#endasm
 						}

 					WDTDisable();

 					WDTConfig(oldwdtvalue);

 					if (oldwdttest) {
 						WDTEnable();
 						}
 				}
 			}
 		}

 		// ControlFieldStrength:
 		// give the value to set the fieldstrength
 		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CFS_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CFS_LO))
 		{
 			if (ACLMatchesMyIP(LL_payload_receivebuffer+6))
 			{
 				int fsvalue;
 				fsvalue = LL_payload_receivebuffer[14];
 				ACLSetFieldStrength(fsvalue);
 			}

 		}



#ifdef SYNC_STATE_SYNCED
		// check syncrate setting
		if ((LL_payload_receivebuffer[3]==198) && (LL_payload_receivebuffer[4]==208))
		{
			RFSetSyncRate(LL_payload_receivebuffer[6]);

		}
#endif

		// check initial listen slots setting
		if ((LL_payload_receivebuffer[3]==159) && (LL_payload_receivebuffer[4]==192))
		{
			RFSetInitialListenSlots(LL_payload_receivebuffer[6]);
		}


		// check modeswitch
		if ((LL_payload_receivebuffer[3]==1) && (LL_payload_receivebuffer[4]==1))	// check a control msg, e.g. "1,1"
		{
		}

#ifdef USE_OTAP
		//check over the air programming
		if ((LL_payload_receivebuffer[3]==198) && (LL_payload_receivebuffer[4]==88))	// check a control msg, e.g. "1,1"
		{
				OtapReceive();		// call over the airprogramming stuff
		}


		//check set ID
		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CID_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CID_LO) &&( selftest_active==true))
		{
 			OtapSetID();
  		}
#endif

		//check set ledstyle
		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CLE_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CLE_LO))
		{
			if( ACLMatchesMyIP( LL_payload_receivebuffer+ 12 ))
			{
				AppSetLEDBehaviour( LL_payload_receivebuffer[6] );
				if( LL_payload_receivebuffer[7] ) PCLedRedOn();
				else PCLedRedOff();
				if( LL_payload_receivebuffer[8] ) PCLedBlueOn();
				else PCLedBlueOff();
			}
		}


		//check set production id
		#ifndef __PC210_C__
		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CPI_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CPI_LO))
		{
			if( ACLMatchesMyIP( LL_payload_receivebuffer+ 11 ))
			{
				ACLStop();
				DelayMs( 20 );
				IDChipWriteProductionID( LL_payload_receivebuffer[6], LL_payload_receivebuffer[7] );
				ACLStart();
				DelayMs( 20 );
			}
		}
		#endif

		//////////////////////////////////////////
		if (ACL_ACM_answers==false) return 0;	// no answering allowed; all messages below here are not allowed, if acl_acm_answer is false





		//check Helo
		if ((LL_payload_receivebuffer[3]==115) && (LL_payload_receivebuffer[4]==216))
		{
			if (ACLMatchesMyIP(LL_payload_receivebuffer+6))		// does sent IP match my IP??
			{

				if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff



				// now set the whole packet by hand:
				LL_payload_send[0]=ACL_TYPE_ACM_H;
				LL_payload_send[1]=ACL_TYPE_ACM_L;
				LL_payload_send[2]=0;

				LL_payload_send[3]=134;
				LL_payload_send[4]=32;

				LL_payload_send[5]=8;
				LL_payload_send[6]=LL_header_receivebuffer[3];		// put in orogin adress from HELO
				LL_payload_send[7]=LL_header_receivebuffer[4];
				LL_payload_send[8]=LL_header_receivebuffer[5];
				LL_payload_send[9]=LL_header_receivebuffer[6];
				LL_payload_send[10]=LL_header_receivebuffer[7];
				LL_payload_send[11]=LL_header_receivebuffer[8];
				LL_payload_send[12]=LL_header_receivebuffer[9];
				LL_payload_send[13]=LL_header_receivebuffer[10];
				LL_payload_length=14;								// give the correct len

				if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
				{
					rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
				}


			}

		}
		// this was helo



		return 1;											// there was a control management msg
	}
	return 0;												// there was no control management msg
}

void ACLSetFieldStrength(int power)
{
	LLSetFieldStrength(power);
}

#pragma inline
int ACLSendingBusy()
{
	return LLSendingBusy();
}

#pragma inline
int ACLGetSendSuccess()
{
	return LLGetSendSuccess();
}


#pragma inline
int ACLIdEquals(uint8_t *id1,uint8_t *id2)
{
        int i; 
        for(i=8;i;i--){
	 	if (id1[i-1]!=id2[i-1]) 
	 	return 0;
        }
        return 1;
}

int ACLMatchesMyIP(char *buffer)
{
	int i;

	// check bcast 
        for(i=8;i;i--){
	 	if (buffer[i-1]!=255) break;
        }

	if(i==0)
          return 1; //remove this and i=8 for subnet semantics

	// check if was my address
        for(i=8;i;i--){
	 	if (buffer[i-1]!=ACLGetLocalId()[i-1]) break;
        }

	if (i==0)
          return 1;
        else
          return 0;
}





void ACLAbortSending()
{
	ACL_send_buffer_locked=false;
	ACL_write_position=0;
	ACL_payload_length=0;
	LLAbortSending();
}




// returns 1 if packet was started
// returns 0 if still sending another or connection is off..
int ACLSendPacket(int slot_timeout)
{

	// check if sending busy or layer off

	if (!(rf_status & RF_STATUS_LAYER_ON)) return 0;
	if (rf_status & RF_STATUS_SENDING) return 0;

	// now copy the ACL to the LL payload
	// # copy
	memcpy(LL_payload_send,ACL_payload_send,LL_PAYLOAD_SIZE);	// copies the acl data to ll
	LL_payload_length=ACL_payload_length;
	if (LLSendPacket(slot_timeout))
	{
		// now ACL_payload_send is free again
		ACL_payload_length=0;
		ACL_write_position=0;
		return 1;
	}

	else return 0;

}

int ACLClearSendData()
{
	if (LLSendingBusy()) return 0;					// dont clear if busy sending
	if (ACL_send_buffer_locked==true) return 0;		// dont clear if reservation is active

	//for(i=0;i<LL_PAYLOAD_SIZE;i++)
	//	ACL_payload_send[i]=0;

	memset(ACL_payload_send,0, LL_PAYLOAD_SIZE);	// clear buffer

	ACL_payload_length=0;							// no data in ACL buffer
	ACL_write_position=0;


	return 1;
}




int ACLAddPayload(unsigned char* payload, unsigned char len)
{
	if (ACL_send_buffer_locked==true) return 2;		// dont set if reservation is active
	if ((LL_PAYLOAD_SIZE-ACL_payload_length)<len)
	{
		return 1;		// not enough space
	}
	else
	{
		unsigned char i;
		unsigned char last;

		memcpy(&ACL_payload_send[ACL_payload_length],payload,len);

		last=ACL_payload_length+2;

		for(i=0;i<len;i+=((unsigned char) payload[i+2]+3))
		{
			ACL_write_position=last;
			last+=payload[i+2];
		}

		//place the new type
		ACL_payload_length+=len;
		return 0;
	}
}


// return 0 if ok
// returns 1 if full
// returns 2 if locked
int ACLAddNewType(int type_h, int type_l)
{
	if (ACL_send_buffer_locked==true) return 2;		// dont set if reservation is active
	if ((LL_PAYLOAD_SIZE-ACL_payload_length)<3)
	{
		return 1;		// not enough space
	}
	else
	{
		//place the new type
		ACL_payload_send[ACL_payload_length]=type_h;
		ACL_payload_send[ACL_payload_length+1]=type_l;
		ACL_payload_send[ACL_payload_length+2]=0;	//data length is zero
		ACL_write_position=ACL_payload_length+2;
		ACL_payload_length+=3;
		return 0;
	}
}

int ACLAddDataN(char *data, len)
{
	if (ACL_send_buffer_locked==true) return 2;		// dont set if reservation is active
	if ((LL_PAYLOAD_SIZE-ACL_payload_length)<len)
	{
		return 1;		// not enough space
	}
	else
	{
		memcpy(&ACL_payload_send[ACL_payload_length],data,len);		//place the data
		ACL_payload_send[ACL_write_position]+=len;			//count the acl tuple len up
		ACL_payload_length+=len;
		return 0;
	}
}


int ACLAddData(int data)
{
	if (ACL_send_buffer_locked==true) return 2;		// dont set if reservation is active
	if ((LL_PAYLOAD_SIZE-ACL_payload_length)<1)
	{
		return 1;		// not enough space
	}
	else
	{
		ACL_payload_send[ACL_write_position]++;			//count the acl tuple len up
		ACL_payload_send[ACL_payload_length]=data;		//place the data
		ACL_payload_length+=1;
		return 0;
	}

}

int ACLGetRemainingPayloadSpace()
{
	//return (LL_payload_length-ACL_payload_length);
	return LL_PAYLOAD_SIZE-ACL_payload_length;
}


char* ACLGetSrcId()
{
	return	(LL_header_received+3);
}

char* ACLGetLocalId()
{
  return &LL_header_send[3];
}

int ACLGetReceivedFieldstrength()
{
	return	LL_header_received[2];
}


int ACLGetReceivedPayloadLength()
{
	return LL_header_received[1];
}


char* ACLGetSubject()
{
  return (&(LL_payload_received[0]));
}

int ACLGetDataLength(char *data)
{
 return *(data-1);
}


char ACLGetTypeHi(char *data)
{
 return *(data-3);
}

char ACLGetTypeLo(char *data)
{
 return *(data-2);
}

uint16_t ACLGetType(char *data)
{
    return  ((uint16_t)ACLGetTypeHi(data))<<8 | ACLGetTypeLo(data) ;
}


char* ACLGetNextTupleData(char *data)
{
  data=data+(ACLGetDataLength(data)+3);

  if(data>ACLGetSubject()+ACLGetReceivedPayloadLength())
    data=0;

  return data;
}

#define ACLGetTupleData(X) &X[3]
#define ACLGetTupleLength(X) X[2]


char* ACLGetReceivedSubjectData()
{
  char* ret=LL_payload_received;
  return (ret+3);
}

//looks for the length of context data of the given type, return2 -1 if type wasn't found
char* ACLGetReceivedData(int type_h, int type_l)
{
  int i;
  for (i=0; i<ACLGetReceivedPayloadLength();i+=3+LL_payload_received[i+2])
  {
    if ((LL_payload_received[i]==type_h) && (LL_payload_received[i+1]==type_l))
    {
      return (&(LL_payload_received[i+3]));
    }
  }

  return 0;
}

signed int ACLGetReceivedDataLength(int type_h, int type_l)
{
  char *data;
  data=ACLGetReceivedData(type_h,type_l);
  if(data) return *(data-1);
  else return -1;
}

char ACLGetReceivedByte(int type_h, int type_l, int position)
{
  char *data;
  data=ACLGetReceivedData(type_h,type_l);
  return data[position];
}

#pragma inline
int ACLFoundReceivedType(int type_h, int type_l)
{
  return (0!=ACLGetReceivedData(type_h,type_l));
}



void ACLSetControlMessagesBehaviour(boolean ignore, boolean pass)
{
  ACL_ignore_control_messages=ignore;		//all control messages are ignored
  ACL_pass_control_messages=pass;			// all control messages are as well copied to receivebuffer for applicaiton
}



int ACLSentPacketInThisSlot()
{
  return (LLSentPacketInThisSlot());
}


void ACLStart()
{
  LLStart();
}

void ACLStop()
{
  LLStop();
}


void ACLLockReceiveBuffer()
{
  LLLockReceiveBuffer();
}

int ACLReceiveBufferLocked()
{
  return LLReceiveBufferLocked();
}


void ACLReleaseReceiveBuffer()
{
  LLReleaseReceiveBuffer();
}


void ACLSetDataToOld()
{
	LLSetDataToOld();
}

void ACLSetDataToNew()
{
	LLSetDataToNew();
}


int ACLAnyDataIsNew()
{
	return LLDataIsNew();
}

int ACLDataReceivedInThisSlot()
{
	if (LL_received_packet_in_this_slot==true) return 1;
	return 0;
}

int ACLAnyDataIsNewNow()
{
	return ACLDataReceivedInThisSlot();
}

int ACLXAddressedDataIsAddressed()
{
  return ACLFoundReceivedType(ACL_TYPE_CAD_HI,ACL_TYPE_CAD_LO);
}

int ACLAddressedDataIsNew()
{
        char *cad;
	if (!ACLAnyDataIsNew()) return 0;

        cad=ACLGetReceivedData(ACL_TYPE_CAD_HI,ACL_TYPE_CAD_LO);
	if ( cad )
	{
		if (ACLMatchesMyIP( cad )) return 1;
                else return 0;
	}
        else return 1;
}

int ACLAddressedDataIsNewNow()
{
	if (!ACLAnyDataIsNewNow()) return 0;
        else return ACLAddressedDataIsNew();
}


//sends out the current packet with a target ID
int ACLSendPacketAddressed(unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout )
{
	if (ACLGetRemainingPayloadSpace()<11) return 0;		//not enough space

	ACLAddNewType(ACL_TYPE_CAD_HI,ACL_TYPE_CAD_LO);
	ACLAddData(add1);
	ACLAddData(add2);
	ACLAddData(add3);
	ACLAddData(add4);
	ACLAddData(add5);
	ACLAddData(add6);
	ACLAddData(add7);
	ACLAddData(add8);

	if (ACLSendPacket(timeout)) return 1;
	return 0;
}

void ACLWaitForReady()
{
	while (ACLSendingBusy()) ;
}
