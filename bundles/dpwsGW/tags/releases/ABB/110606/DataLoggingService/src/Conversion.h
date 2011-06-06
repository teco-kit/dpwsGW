
#ifndef CONVERSION_H_
#define CONVERSION_H_

/**
 * Write Event Info to SOAP output
 */
void writeEvent(struct soap *soap,char * buf,int len);

/**
 * Read Logging info from SOAP
 */
int readLoggingInfo(struct soap *soap, LoggingInfo * info);

/**
 * Write session count to SOAP output
 */
void writeSessionCount(struct soap *soap,sessioninfo * info);

#endif /* CONVERSION_H_ */
