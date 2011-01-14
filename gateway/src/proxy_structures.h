/*
 * proxy_structures.h
 *
 *  Created on: 23.06.2009
 *      Author: genaid
 */

#ifndef PROXY_STRUCTURES_H_
#define PROXY_STRUCTURES_H_
#include <sys/types.h>
#include <ws4d-gSOAP/dpws_device.h>

struct remote_device;
struct dpws_model;

typedef int (*serve_requests_ptr)(struct soap *);



void gateway_set_interface(const char *interface);
char *gateway_get_interface();
void gateway_clear_interface();

struct dpws_s *remote_device_get_dpws_device(struct remote_device *rem_device);
struct soap *remote_device_get_soap_service(struct remote_device *rem_device);
struct dpws_model *remote_device_get_model(struct remote_device *rem_device);

void  *remote_device_get_addr(struct remote_device *rem_device);

void device_proxy_init_device_list();

void * device_get_func(struct remote_device *rem_device, const char * name);
void * device_event_func(struct remote_device *rem_device);
void * model_get_func(struct dpws_model *model, const char * name);

struct dpws_model * device_proxy_get_model(char * id);

struct dpws_model * init_model(const char * id);

struct remote_device *device_proxy_list_add_device(struct dpws_s *dpws_device,struct dpws_model *model,
	struct soap *service, void * addr, size_t addr_len);

void device_proxy_list_del_device(struct remote_device *rem_device);
void device_proxy_list_del_model(struct dpws_model *model);
void device_proxy_item_free(struct remote_device *rem_device);

void device_proxy_model_free(struct dpws_model *model);

int device_proxy_list_count_devices(void);
int device_proxy_list_empty(void);

int device_proxy_list_count_services();
void device_proxy_get_all_services(serve_requests_ptr * serve_requests);

struct remote_device *device_proxy_list_get_device(int (*comparefunc)(void *address,void *key), void* key);
struct remote_device *device_proxy_list_get_device_by_dpws_device(const char *uuid);

struct soap *device_proxy_list_check_handles();
void device_proxy_list_check_subscriptions();

void device_proxy_list_cleanup();

#endif /* PROXY_STRUCTURES_H_ */
