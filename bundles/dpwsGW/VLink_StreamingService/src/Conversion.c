

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <limits.h>
#include <errno.h>
#include <stdsoap2.h>
#include <microstrain_struct.h>
#include "Streaming_operations.h"
#include "Conversion.h"

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string 0
#endif

void writeSOAPBuf(struct soap *soap,char * buf)
{
	ldc_message * msg = (ldc_message*) buf;
	writeSOAPValues(soap,msg);
}

void writeSOAPValues(struct soap *soap,ldc_message * msg)
{
	printf("Calling writeSOAPValues\n");
	char * str = NULL;

	soap_element_begin_out(soap, "vlstr:series", 0, "");

	asprintf(&str, "%d", (int)msg->timertick);
	char * count_str = soap_strdup(soap, str);
	soap_outstring(soap, "vlstr:count", -1, &count_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	if(msg->timertick<=1)
	{
		struct timeval t;
		struct tm res;
		char out[64];
		gettimeofday(&t,NULL);
		strftime(out, sizeof(out), "%Y-%m-%dT%H:%M:%S",localtime_r(&t.tv_sec, &res));
		asprintf(&str, "%s.%06uZ", out, (uint32_t)((((uint64_t) t.tv_usec) * 1000000) >> 16));
		char * TimeStamp_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:timestamp", -1, &TimeStamp_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	asprintf(&str, "PT%fS", msg->delta);
	char * delta_str = soap_strdup(soap, str);
	soap_outstring(soap, "vlstr:delta", -1, &delta_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	soap_element_begin_out(soap, "vlstr:sample", 0, "");

	if((msg->channelmask >> 0) & 1)
	{
		asprintf(&str, "%f", msg->values[0]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:differential1", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	if((msg->channelmask >> 1) & 1)
	{
		asprintf(&str, "%f", msg->values[1]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:differential2", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}
	if((msg->channelmask >> 2) & 1)
	{
		asprintf(&str, "%f", msg->values[2]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:differential3", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	if((msg->channelmask >> 3) & 1)
	{
		asprintf(&str, "%f", msg->values[3]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:differential4", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	if((msg->channelmask >> 4) & 1)
	{
		asprintf(&str, "%f", msg->values[4]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:single1", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	if((msg->channelmask >> 5) & 1)
	{
		asprintf(&str, "%f", msg->values[5]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:single2", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	if((msg->channelmask >> 6) & 1)
	{
		asprintf(&str, "%f", msg->values[6]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:single3", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	if((msg->channelmask >> 7) & 1)
	{
		asprintf(&str, "%f", msg->values[7]);
		char * val_str = soap_strdup(soap, str);
		soap_outstring(soap, "vlstr:temperature", -1, &val_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
	}

	soap_element_end_out(soap, "vlstr:sample");
	soap_element_end_out(soap, "vlstr:series");
}

int readLDCInfo(struct soap *soap, LDC_info * info)
{
	printf("Calling readLDCInfo\n");
	if (soap_element_begin_in(soap, "vlstr:ldcinfo", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: ldcinfo expected",NULL);
		return 0;
	}
	if (soap_element_begin_in(soap, "vlstr:rate", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: rate expected",NULL);
		return 0;
	}

	{
		const char* str = soap_value(soap);
		strcpy(info->rate,str);
	}

	if (soap_element_end_in(soap, "vlstr:rate") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: rate expected",NULL);
		return 0;
	}

	if (soap_element_begin_in(soap, "vlstr:duration", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: duration expected",NULL);
		return 0;
	}

	{
		const char* str = soap_value(soap);
		strcpy(info->duration,str);
	}

	if (soap_element_end_in(soap, "vlstr:duration") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: duration expected",NULL);
		return 0;
	}

	if (soap_element_end_in(soap, "vlstr:ldcinfo") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: ldcinfo expected",NULL);
		return 0;
	}
	return 1;
}

