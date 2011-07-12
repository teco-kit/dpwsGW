
#ifndef CONVERSION_H_
#define CONVERSION_H_

#include "Acceleration_operations.h"

/**
 * Write SOAP output
 */
void writeSOAPBuf(struct soap *soap,char * buf);

/**
 * Write acceleration values to SOAP output
 */
void writeSOAPValues(struct soap *soap,float x,float y,float z,float tick, float delta);

/**
 * Read Logging info from SOAP
 */
int readLDCInfo(struct soap *soap,LDCInfo * info);

#endif /* CONVERSION_H_ */
