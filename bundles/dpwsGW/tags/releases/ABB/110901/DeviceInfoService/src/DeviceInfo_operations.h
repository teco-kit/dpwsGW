
#ifndef DEVICEINFO_OPERATIONS_H
#define DEVICEINFO_OPERATIONS_H

#define SRV_DeviceInfo 1

enum DeviceInfo_operations {
	OP_DeviceInfo_GetDeviceInfo,
	OP_DeviceInfo_StopDevice
};

typedef struct {
	char status[256];
} DeviceInfo;
#endif //DEVICEINFO_OPERATIONS_H
