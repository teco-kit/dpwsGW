#include <ws4d-gSOAP/dpws_device.h>
#include <bitsio/read_bits_buf.h>
#include <ws4d-gSOAP/dpwsH.h>
#include <microstrain_struct.h>
#include "DataLogging_operations.h"
#include <sendrcv.h>
#include <assert.h>
extern char * DPWS_SUBS_END_FAILURE; //TODO missing decleration in ws4d header

#include "Conversion.h"

struct dpws_s *_device;

enum DataLogging_operations DataLoggingService_get_opid(char *soap_action_uri) {

	if (0 == strcmp(soap_action_uri,
			"http://www.teco.edu/DataLoggingService/DataLoggingServiceEventOut"))
		return OP_AccelModel_DataLogging_DataEvent;

	return -1;
}

char * DataLoggingService_get_opuri(enum DataLogging_operations op) {
	switch (op) {

	case OP_AccelModel_DataLogging_DataEvent:
		return "http://www.teco.edu/DataLoggingService/DataLoggingServiceEventOut";

	default:
		return NULL;
	}
}

void DataLoggingService_event(enum DataLogging_operations op, void *_device,
		char * buf, size_t buf_len) {

	struct soap soap;
	struct ws4d_subscription *subs, *next;

	struct dpws_s *device = (struct dpws_s *) _device;

	soap_init(&soap);

	soap_set_namespaces(&soap, device->hosting_handle->namespaces); //TODO:Check if correct
	//soap_set_namespaces (&soap, namespaces);
	{

		char *soap_action_uri = DataLoggingService_get_opuri(op);
		if (!soap_action_uri)
			return;

		{

dpws_for_each_subs		(subs, next, _device,
				soap_action_uri)
		{

			if(WS4D_OK!=
					dpws_header_gen_event(&soap,device,soap_action_uri,subs,sizeof(struct SOAP_ENV__Header)))
			{
				printf("Error in gen_event, uri %s\n",soap_action_uri);
				continue;
			}

			soap.omode|=SOAP_IO_CHUNK;

			if ( soap_connect(&soap, dpws_subsm_get_deliveryPush_address (device, subs), soap_action_uri)
					|| soap_envelope_begin_out(&soap)
					|| soap_putheader(&soap)
					|| soap_body_begin_out(&soap) )
			{
				soap_closesock(&soap);
				soap_print_fault (&soap, stderr);
				dpws_end_subscription(_device,subs,DPWS_SUBS_END_FAILURE,NULL);
			}

			{

				switch(op)
				{
					assert(0&&"unreachable"); //optimized for compiler if NDEBUG

					case OP_AccelModel_DataLogging_DataEvent:

					{
						writeEvent(&soap,buf,buf_len);
					}

					break;
					default:
					break;
				}
			}
			if ( soap_body_end_out(&soap)
					|| soap_envelope_end_out(&soap)
					|| soap_end_send(&soap) )
			{
				soap_closesock(&soap);
				soap_print_fault (&soap, stderr);
			}
			soap_end (&soap);
		}
	}
	soap_done (&soap);
	return;
}
}

