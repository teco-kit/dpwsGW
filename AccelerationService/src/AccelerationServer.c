#include <ws4d-gSOAP/dpwsH.h>
#include <ws4d-gSOAP/ws-addressing.h>
#include <stdsoap2.h>

#include <ws4d-gSOAP/dpws_device.h>

int send_buf(struct dpws_s *, uint16_t , uint8_t , struct soap* , u_char* , ssize_t );
ssize_t rcv_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id, struct soap* msg, char **buf);

#include "Acceleration_operations.h"
#include "Conversion.h"

static int soap_serve_StartLDC(struct soap *soap) //TODO: pass device context
{
	printf("Calling soap_serve_StartLDC\n");
	int op_id = OP_StartLDC;
	int service_id = SRV_Acceleration;
	struct dpws_s *device = NULL;
	int error=WS4D_OK;

	LDCInfo info;
	strcpy(info.rate,"1"); // Default rate is 1 Hz
	int ret = 0;
	if(!readLDCInfo(soap, &info))
	{
		printf("Error Calling readLDCInfo\n");
		printf("Read LDC rate: %s\n",info.rate);
		error=soap->error;
	} else {
		printf("LDC rate %s\n",info.rate);
		printf("Calling send_buf\n");
		ret = send_buf(device, service_id, op_id, soap, (u_char *)&info, sizeof(LDCInfo));
	}

	/* prepare response */
	{
		const char* To = wsa_header_get_ReplyTo(soap);
		char * MessageId = wsa_header_get_MessageId(soap);
		char * Action = "http://www.teco.edu/AccelerationService/StartLDCOut";

		soap->omode |= SOAP_IO_CHUNK;

		if (!To || (0 == strcmp(To, wsa_anonymousURI))) {
			To = wsa_anonymousURI;
		} else {

		}

		if (To != wsa_anonymousURI) // no strcmp needed -> s.a.
		{
			struct soap *reply_soap = soap_copy(soap);
			if (reply_soap) {
				soap_copy_stream(reply_soap, soap);
				soap_clr_omode(reply_soap, SOAP_ENC_MIME | SOAP_ENC_DIME
						| SOAP_ENC_MTOM);
				soap->socket = SOAP_INVALID_SOCKET; /* prevents close */
				if (soap_connect(soap, To, Action)) /*Todo: can this be delayed ??*/
					return soap->error;
				soap_send_empty_response(reply_soap, SOAP_OK); /* HTTP ACCEPTED */
				soap_closesock(reply_soap);
				soap_end(reply_soap);
				soap_free(reply_soap);
				/*
				 data->fresponse = soap->fresponse;
				 soap->fresponse = soap_wsa_response;
				 response will be a POST */
			} else
				return soap->error;
		} else {
			if (soap_response(soap, SOAP_OK) != SOAP_OK)
				return soap->error;
		}

		wsa_header_gen_response(soap, NULL, To, Action, MessageId,
									sizeof(struct SOAP_ENV__Header));
		if(error!=WS4D_OK)
		{

           return error;
		}

	}

	if (soap_envelope_begin_out(soap) || soap_putheader(soap)
			|| soap_body_begin_out(soap)) {
		return soap->error;
	}

	{
		char * buf;
		ssize_t len = rcv_buf(device, service_id, op_id, soap, &buf);

		if (ret == ACLERR_NotReady||len == ACLERR_NotReady) {
			soap->error = soap_receiver_fault(soap, "Node not ready. Please stop the node first.", NULL);
			return soap->error;
		} else if (ret == ACLERR_GW_Busy||len == ACLERR_GW_Busy) {
			soap->error = soap_receiver_fault(soap, "Gateway is busy. Please stop all other nodes first.", NULL);
			return soap->error;
		}


	}

	if (soap_body_end_out(soap) || soap_envelope_end_out(soap)
			|| soap_end_send(soap)) {
		return soap->error;
	}

	return soap_closesock(soap);
}

int AccelerationService_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (soap->action)
	{
		printf("Calling AccelerationService_serve_request with soap action %s\n",soap->action);
	} else {
		printf("Calling AccelerationService_serve_request without soap action\n");
	}
	if ((soap->action && !strcmp(soap->action,
			"http://www.teco.edu/AccelerationService/StartLDCIn"))

	)
		return soap_serve_StartLDC(soap);

	return soap->error = SOAP_NO_METHOD;
}
