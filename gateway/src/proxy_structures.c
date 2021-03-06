#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ws4d-gSOAP/dpws_device.h>

#include "proxy_handler.h"
#include "mutex_handling.h"
#include "service_cache.h"

#include "proxy_structures.h"

static char *gateway_interface = NULL;

static struct soap *proxy_structures_maccept(ws4d_time timeout);
static struct soap *proxy_structures_accept(ws4d_time timeout, int count, struct soap **soap_handles);

typedef struct remote_device {
	struct dpws_s *dpws_device;
	struct soap *soap_service;
	int (**serve_requests)(struct soap * soap);
	struct ws4d_list_node list; //pointer for list management
	char addr[];
} remote_device;

static struct ws4d_list_node head;

//don't forget to cleanup before shutting down, given string should be freed after calling this function
void gateway_set_interface(const char *interface)
{
	gateway_clear_interface();
	gateway_interface = strdup(interface);
}

char *gateway_get_interface()
{
	return gateway_interface;
}

void gateway_clear_interface()
{
	if (gateway_interface != NULL) {
		free(gateway_interface);
	}
}

void device_proxy_init_device_list()
{
	WS4D_INIT_LIST(&head);
}

static remote_device *init_device(struct dpws_s *dpws_device,
	struct soap *service, void * addr, ssize_t addr_len)
{
	remote_device *rem_device = calloc(1, sizeof(remote_device)+addr_len);
	rem_device->dpws_device = dpws_device;
	memcpy(rem_device->addr, addr, addr_len);
	rem_device->soap_service = service;
	return rem_device;
}

struct dpws_s *remote_device_get_dpws_device(remote_device *rem_device)
{
	assert(rem_device != NULL);
	return rem_device->dpws_device;
}

struct soap *remote_device_get_soap_service(remote_device *rem_device)
{
	assert(rem_device != NULL);
	return rem_device->soap_service;
}

void *remote_device_get_addr(remote_device *rem_device)
{
	assert(rem_device != NULL);
	return rem_device->addr;
}

void device_proxy_list_del_device(remote_device *rem_device)
{
	gateway_mutex_lock();
	assert(rem_device != NULL);
	ws4d_list_del(&(rem_device->list));
	//gateway_broadcast();
	gateway_mutex_unlock();
}

void device_proxy_item_free(remote_device *rem_device)
{
	assert(rem_device != NULL);
	if (rem_device->dpws_device != NULL) {
		free(rem_device->dpws_device);
		rem_device->dpws_device = NULL;
	}

	free(rem_device);
}

remote_device *device_proxy_list_add_device(struct dpws_s *dpws_device,
		struct soap *service, void* addr,size_t addr_len) {


	remote_device *rem_device = init_device(dpws_device, service, addr,addr_len);

	gateway_mutex_lock();
	ws4d_list_add_tail(&(rem_device->list), &head);
	gateway_broadcast();
	gateway_mutex_unlock();

	return rem_device;
}

remote_device *device_proxy_list_get_device(int (*comparefunc)(void *address,void *key), void* key)
{
	remote_device *rem_device = NULL, *next = NULL;
	ws4d_list_foreach(rem_device, next, &head, remote_device, list) {
		if (comparefunc(rem_device->addr,key)) {
			return rem_device;
		}
	}

	return NULL;
}

int device_proxy_list_empty()
{
	return ws4d_list_empty(&head);
}

remote_device *device_proxy_list_get_device_by_dpws_device(const char *uuid)
{
	remote_device *rem_device = NULL, *next = NULL;
	ws4d_list_foreach(rem_device, next, &head, remote_device, list) {
		printf("Comparing %s and %s\n",rem_device->dpws_device->hosting_addr,uuid);
		if (strstr(rem_device->dpws_device->hosting_addr, uuid)!=NULL) {
			return rem_device;
		}
	}

	return NULL;
}

int device_proxy_list_count_devices()
{
	int device_count = 0;
	remote_device *rem_device = NULL, *next = NULL;
	ws4d_list_foreach(rem_device, next, &head, remote_device, list) {
		device_count ++;
	}

	return device_count;
}

struct soap *device_proxy_list_check_handles(remote_device ** rem_device)
{
	struct soap *handle = NULL;

	handle = proxy_structures_maccept(1000);

	return handle;
}

void device_proxy_list_cleanup()
{
	remote_device *rem_device = NULL, *next = NULL;

	ws4d_list_foreach(rem_device, next, &head, remote_device, list) {
		dpws_device_unregister(rem_device);
	}

	gateway_mutex_lock();
		gateway_broadcast();
	gateway_mutex_unlock();
}

void device_proxy_list_check_subscriptions()
{
	remote_device *rem_device = NULL, *next = NULL;

	gateway_mutex_lock();

	ws4d_list_foreach(rem_device, next, &head, remote_device, list) {
		dpws_check_subscriptions(rem_device->dpws_device);
	}

	gateway_mutex_unlock();
}

/* New approach: get everything we have to listen to (maccept, nodeconnections and discovery) and listen
 * to them in a select.
 * With this, it is possible to prevent locking as it is serial, node communication may then be done with
 * worker threads and an event queue mechanism
 * Things to think about: it is not good to loop through every structure to find the one connection which
 * is receiving an event
 */

static int service_get_master(struct soap *service) {
	if (soap_valid_socket (service->master)) {
		return service->master;
	} else {
		printf("unregistered soap handle found\n");
		return -1;
	}
}

void proxy_structure_get_soap_fds(remote_device *rem_device, int *write_ptr, int *big_fd) {
	struct dpws_s *dpws_device = remote_device_get_dpws_device(rem_device);
	struct soap *service = remote_device_get_soap_service(rem_device);
	int current_socket = -1;

	//ws4d_assert(dpws_device && service, NULL);

	if ((current_socket = service_get_master(service)) != -1) {
		*write_ptr = current_socket;
		if (current_socket > *big_fd) {
			*big_fd = current_socket;
		}
		write_ptr++;
	}

	if (dpws_device->dis_mca) {
		if ((current_socket = service_get_master(dpws_device->dis_mca)) != -1) {
			*write_ptr = current_socket;
			if (current_socket > *big_fd) {
				*big_fd = current_socket;
			}
			write_ptr++;
		}
	}

	if (dpws_device->hosting_handle) {
		if ((current_socket = service_get_master(dpws_device->hosting_handle)) != -1) {
			*write_ptr = current_socket;
			if (current_socket > *big_fd) {
				*big_fd = current_socket;
			}
			write_ptr++;
		}
	}
}
#if 0
static void proxy_structure_get_node_event_fd(remote_device *rem_device, int *write_ptr, int *big_fd) {
	int current_socket = -1;
	if ((current_socket = remote_device_get_eventing_socket(rem_device)) > 0) {
		*write_ptr = current_socket;
		if (current_socket > *big_fd) {
			*big_fd = current_socket;
		}
		write_ptr++;
	}
}

int *proxy_structures_get_incoming_ports(int *big_fd) {
	remote_device *rem_device = NULL, *next = NULL;
	int device_list_size = device_proxy_list_count_devices();
	int *fd_list = NULL;
	int *fd_iter = 0;

	if (device_list_size > 0) {
		fd_list = calloc(device_list_size * 4, sizeof(void *));
	}

	ws4d_list_foreach(rem_device, next, &head, remote_device, list) {
		proxy_structure_get_soap_fds(rem_device, fd_iter, big_fd);
		proxy_structure_get_node_event_fd(rem_device, fd_iter, big_fd);
	}

	return fd_list;
}
#endif

struct soap *proxy_structures_maccept(ws4d_time timeout) {
	int index;

	struct soap **soap_set = NULL;
	struct soap *result;
	remote_device *rem_device = NULL, *next = NULL;

	int service_list_size = service_cache_count_services();

	//if (service_list_size > 0) {
		soap_set = calloc(service_list_size + (device_proxy_list_count_devices() * 2), sizeof(void *));
	//}

	for (index = 0; index < service_list_size; index++) {
		soap_set[index] = service_cache_get_service(index);
	}

	ws4d_list_foreach(rem_device, next, &head, remote_device, list) {
		struct dpws_s *dpws_device = remote_device_get_dpws_device(rem_device);

		ws4d_assert(dpws_device && soap_set, NULL);

		if (dpws_device->dis_mca) {
			soap_set[index] = dpws_device->dis_mca;
			index++;
		}

		if (dpws_device->hosting_handle) {
			soap_set[index] = dpws_device->hosting_handle;
			index++;
		}
	}

	result = proxy_structures_accept(timeout, index, soap_set);

	if (soap_set != NULL) {
		free(soap_set);
	}

	return result;
}

struct soap *proxy_structures_accept(ws4d_time timeout, int count,
		struct soap **soap_handles) {
	struct timeval timeout_t;
	register int i, r = -1;
	SOAP_SOCKET bigfd = SOAP_INVALID_SOCKET;
	fd_set fd;
	struct soap *result = NULL;

	FD_ZERO (&fd);

	if (!soap_handles)
		return result;

	for (i = 0; i < count; i++) {
		if (soap_valid_socket (soap_handles[i]->master)) {
			FD_SET ((SOAP_SOCKET) soap_handles[i]->master, &fd);
			if ((SOAP_SOCKET) soap_handles[i]->master > bigfd)
				bigfd = soap_handles[i]->master;
		} else {
			printf("register soap handle number %d first\n", i);
			goto exit;
		}
	}

	if (timeout != DPWS_SYNC) {
		timeout_t.tv_sec = timeout / 1000;
		timeout_t.tv_usec = (timeout % 1000) * 1000;
	}

	while (r < 0) {
		r = select(bigfd + 1, &fd, NULL, NULL, &timeout_t);
		if (r > 0) {
			for (i = 0; i < count; i++) {
				if (soap_valid_socket (soap_handles[i]->master)) {
					if (FD_ISSET ((SOAP_SOCKET) soap_handles[i]->master, &fd)) {
						result = soap_handles[i];
						goto exit;
					}
				}
			}
		}
	}

	exit:

	if (result)
		soap_accept(result);

	return result;
}

