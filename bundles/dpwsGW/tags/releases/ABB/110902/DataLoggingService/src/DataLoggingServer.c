#include <ws4d-gSOAP/dpwsH.h>
#include <ws4d-gSOAP/ws-addressing.h>
#include <stdsoap2.h>

#include <ws4d-gSOAP/dpws_device.h>

int send_buf(struct dpws_s *, uint16_t , uint8_t , struct soap* , u_char* , ssize_t );
ssize_t rcv_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id, struct soap* msg, char **buf);

#include <microstrain_struct.h>
#include "DataLogging_operations.h"
#include "Conversion.h"

static int soap_serve_StartLogging(struct soap *soap) //TODO: pass device context
{
	printf("Calling soap_serve_StartLogging\n");
	int op_id = OP_AccelModel_DataLogging_StartLogging;
	int service_id = SRV_DataLogging;
	struct dpws_s *device = NULL;

	logging_info info;
	strcpy(info.rate,"32"); // Default rate is 32 Hz


	if(!readLoggingInfo(soap, &info))
		return soap->error;


	int ret = send_buf(device, service_id, op_id, soap, (u_char*)&info, sizeof(logging_info));

	/* prepare response */
	{
		const char* To = wsa_header_get_ReplyTo(soap);
		char * MessageId = wsa_header_get_MessageId(soap);
		char * Action = "http://www.teco.edu/DataLoggingService/StartLoggingOut";

		soap->omode |= SOAP_IO_CHUNK;

		wsa_response(soap, NULL, To, Action, MessageId,
				sizeof(struct SOAP_ENV__Header));
	}

	{
		char * buf;
		ssize_t len = rcv_buf(device, service_id, op_id, soap, &buf);

		if (ret == DLERR_NotReady) {
			soap->error = soap_receiver_fault(soap, "Node not ready. Please stop the node first.", NULL);
			return soap->error;
		} else if (ret == DLERR_NotCleared) {
			soap->error = soap_receiver_fault(soap, "Node has not been cleared. Please erase previous session first.", NULL);
			return soap->error;
		} else if (ret == DLERR_GW_Busy) {
			soap->error = soap_receiver_fault(soap, "Gateway is busy. Please stop all other nodes first.", NULL);
			return soap->error;
		}


	}

	if (soap_response(soap, SOAP_OK) ||soap_envelope_begin_out(soap) || soap_putheader(soap)
			|| soap_body_begin_out(soap)) {
		return soap->error;
	}



	if (soap_body_end_out(soap) || soap_envelope_end_out(soap)
			|| soap_end_send(soap)) {
		return soap->error;
	}

	return soap_closesock(soap);
}

static int soap_serve_StartDownload(struct soap *soap)
{
	printf("Calling soap_serve_StartDownload\n");
	int op_id = OP_AccelModel_DataLogging_StartDownload;
	int service_id = SRV_DataLogging;
	struct dpws_s *device = NULL;

	int ret = send_buf(device, service_id, op_id, soap, NULL, 0);

	/* prepare response */
	{
		const char* To = wsa_header_get_ReplyTo(soap);
		char * MessageId = wsa_header_get_MessageId(soap);
		char * Action = "http://www.teco.edu/DataLoggingService/StartDownloadOut";

		soap->omode |= SOAP_IO_CHUNK;

		wsa_response(soap, NULL, To, Action, MessageId,
				sizeof(struct SOAP_ENV__Header));
	}

	{
		char * buf;
		ssize_t len = rcv_buf(device, service_id, op_id, soap, &buf);

		if (ret == DLERR_NotReady) {
			soap->error = soap_receiver_fault(soap, "Node not ready. Please stop the node first.", NULL);
			return soap->error;
		} else if (ret == DLERR_NoSessions) {
			soap->error = soap_receiver_fault(soap, "Node does not contain any sessions. Please start a logging session first.", NULL);
			return soap->error;
		} else if (ret == DLERR_GW_Busy) {
			soap->error = soap_receiver_fault(soap, "Gateway is busy. Please stop all other nodes first.", NULL);
			return soap->error;
		}


	}

	if (soap_response(soap, SOAP_OK) ||soap_envelope_begin_out(soap) || soap_putheader(soap)
			|| soap_body_begin_out(soap)) {
		return soap->error;
	}



	if (soap_body_end_out(soap) || soap_envelope_end_out(soap)
			|| soap_end_send(soap)) {
		return soap->error;
	}

	return soap_closesock(soap);
}

static int soap_serve_Erase(struct soap *soap)
{
	printf("Calling soap_serve_Erase\n");
	int op_id = OP_AccelModel_DataLogging_Erase;
	int service_id = SRV_DataLogging;
	struct dpws_s *device = NULL;

	int ret = send_buf(device, service_id, op_id, soap, NULL, 0);

	/* prepare response */
	{
		const char* To = wsa_header_get_ReplyTo(soap);
		char * MessageId = wsa_header_get_MessageId(soap);
		char * Action = "http://www.teco.edu/DataLoggingService/EraseOut";

		soap->omode |= SOAP_IO_CHUNK;

		wsa_response(soap, NULL, To, Action, MessageId,
				sizeof(struct SOAP_ENV__Header));
	}

	{
		char * buf;
		ssize_t len = rcv_buf(device, service_id, op_id, soap, &buf);

		if (ret == DLERR_NotReady) {
			soap->error = soap_receiver_fault(soap, "Node not ready. Please stop the node first.", NULL);
			return soap->error;
		} else if (ret == DLERR_GW_Busy) {
			soap->error = soap_receiver_fault(soap, "Gateway is busy. Please stop all other nodes first.", NULL);
			return soap->error;
		}


	}

	if (soap_response(soap, SOAP_OK) ||soap_envelope_begin_out(soap) || soap_putheader(soap)
			|| soap_body_begin_out(soap)) {
		return soap->error;
	}



	if (soap_body_end_out(soap) || soap_envelope_end_out(soap)
			|| soap_end_send(soap)) {
		return soap->error;
	}

	return soap_closesock(soap);
}

static int soap_serve_GetSessionCount(struct soap *soap)
{
	printf("Calling soap_serve_GetSessionCount\n");
	int op_id = OP_AccelModel_DataLogging_GetSessionCount;
	int service_id = SRV_DataLogging;
	struct dpws_s *device = NULL;

	int ret = send_buf(device, service_id, op_id, soap, NULL, 0);

	/* prepare response */
	{
		const char* To = wsa_header_get_ReplyTo(soap);
		char * MessageId = wsa_header_get_MessageId(soap);
		char * Action = "http://www.teco.edu/DataLoggingService/GetSessionCountOut";

		soap->omode |= SOAP_IO_CHUNK;

		wsa_response(soap, NULL, To, Action, MessageId,
				sizeof(struct SOAP_ENV__Header));
	}

	session_info info;
	session_info * info_ptr = &info;
	ssize_t len = rcv_buf(device, service_id, op_id, soap, (char**)&info_ptr);

	if (ret == DLERR_NotReady) {
		soap->error = soap_receiver_fault(soap, "Node not ready. Please stop the node first.", NULL);
		return soap->error;
	} else if (ret == DLERR_GW_Busy) {
		soap->error = soap_receiver_fault(soap, "Gateway is busy. Please stop all other nodes first.", NULL);
		return soap->error;
	}

	if (len < 0) {
		soap->error = soap_receiver_fault(soap, "No reply from Node", NULL);
		return soap->error;
	}

	if (soap_response(soap, SOAP_OK) ||soap_envelope_begin_out(soap) || soap_putheader(soap)
			|| soap_body_begin_out(soap)) {
		return soap->error;
	}




	writeSessionCount(soap,info_ptr);




	if (soap_body_end_out(soap) || soap_envelope_end_out(soap)
			|| soap_end_send(soap)) {
		return soap->error;
	}

	return soap_closesock(soap);
}

int DataLoggingService_serve_request(struct soap *soap)
{

	soap_peek_element(soap);
	if (soap->action)
	{
		printf("Calling DataLoggingService_serve_request with soap action %s\n",soap->action);
	} else {
		printf("Calling DataLoggingService_serve_request without soap action\n");
	}


	if ((soap->action && !strcmp(soap->action,
			"http://www.teco.edu/DataLoggingService/StartLoggingIn"))

	)
		return soap_serve_StartLogging(soap);

	if ((soap->action && !strcmp(soap->action,
			"http://www.teco.edu/DataLoggingService/StartDownloadIn"))

	)
		return soap_serve_StartDownload(soap);

	if ((soap->action && !strcmp(soap->action,
			"http://www.teco.edu/DataLoggingService/EraseIn"))

	)
		return soap_serve_Erase(soap);

	if ((soap->action && !strcmp(soap->action,
			"http://www.teco.edu/DataLoggingService/GetSessionCountIn"))

	)
		return soap_serve_GetSessionCount(soap);

	return soap->error = SOAP_NO_METHOD;
}
