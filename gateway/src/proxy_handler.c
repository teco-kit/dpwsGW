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

#include <ws4d-gSOAP/dpws_device.h>

#include "proxy_structures.h"
#include "mutex_handling.h"
#include "service_cache.h"

#ifdef UPART
#define MODEL(X) uPartDevice_##X
#elif MICROSTRAIN_GLINK
#define MODEL(X) AccelModel_##X
#elif MICROSTRAIN_VLINK
#define MODEL(X) VLinkModel_##X
#else
#define MODEL(X) SSimpDevice_##X
#endif

#ifdef MICROSTRAIN_GLINK
#define OP(X) OP_AccelModel_##X
#elif MICROSTRAIN_VLINK
#define OP(X) OP_VLinkModel_##X
#endif

#include <device.h>

//TODO use dynamic loading!!
#define get_device_func(FUNC_PTR,_MODEL,CALL)\
		do{\
			*(FUNC_PTR)=&MODEL(CALL);\
		}while(0)

static int dpws_device_proxy_done(struct dpws_s *dpws);

// caller has to free allocated memory for string
static char *generate_uuid() {
	char uuidstr[WS4D_UUID_SIZE];
	char *id_buf = calloc(WS4D_UUID_SCHEMA_SIZE, sizeof(char));

	ws4d_uuid_generate_random(uuidstr);
	ws4d_uuid_generate_schema (id_buf, uuidstr);

	return id_buf;
}

void dpws_device_check_handles() {
	struct soap *handle = NULL;
	struct remote_device *rem_device = NULL;

	/* waiting for new messages */
	gateway_mutex_lock();

	while (device_proxy_list_empty() && gateway_running())
		gateway_wait();

	handle = device_proxy_list_check_handles(&rem_device);

	if (handle) {
#ifdef DEBUG
		printf("\nGateway: processing request from %s:%d\n", inet_ntoa(
				handle->peer.sin_addr), ntohs(handle->peer.sin_port));
#endif

		/* dispatch messages */
		{

			ssize_t count=-1;
			device_serve_requests_ptr *sr;

			device_get_serve_requests_func get_serve_requests;

			get_device_func(&get_serve_requests,MODEL,get_serve_requests);

			count=get_serve_requests(&sr);

			if (dpws_mserve(handle, count, sr)) {
				soap_print_fault(handle, stderr);

			}
		}


		/* clean up soap's internally allocated memory */
		soap_end(handle);
	}



	gateway_mutex_unlock();
}

static struct soap *init_service_structure(char * model) {

	//TODO: check for existing service (by id... where to get that from?), use this one if exists, build otherwise */
	struct soap *node_service = calloc(1, sizeof(struct soap));
	//struct soap *node_service = NULL;

	device_init_service_func device_init_service;

	printf("model is %s\n", model);

	get_device_func(&device_init_service,MODEL,init_service);

	//node_service = service_cache_register_node_on_service(0, device_init_service);
	device_init_service(node_service);

	return node_service;
}

static struct dpws_s *init_device_structure(char *model, char *interface, char *uuid,
		char *version, char *serial_num,
		char *friendly_name, struct soap *node_service)
{
	struct dpws_s *node_dev = calloc(1, sizeof(struct dpws_s));

	/* initialize device and services */
	if (dpws_init(node_dev, interface)) {
		fprintf(stderr, "\nGateway: Can't init device\n");
		free(node_dev);
		return NULL;
	}
	{
		device_setup_hosting_service_func setup_hosting_service;
		get_device_func(&setup_hosting_service,MODEL,setup_hosting_service);

		if (setup_hosting_service(node_dev, node_service, uuid) != 0) {
			goto failure;
		}
	}

	{
		device_setup_device_func setup_device;
		get_device_func(&setup_device,MODEL,setup_device);

		if (setup_device(node_dev, node_service) != 0) {
			goto failure;
		}
	}
	{

		device_set_metadata_device_func set_metadata_device;
		get_device_func(&set_metadata_device,MODEL,set_metadata_device);

		if (set_metadata_device(node_dev) != 0) {
			goto failure;
		}
	}

	{
		device_set_metadata_model_func set_metadata_model;
		get_device_func(&set_metadata_model,MODEL,set_metadata_model);
		if (set_metadata_model(node_dev) != 0) {
			goto failure;
		}
	}

	{
		device_set_wsdl_func set_wsdl;
		get_device_func(&set_wsdl,MODEL,set_wsdl);
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

	{


	/* activate eventing. */
	if (dpws_activate_eventsource(node_dev, node_service)) {
		printf("\nGateway: Eventing already activated, will ignore this.\n");
	}

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

	//service_cache_unregister_node_on_service(0);
	soap_free(dpws_dev->hosting_handle);

	dpws_device_proxy_done(dpws_dev);

	device_proxy_item_free(rem_device);

	printf("\nproxyhandler: successfully unregistered one device\n");

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

int dpws_device_build_and_register(char *model, void *addr,
		size_t addr_len, char *interface_name, char *version, char *serial_num,
		char *friendly_name) {
	//TODO: see task 16: uuid by hashing the node info
	char *uuid = generate_uuid();

	struct dpws_s *node_device = NULL;

	struct soap *service = init_service_structure(model);

	node_device = init_device_structure(model,interface_name, uuid, version, serial_num, friendly_name, service);


	free(uuid);

	if (node_device != NULL) {
		device_proxy_list_add_device((struct dpws_s *) node_device,
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
