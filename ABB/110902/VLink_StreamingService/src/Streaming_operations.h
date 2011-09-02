
#ifndef STREAMING_OPERATIONS_H
#define STREAMING_OPERATIONS_H

#define SRV_Streaming 0

enum Streaming_operations {
	OP_VLinkModel_Streaming_ValuesEvent,
	OP_VLinkModel_Streaming_StartLDC
};

#define	ACLERR_NotReady -1
#define ACLERR_GW_Busy -2

#endif //STREAMING_OPERATIONS_H
