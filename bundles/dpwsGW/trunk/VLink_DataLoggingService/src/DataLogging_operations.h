
#ifndef DATALOGGING_OPERATIONS_H
#define DATALOGGING_OPERATIONS_H

#define SRV_DataLogging 2

enum DataLogging_operations {
	OP_VLinkModel_DataLogging_DataEvent,
	OP_VLinkModel_DataLogging_StartLogging,
	OP_VLinkModel_DataLogging_StartDownload,
	OP_VLinkModel_DataLogging_Erase,
	OP_VLinkModel_DataLogging_GetSessionCount
};


#define	DLERR_NotReady -1
#define DLERR_NoSessions -2
#define	DLERR_NotCleared -3
#define DLERR_GW_Busy -4


#endif //DATALOGGING_OPERATIONS_H
