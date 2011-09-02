
#ifndef ACCELERATION_OPERATIONS_H
#define ACCELERATION_OPERATIONS_H

#define SRV_Streaming 0

enum Acceleration_operations {
	OP_AccelModel_Streaming_ValuesEvent,
	OP_AccelModel_Streaming_StartLDC
};

#define	ACLERR_NotReady -1
#define ACLERR_GW_Busy -2

#endif //ACCELERATION_OPERATIONS_H
