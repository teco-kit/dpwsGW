enum ll_data_in_last_slot{LL_CRC_ERROR,LL_ACM,LL_GOOD,LL_NONE};


// LL
#define LL_HEADER_SIZE			12
#define LL_PAYLOAD_SIZE			64//64
#define LL_TAIL_SIZE			2
#define LL_FRAME_MAX_SIZE		(LL_HEADER_SIZE+LL_PAYLOAD_SIZE+LL_TAIL_SIZE)

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
// LL Layer
void LLStart();
void LLStop();
int LLSendPacket(int slot_limit);
int LLSendingBusy();
int LLGetSendSuccess();
int LLIsActive();
void LLLockReceiveBuffer();
void LLReleaseReceiveBuffer();
int LLReceiveBufferLocked();
void LLSlotEnd();
void LLSetDataToOld();
void LLSetDataToNew();
void LLInit();
int  LLGetFieldStrength();
void LLSetFieldStrength(int value);
long LLCalcCRC16(byte *header_data,byte *payload_data,byte payload_size);//,int payload_size);
void LLAbortSending();
int  LLGetRemainingPayloadSpace();
void LLSetSendingSuccess();
int LLDataIsNew();
int LLSentPacketInThisSlot();
int LLGetIDFromHardware();								// takes id either from internal or external eeprom


