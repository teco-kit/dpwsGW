
#ifndef ACCELERATION_OPERATIONS_H
#define ACCELERATION_OPERATIONS_H

#define SRV_Acceleration 0

enum Acceleration_operations {
	OP_GetAccelerationValuesEvent,
	OP_StartLDC
};

typedef struct {
	float values[8];
	float timertick;
	float delta;
} ldcmessage;

typedef struct {
	char rate[64];
	char duration[64];
} LDCInfo;

#define	ACLERR_NotReady -1
#define ACLERR_GW_Busy -2

#endif //ACCELERATION_OPERATIONS_H
