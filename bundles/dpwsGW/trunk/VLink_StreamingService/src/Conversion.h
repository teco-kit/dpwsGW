
#ifndef CONVERSION_H_
#define CONVERSION_H_

#include "Streaming_operations.h"

/**
 * Write SOAP output
 */
void writeSOAPBuf(struct soap *soap,char * buf);

/**
 * Write values to SOAP output
 */
void writeSOAPValues(struct soap *soap,ldc_message * msg);

/**
 * Read LDC info from SOAP
 */
int readLDCInfo(struct soap *soap,LDC_info * info);

#endif /* CONVERSION_H_ */
