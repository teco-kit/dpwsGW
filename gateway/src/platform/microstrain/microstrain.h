#ifndef MICROSTRAIN_H_
#define MICROSTRAIN_H_

enum MS_MSGRESPONSETYPE {
	MS_NONE,
	MS_STREAM,
	MS_HSS,
	MS_LDC,
	MS_LONGPING,
	MS_STOP,
	MS_SHORTPING,
	MS_READVALUE,
	MS_DATAPAGE,
	MS_WRITEEEPROM,
	MS_ERASE,
	MS_TRIGGERLOGGING,
	MS_DISCOVERY,
	MS_BS,
	MS_BSPING,
	MS_FAILURE
};

enum MS_DEVICESTATE {
	DEVICE_IDLE,
	DEVICE_LDC,
	DEVICE_LOGGING,
	DEVICE_SENDING
};

enum MS_GATEWAYSTATE {
	GATEWAY_IDLE,
	GATEWAY_LDC,
	GATEWAY_RECEIVING
};

typedef struct {
	// Message type
	int type;
	// Time received
	struct timeval received;
	// Payload length - without header
	int length;
	// Message payload, maximum is 266 (Download page is 267 with 1 byte header)
	unsigned char buffer[266];
} msmessage;



typedef struct {
	// Current page
	unsigned short page;
	// Session index
	unsigned short index;
	// Channel mask
	unsigned char channelmask;
	// Data rate
	float rate;
	// Last page
	msmessage lastmsg;
	// Last values wrap, contains number of bytes on last page
	int wrap;
	// First part of wrapped value
	unsigned char wrappedval[16];
	// Number of samples
	int samplecount;

} mssession;

typedef struct {
	// Last message
	float lastvalue[8];
	// Channel action IDs
	unsigned char channelaction[8];
	// Gain
	float gain[8];
	// Offset
	float offset[8];
	// Node ID
	unsigned short id;
	// MicroStrain model number
	unsigned short modelnumber;
	// Active channel mask
	unsigned char channelmask;
	// Last updated
	struct timeval updated;
	// LDC Sampling period (1/frequency) in s
	float ldcrate;
	// Last timer tick
	unsigned short lasttick;
	// State
	unsigned short state;
	// Session information
	mssession session;

} msdevice;

/**
 * Open serial connection
 */
int openSerial();

/**
 * Close serial connection
 */
int closeSerial();

/**
 * Check if a message type is in queue
 */
int inMessageQueue(int type);

/**
 * Read next message
 */
int readMessage(int expectmultiple);

/**
 * Get a message from the queue
 *
 * Uses dest to allow static buffer
 */
int getMessageFromQueue(int type, msmessage * dest);

/**
 * Process Discovery message
 */
void processDiscovery(msmessage * msg);

/**
 * Process Low Duty Cycle message
 */
void processLDC(msmessage * msg);

/**
 * Register a new node
 */
void registerNode(unsigned short id);

/**
 * Ping Node to check for existence
 */
int pingNode(unsigned short id);

/**
 * Fetch and update node information
 */
void updateNodeInfo(msdevice * node);

/**
 * Request a datum from the node EEPROM
 */
void requestEEPROM(unsigned short node,unsigned short eepromaddr);

/**
 * Initiate LDC
 */
void initLDC(unsigned short node,unsigned short rateindex, unsigned short samples);

/**
 * Stop a node
 */
int stopNode(unsigned short node);

/**
 * Convert a MS_MSGRESPONSETYPE to a string
 */
char * msmessagestr(int type);

/**
 * Translate LDC rate number into a period
 */
float getLDCPeriod(unsigned short index);

/**
 * Get the rate number belonging to the LDC rate
 */
unsigned short getLDCRateIndex(char * rate);

/**
 * Get the rate number belonging to the Logging rate
 */
unsigned short getLoggingRateIndex(char * rate);

/**
 * Get the number of saved sessions on a node
 */
int getSavedSessions(unsigned short node);

/**
 * Init logging on a node
 */
void initLogging(unsigned short node,unsigned short rateindex, unsigned short samplecount);

/**
 * Init download from node
 */
void initDownload(unsigned short node);

/**
 * Erase sessions on a node
 */
void eraseSessions(unsigned short node);

/**
 * Request a page from the node memory
 */
void requestPage(unsigned short node, unsigned short page);

/**
 * Process a data page.
 * Returns 1 if end is not reached, 0 otherwise
 */
int processLogging(msmessage * msg);

/**
 * Translate logging rate into a period
 */
float getLoggingPeriod(unsigned short index);

/**
 * Return a string describing the device state
 */
const char * getDeviceInfoString(int state);

/**
 * Returns the samples * 100 in the given period for the sampling rate
 */
unsigned short getLoggingSampleCount(unsigned short rateindex,char * duration);

/**
 * Returns the samples * 100 in the given period for the sampling rate
 */
unsigned short getLDCSampleCount(unsigned short rateindex,char * duration);

/**
 * Update GW state by checking all nodes
 */
void updateGWState();

#endif /* MICROSTRAIN */
