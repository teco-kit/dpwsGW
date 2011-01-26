#ifndef MICROSTRAIN_H_
#define MICROSTRAIN_H_

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
} msdevice;

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
	MS_FAILURE
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
void initLDC(unsigned short node);

/**
 * Stop a node
 */
int stopNode(unsigned short node);

/**
 * Convert a MS_MSGRESPONSETYPE to a string
 */
char * msmessagestr(int type);

/**
 * Translate LDC rate number into frequency
 */
float getLDCRate(unsigned short index);

#endif /* MICROSTRAIN */
