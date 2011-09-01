
#ifndef CONVERSION_H_
#define CONVERSION_H_

/**
 * Write Device Info to SOAP output
 */
void writeDeviceInfo(struct soap *soap,device_info * info);

/**
 * Write StopDevice result to SOAP output
 */
void writeStopDevice(struct soap *soap,char * buf,int len);



#endif /* CONVERSION_H_ */
