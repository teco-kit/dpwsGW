

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <limits.h>
#include <errno.h>
#include <stdsoap2.h>
#include <microstrain_struct.h>
#include "DeviceInfo_operations.h"
#include "Conversion.h"

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string 0
#endif

void writeDeviceInfo(struct soap *soap,device_info * info)
{
	soap_element_begin_out(soap, "vldvcinf:status", 0, "");

	char * status_str = soap_strdup(soap, info->status);
	soap_outstring(soap, "vldvcinf:description", -1, &status_str, "", SOAP_TYPE_string);
	if(strcmp("Idle",info->status)==0)
	{
		char * ready_str = soap_strdup(soap, "true");
		soap_outstring(soap, "vldvcinf:ready", -1, &ready_str, "", SOAP_TYPE_string);
	} else {
		char * ready_str = soap_strdup(soap, "false");
		soap_outstring(soap, "vldvcinf:ready", -1, &ready_str, "", SOAP_TYPE_string);
	}


	soap_element_end_out(soap, "vldvcinf:status");
}

void writeStopDevice(struct soap *soap,char * buf,int len)
{
	soap_element_begin_out(soap, "vldvcinf:stopinfo", 0, "");

	char * status_str = soap_strdup(soap, buf);
	soap_outstring(soap, "vldvcinf:status", -1, &status_str, "", SOAP_TYPE_string);

	soap_element_end_out(soap, "vldvcinf:stopinfo");
}
