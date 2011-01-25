
#include <stdint.h>

//ACL
int ACLSubscribe(byte LL_type_h,LL_type_l);				//adds a type to the subscription list
int ACLUnsubscribe(byte LL_type_h,LL_type_l);			//deletes a type out of the subscription list
void ACLFlushSubscriptions();							//deletes all subscription (not the default ones)
int ACLSubscribeDefault();								//subscribes to the default types (control msgs..)
int ACLVerifySubscription(int type_h,int type_l);		//checks if a subscription is there
void ACLInit();											//start ACL and lower layers, resets the whole stack
//#inline
int ACLProcessControlMessages();						//internal: is called if control msg is there
void ACLSetFieldStrength(int power);					//sets the field strength of transmitter signals
#pragma inline
int ACLSendingBusy();									//returns true if LL has Packet in send queue
#pragma inline
int ACLGetSendSuccess();								//returns the result of last Packet transmission
int ACLSendPacket(int slot_timeout);					//starts packet transmission, return values!
int ACLAddNewType(int type_h, int type_l);				//adds a type into the ACL send buffer, return values!
int ACLAddData(int data);								//adds one byte data into the ACL send buffer, return values!
int ACLAddDataN(char *data,int len);								//adds one byte data into the ACL send buffer, return values!
void ACLAbortSending();									//stops a running transmission
void ACLSubscribeAll();									//subscribes to any possible type (all packets are received)
int ACLMatchesMyIP(char *buffer);				//checks, if buffer holds my IP

#pragma inline
int ACLIdEquals( uint8_t *id1, uint8_t *id2 );

char* ACLGetLocalId();
char* ACLGetReceivedData(int type_h, int type_l);		//returns the ACL payload data of the given type in the last received packet
int ACLGetReceivedPayloadLength();						//returns the number of bytes received
void ACLSetControlMessagesBehaviour(boolean ignore, boolean pass);	//if ignore==true: don't react on any control msg; if pass==true: give control msgs to ACL as received payload packets
signed int ACLGetReceivedDataLength(int type_h, int type_l);	//returns number of bytes received of given type in last msg
int ACLSentPacketInThisSlot();							//is true until next slot if msg was send in this slot
void ACLAnswerOnACM();									//if a control msgs comes in that requires an answer, it is answered
void ACLNoAnswerOnACM();									//if a control msgs comes in that requires an answer, it will not be answered
int ACLClearSendData();									//deletes the send buffer of ACL
void ACLStart();										//restart ACL after ACLstop()
void ACLStop();											//stops the RF stack. Everything is hold, continues after ACLstart. msg stay in queue
int ACLGetRemainingPayloadSpace();						//returns the number of free bytes in the transmit buffer
void ACLLockReceiveBuffer();							//locks the receive buffer: no new msgs are received
int ACLReceiveBufferLocked();							//returns true if receivebuffer is locked
void ACLReleaseReceiveBuffer();							//un-lock the receivebuffer;
void ACLSetDataToOld();									//set received data to "old": means ACLDataIsNew will not return true unless a new packet was received
void ACLSetDataToNew();									//set received data to "new": means ACLDataIsNew will return true every slot
int ACLAnyDataIsNewNow();										//returns true if data in receive buffer is "new" now
int ACLAnyDataIsNew();										//returns true if data in receive buffer is "new"
char ACLGetReceivedByte(int type_h, int type_l, int position);		//returns a single received byte on the position
void ACLStartup();										// is the first function in any case,  runs selftest and aclinit
int ACLDataReceivedInThisSlot();						//returns true if data was received in this slot

#pragma inline
int ACLFoundReceivedType(int type_h, int type_l);		//returns true if type was found
int ACLAddressedDataIsNew();								// return1 if adressed data came
int ACLAddressedDataIsNewNow();							// return 1 if adressed data came in this slot
int ACLSendPacketAddressed(unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout );//sends out the current packet with a target ID
void ACLWaitForReady();									// waits until ACL has finished all jobs like packet send etc.

#define ACL_ADDRESS_ARG(addr) (addr)[0],(addr)[1],(addr)[2],(addr)[3],(addr)[4],(addr)[5],(addr)[6],(addr)[7]
