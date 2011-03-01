

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <limits.h>
#include <errno.h>
#include <stdsoap2.h>

#include "DataLogging_operations.h"
#include "Conversion.h"

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string 0
#endif

void writeEvent(struct soap *soap,char * buf,int len)
{
	printf("Calling writeEvent\n");
	loggingpage * page = (loggingpage*) buf;
	char * str = NULL;

	soap_element_begin_out(soap, "logging:series", 0, "");

	asprintf(&str, "%d", page->samplecount);
	char * count_str = soap_strdup(soap, str);
	soap_outstring(soap, "logging:count", -1, &count_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	float delta = page->prevsamplecount * (1/(float)page->rate);
	asprintf(&str, "PT%fS", delta);
	char * delta_str = soap_strdup(soap, str);
	soap_outstring(soap, "logging:delta", -1, &delta_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	int i;
	for(i=0;i<page->samplecount;i++)
	{
		soap_element_begin_out(soap, "logging:sample", 0, "");
		delta = i * (1/(float)page->rate);
		asprintf(&str, "PT%fS", delta);
		char * sampledelta_str = soap_strdup(soap, str);
		soap_outstring(soap, "logging:delta", -1, &sampledelta_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;
		soap_element_begin_out(soap, "logging:accl", 0, "");
		asprintf(&str, "%f", page->samples[i].values[0]);
		char * x_str = soap_strdup(soap, str);
		soap_outstring(soap, "logging:x", -1, &x_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;

		asprintf(&str, "%f", page->samples[i].values[1]);
		char * y_str = soap_strdup(soap, str);
		soap_outstring(soap, "logging:y", -1, &y_str, "", SOAP_TYPE_string);
		free(str);

		asprintf(&str, "%f", page->samples[i].values[2]);
		char * z_str = soap_strdup(soap, str);
		soap_outstring(soap, "logging:z", -1, &z_str, "", SOAP_TYPE_string);
		free(str);

		soap_element_end_out(soap, "logging:accl");
		soap_element_end_out(soap, "logging:sample");
	}
	soap_element_end_out(soap, "logging:series");
}

int readLoggingInfo(struct soap *soap, LoggingInfo * info)
{
	printf("Calling readLoggingInfo\n");

	if (soap_element_begin_in(soap, "logging:loggingconfig", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: start of loggingconfig expected",NULL);
		return 0;
	}
	if (soap_element_begin_in(soap, "logging:rate", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: start of rate expected",NULL);
		return 0;
	}

	const char* str = soap_value(soap);
	strcpy(info->rate,str);

	if (soap_element_end_in(soap, "logging:rate") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: end of rate expected",NULL);
		return 0;
	}

	if (soap_element_begin_in(soap, "logging:duration", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: start of duration expected",NULL);
		return 0;
	}

	str = soap_value(soap);
	strcpy(info->duration,str);

	if (soap_element_end_in(soap, "logging:duration") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: end of duration expected",NULL);
		return 0;
	}

	if (soap_element_end_in(soap, "logging:loggingconfig") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: end of loggingconfig expected",NULL);
		return 0;
	}
	return 1;
}

void writeSessionCount(struct soap *soap,sessioninfo * info)
{
	printf("Calling writeSessionCount\n");
	soap_element_begin_out(soap, "logging:sessioninfo", 0, "");

	char * str = NULL;
	asprintf(&str, "%d", info->count);
	char * count_str = soap_strdup(soap, str);
	soap_outstring(soap, "logging:count", -1, &count_str, "", SOAP_TYPE_string);
	free(str);

	soap_element_end_out(soap, "logging:sessioninfo");
}
