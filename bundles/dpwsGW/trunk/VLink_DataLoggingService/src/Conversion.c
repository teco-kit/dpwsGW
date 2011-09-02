

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <limits.h>
#include <errno.h>
#include <stdsoap2.h>

#include <microstrain_struct.h>
#include "DataLogging_operations.h"
#include "Conversion.h"

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string 0
#endif

void writeEvent(struct soap *soap,char * buf,int len)
{
	printf("Calling writeEvent\n");
	logging_page * page = (logging_page*) buf;
	char * str = NULL;

	soap_element_begin_out(soap, "vllogging:series", 0, "");

	asprintf(&str, "%d", page->samplecount);
	char * count_str = soap_strdup(soap, str);
	soap_outstring(soap, "vllogging:count", -1, &count_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	float delta = page->prevsamplecount * (1/(float)page->rate);
	asprintf(&str, "PT%fS", delta);
	char * delta_str = soap_strdup(soap, str);
	soap_outstring(soap, "vllogging:delta", -1, &delta_str, "", SOAP_TYPE_string);
	free(str);
	str = NULL;

	int i;
	for(i=0;i<page->samplecount;i++)
	{
		soap_element_begin_out(soap, "vllogging:sample", 0, "");
		delta = i * (1/(float)page->rate);
		asprintf(&str, "PT%fS", delta);
		char * sampledelta_str = soap_strdup(soap, str);
		soap_outstring(soap, "vllogging:delta", -1, &sampledelta_str, "", SOAP_TYPE_string);
		free(str);
		str = NULL;

		if((page->channelmask >> 0) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[0]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:differential1", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}

		if((page->channelmask >> 1) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[1]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:differential2", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}
		if((page->channelmask >> 2) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[2]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:differential3", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}

		if((page->channelmask >> 3) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[3]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:differential4", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}

		if((page->channelmask >> 4) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[4]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:single1", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}

		if((page->channelmask >> 5) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[5]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:single2", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}

		if((page->channelmask >> 6) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[6]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:single3", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}

		if((page->channelmask >> 7) & 1)
		{
			asprintf(&str, "%f", page->samples[i].values[7]);
			char * val_str = soap_strdup(soap, str);
			soap_outstring(soap, "vllogging:temperature", -1, &val_str, "", SOAP_TYPE_string);
			free(str);
			str = NULL;
		}
		soap_element_end_out(soap, "vllogging:sample");
	}
	soap_element_end_out(soap, "vllogging:series");
}

int readLoggingInfo(struct soap *soap, logging_info * info)
{
	printf("Calling readLoggingInfo\n");

	if (soap_element_begin_in(soap, "vllogging:loggingconfig", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: start of loggingconfig expected",NULL);
		return 0;
	}
	if (soap_element_begin_in(soap, "vllogging:rate", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: start of rate expected",NULL);
		return 0;
	}

	const char* str = soap_value(soap);
	strcpy(info->rate,str);

	if (soap_element_end_in(soap, "vllogging:rate") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: end of rate expected",NULL);
		return 0;
	}

	if (soap_element_begin_in(soap, "vllogging:duration", 0, NULL) != SOAP_OK)
	{
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: start of duration expected",NULL);
		return 0;
	}

	str = soap_value(soap);
	strcpy(info->duration,str);

	if (soap_element_end_in(soap, "vllogging:duration") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: end of duration expected",NULL);
		return 0;
	}

	if (soap_element_end_in(soap, "vllogging:loggingconfig") != SOAP_OK) {
		soap->error = soap_sender_fault(soap,"tag name or namespace mismatch: end of loggingconfig expected",NULL);
		return 0;
	}
	return 1;
}

void writeSessionCount(struct soap *soap,session_info * info)
{
	printf("Calling writeSessionCount\n");
	soap_element_begin_out(soap, "vllogging:sessioninfo", 0, "");

	char * str = NULL;
	asprintf(&str, "%d", info->count);
	char * count_str = soap_strdup(soap, str);
	soap_outstring(soap, "vllogging:count", -1, &count_str, "", SOAP_TYPE_string);
	free(str);

	soap_element_end_out(soap, "vllogging:sessioninfo");
}
