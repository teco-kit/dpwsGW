

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <limits.h>
#include <errno.h>
#include <stdsoap2.h>
#include <microstrain_struct.h>
#include "Acceleration_operations.h"
#include "Conversion.h"

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string 0
#endif

void writeSOAPBuf(struct soap *soap,char * buf)
{
	ldc_message * msg = (ldc_message*) buf;
	writeSOAPValues(soap,msg->values[0],msg->values[1],msg->values[2],msg->timertick,msg->delta);
}

void writeSOAPValues(struct soap *soap,float x,float y,float z,float tick, float delta)
{
	printf("Calling writeSOAPValues\n");
	char * str = NULL;

	soap_element_begin_out(soap, "acs:series", 0, "");

	asprintf(&str, "%d", (int)tick);
	char * count_str = soap_strdup(soap, str);
	soap_outstring(soap, "acs:count", -1, &count_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	if(tick<=1)
	{
		struct timeval t;
		struct tm res;
		char out[64];
		gettimeofday(&t,NULL);
		strftime(out, sizeof(out), "%Y-%m-%dT%H:%M:%S",localtime_r(&t.tv_sec, &res));
		asprintf(&str, "%s.%06uZ", out, (uint32_t)((((uint64_t) t.tv_usec) * 1000000) >> 16));
		char * TimeStamp_str = soap_strdup(soap, str);
		soap_outstring(soap, "acs:timestamp", -1, &TimeStamp_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	asprintf(&str, "PT%fS", delta);
	char * delta_str = soap_strdup(soap, str);
	soap_outstring(soap, "acs:delta", -1, &delta_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	soap_element_begin_out(soap, "acs:sample", 0, "");

	soap_element_begin_out(soap, "acs:accl", 0, "");
	asprintf(&str, "%f", x);
	char * x_str = soap_strdup(soap, str);
	soap_outstring(soap, "acs:x", -1, &x_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	asprintf(&str, "%f", y);
	char * y_str = soap_strdup(soap, str);
	soap_outstring(soap, "acs:y", -1, &y_str, "", SOAP_TYPE_string);
	free(str);

	asprintf(&str, "%f", z);
	char * z_str = soap_strdup(soap, str);
	soap_outstring(soap, "acs:z", -1, &z_str, "", SOAP_TYPE_string);
	free(str);

	soap_element_end_out(soap, "acs:accl");
	soap_element_end_out(soap, "acs:sample");
	soap_element_end_out(soap, "acs:series");
}

int readLDCInfo(struct soap *soap, LDC_info * info)
{
	printf("Calling readLDCInfo\n");
	if (soap_element_begin_in(soap, "acs:ldcinfo", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: ldcinfo expected",NULL);
		return 0;
	}
	if (soap_element_begin_in(soap, "acs:rate", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: rate expected",NULL);
		return 0;
	}

	{
	const char* str = soap_value(soap);
	strcpy(info->rate,str);
	}

	if (soap_element_end_in(soap, "acs:rate") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: rate expected",NULL);
		return 0;
	}

	if (soap_element_begin_in(soap, "acs:duration", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: rate expected",NULL);
		return 0;
	}

	{
	const char* str = soap_value(soap);
	strcpy(info->duration,str);
	}

	if (soap_element_end_in(soap, "acs:duration") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: rate expected",NULL);
		return 0;
	}

	if (soap_element_end_in(soap, "acs:ldcinfo") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: ldcinfo expected",NULL);
		return 0;
	}
	return 1;
}

