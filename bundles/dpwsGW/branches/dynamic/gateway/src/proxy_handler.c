#include <stdlib.h>
#include <stdio.h>
//#include <uuid/uuid.h>
#include <ws4d-gSOAP/ws4d_device_description.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "proxy_handler.h"
#include "discover.h"

#include "proxy_structures.h"
#include "mutex_handling.h"
#include "service_cache.h"

#define MODEL(X) DPWSModel##X

static int dpws_device_proxy_done(struct dpws_s *dpws);

// caller has to free allocated memory for string
static char *generate_uuid() {
	char uuidstr[WS4D_UUID_SIZE];
	char *id_buf = calloc(WS4D_UUID_SCHEMA_SIZE, sizeof(char));

	ws4d_uuid_generate_random(uuidstr);
	ws4d_uuid_generate_schema (id_buf, uuidstr);

	return id_buf;
}

static struct remote_device * get_device(struct soap* msg) {
		char *wsa_header = NULL;
		char *last_slash = NULL;
		char dev_uid[46] = { 0 };

		if (msg != NULL && ((wsa_header = wsa_header_get_To(msg)) != NULL)) {
			last_slash = strrchr(wsa_header, (int) '/');
		}

		if (last_slash != NULL) {
			strncpy(dev_uid, last_slash + 1, 45);
		}

		return device_proxy_list_get_device_by_dpws_device(dev_uid);
	}

void dpws_device_check_handles() {
	struct soap *handle = NULL;
	struct remote_device *rem_device = NULL;

	serve_requests_ptr * serve_requests = NULL;
	int count = 0;

	/* waiting for new messages */
	gateway_mutex_lock();

	while (device_proxy_list_empty() && gateway_running())
		gateway_wait();

	handle = device_proxy_list_check_handles(&rem_device);
	rem_device = get_device(handle);

	if (handle) {
#ifdef DEBUG
		printf("\nGateway: processing request from %s:%d\n", inet_ntoa(
				handle->peer.sin_addr), ntohs(handle->peer.sin_port));
#endif
		printf("Type: %s\n",handle->type);
		printf("Host: %s\n",handle->host);
		printf("Endpoint: %s\n",handle->endpoint);
		printf("ID: %s\n",handle->id);
		printf("Path: %s\n",handle->path);


		/* dispatch messages */

		{
			//typedef int (*serve_requests_ptr)(struct soap *);


			// = dlsym(device_get_lib(rem_device),"DPWSModel_get_serve_requests");

			if(rem_device!=NULL)
			{
				serve_requests_ptr *(* get_serve_requests)(void) = NULL;
				int (* get_service_count)(void) = NULL;
				*(void **)(&get_serve_requests) = device_get_func(rem_device,"DPWSModel_get_serve_requests");
				*(void **)(&get_service_count) = device_get_func(rem_device,"DPWSModel_get_service_count");
				serve_requests = get_serve_requests();
				count = get_service_count();

				printf("Device found!\n");

			} else {


				// Go over every device and get number of services
				count = device_proxy_list_count_services();
				// Allocate memory for all services + 3 from SOAP_SERVE_SET macro
				serve_requests = calloc(sizeof(serve_requests_ptr),count+3);
				// Get all services
				device_proxy_get_all_services(serve_requests);

				printf("No device found!\n");
			}
			if (dpws_mserve(handle, count, serve_requests)) {
				soap_print_fault(handle, stderr);
			}

		}

		/* clean up soap's internally allocated memory */
		soap_end(handle);

		if(rem_device==NULL)
		{
			free(serve_requests);
		}
	}



	gateway_mutex_unlock();
}

static struct soap *init_service_structure(struct dpws_model * model) {

	//TODO: check for existing service (by id... where to get that from?), use this one if exists, build otherwise */
	//struct soap *node_service = calloc(1, sizeof(struct soap));
	struct soap *node_service = NULL;

	void (*device_init_service)(struct soap *);

	//printf("model is %d\n", );

	*(void **)(&device_init_service) = model_get_func(model,"DPWSModel_init_service");

	node_service = service_cache_register_node_on_service(0, device_init_service);

	return node_service;
}

static struct dpws_s *init_device_structure(struct dpws_model *model, char *interface, char *uuid,
		char *version, char *serial_num,
		char *friendly_name, struct soap *node_service)
{
	struct dpws_s *node_dev = calloc(1, sizeof(struct dpws_s));

	/* initialize device and services */
	if (dpws_init(node_dev, interface)!=SOAP_OK) {
		fprintf(stderr, "\nGateway: Can't init device\n");
		free(node_dev);
		return NULL;
	}
	{
		int (*setup_hosting_service)(struct dpws_s *device, struct soap *service, char *uuid);
		*(void **)(&setup_hosting_service) = model_get_func(model,"DPWSModel_setup_hosting_service");


		if (setup_hosting_service(node_dev, node_service, uuid) != 0) {
			goto failure;
		}
	}

	{
		int (*setup_device)(struct dpws_s *device, struct soap *service,void *,void *);
		*(void **)(&setup_device) = model_get_func(model,"DPWSModel_setup_device");
		if (setup_device(node_dev, node_service,send_buf,rcv_buf) != 0) {
			goto failure;
		}
	}
	{

		int (*set_metadata_device)(struct dpws_s *device);
		*(void **)(&set_metadata_device) = model_get_func(model,"DPWSModel_set_metadata_device");
		if (set_metadata_device(node_dev) != 0) {
			goto failure;
		}
	}

	{
		int (*set_metadata_model)(struct dpws_s *device);
		*(void **)(&set_metadata_model) = model_get_func(model,"DPWSModel_set_metadata_model");
		if (set_metadata_model(node_dev) != 0) {
			goto failure;
		}
	}

	{
		int (*set_wsdl)(struct dpws_s *device);
		*(void **)(&set_wsdl) = model_get_func(model,"DPWSModel_set_wsdl");
		if (set_wsdl(node_dev) != 0) {
			goto failure;
		}
	}

	dpws_device_set_node_meta_data(node_dev, version,
			serial_num, friendly_name);

	/* Update Metadata */
	if (dpws_update_Metadata(node_dev)) {
		fprintf(stderr, "\nGateway: Can't init metadata\n");
		dpws_device_proxy_done(node_dev);
		return NULL;
	}

	/* activate eventing. */
	if (dpws_activate_eventsource(node_dev, node_service)) {
		printf("\nGateway: Eventing already activated, will ignore this.\n");
	}

	if (dpws_activate_hosting_service(node_dev)) {
		fprintf(stderr, "\nGateway: Can't activate device\n");
		dpws_device_proxy_done(node_dev);
		return NULL;
	}

	return node_dev;

failure:
	ws4d_assert(node_dev != NULL, NULL);
	dpws_device_proxy_done(node_dev);
	free(node_dev);
	return NULL;
}

int dpws_device_unregister(struct remote_device *rem_device) {
	struct dpws_s *dpws_dev = remote_device_get_dpws_device(rem_device);

	discovery_notify_unregister(rem_device);

	device_proxy_list_del_device(rem_device);

	dpws_deactivate_hosting_service(dpws_dev);

	service_cache_unregister_node_on_service(0);

	dpws_device_proxy_done(dpws_dev);

	device_proxy_item_free(rem_device);

	printf("\nproxyhandler: successfully unregistered one device\n");

	return 0;
}

int dpws_model_unregister(struct dpws_model *model) {

	device_proxy_list_del_model(model);

	device_proxy_model_free(model);

	printf("\nproxyhandler: successfully unregistered one model\n");

	return 0;
}

int dpws_device_unregister_by_addr(int (*comparefunc)(void *address,void *key), void* key) {
	struct remote_device *rem_device = NULL;

	rem_device = device_proxy_list_get_device(comparefunc,key);

	if (rem_device != NULL) {
		return dpws_device_unregister(rem_device);
	} else {
		return -1;
	}
}

int dpws_device_set_node_meta_data(struct dpws_s *device,
		const char *version, const char *serial_number,
		const char *friendly_name) {

	struct ws4d_thisDevice *_device = dpws_change_thisdevice(device);
	ws4d_device_FriendlyName_var(_friendly_name) = {
			ws4d_locstring_init_string("de", friendly_name),};
	ws4d_device_set_FriendlyName(_device, _friendly_name, 1);

	ws4d_device_set_FirmwareVersion(_device, version);
	ws4d_device_set_SerialNumber(_device, serial_number);

	if (dpws_updatemetadata_Device(device)) {
		fprintf(stderr, "\nGateway: Can't init metadata\n");
		dpws_device_proxy_done(device);
		return -1;
	}

	return 0;
}

int dpws_device_build_and_register(char *model_id, void *addr,
		size_t addr_len, char *interface_name, char *version, char *serial_num,
		char *friendly_name) {
	//TODO: see task 16: uuid by hashing the node info
	char *uuid = generate_uuid();

	struct dpws_s *node_device = NULL;
	struct soap *service = NULL;
	struct dpws_model * model = NULL;
	model = device_proxy_get_model(model_id);
	printf("Model ID: %s\n",model_id);
	if(model==NULL)
	{
		printf("Creating new model\n");
		model = init_model(model_id);
	}

	if(model!=NULL)
	{

	service = init_service_structure(model);

	node_device = init_device_structure(model,interface_name, uuid, version, serial_num, friendly_name, service);
	}

	free(uuid);

	if (node_device != NULL) {
		device_proxy_list_add_device((struct dpws_s *) node_device,model,
				(struct soap *) service, addr,
				addr_len);
		return 0; //SUCCESS
	} else {
		return -1; //device could not be built and registered
	}
}

static int dpws_device_proxy_done(struct dpws_s *dpws) {
	/* test parameters */
	ws4d_assert (dpws, WS4D_ERR);

	if (dpws->hosting_handle) {
		if (dpws->dis_mca) {
			soap_done(dpws->dis_mca);
		}
	}
	ws4d_hosting_done(&dpws->hosting); //TODO: does not free everything, memleak
	ws4d_alloclist_done(&dpws->alloc_list);

	/* commented out due to problems with multiple devices */
	//dpws_done_targetservice ();

	return WS4D_OK;
}
