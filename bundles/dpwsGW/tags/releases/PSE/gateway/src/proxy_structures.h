/*
 * proxy_structures.h
 *
 *  Created on: 23.06.2009
 *      Author: genaid
 */

#ifndef PROXY_STRUCTURES_H_
#define PROXY_STRUCTURES_H_
#include <sys/types.h>

struct remote_device;

void gateway_set_interface(const char *interface);
char *gateway_get_interface();
void gateway_clear_interface();

struct dpws_s *remote_device_get_dpws_device(struct remote_device *rem_device);
struct soap *remote_device_get_soap_service(struct remote_device *rem_device);

void  *remote_device_get_addr(struct remote_device *rem_device);

void device_proxy_init_device_list();

struct remote_device *device_proxy_list_add_device(struct dpws_s *dpws_device,
	struct soap *service, void * addr, size_t addr_len);

void device_proxy_list_del_device(struct remote_device *rem_device);
void device_proxy_item_free(struct remote_device *rem_device);

int device_proxy_list_count_devices(void);
int device_proxy_list_empty(void);

struct remote_device *device_proxy_list_get_device(int (*comparefunc)(void *address,void *key), void* key);
struct remote_device *device_proxy_list_get_device_by_dpws_device(const char *uuid);

struct soap *device_proxy_list_check_handles();
void device_proxy_list_check_subscriptions();

void device_proxy_list_cleanup();

#endif /* PROXY_STRUCTURES_H_ */
