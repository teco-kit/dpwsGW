
#ifndef DATALOGGING_OPERATIONS_H
#define DATALOGGING_OPERATIONS_H

#define SRV_DataLogging 2

enum DataLogging_operations {
	OP_DataLogging_DataEvent,
	OP_DataLogging_StartLogging,
	OP_DataLogging_StartDownload,
	OP_DataLogging_Erase,
	OP_DataLogging_GetSessionCount
};

typedef struct {
	char rate[64];
	char duration[64];
} LoggingInfo;

typedef struct {
	float values[8];
} loggingsample;

typedef struct {
	unsigned char channelmask;
	unsigned char channels;
	unsigned char samplecount;
	float rate;
	int prevsamplecount;
	loggingsample samples[132];
} loggingpage;

typedef struct {
	int count;
} sessioninfo;


#define	DLERR_NotReady -1
#define DLERR_NoSessions -2
#define	DLERR_NotCleared -3
#define DLERR_GW_Busy -4


#endif //DATALOGGING_OPERATIONS_H
