/*
 * service_structure.h
 *
 *  Created on: 29.12.2009
 *      Author: polygox
 */

#ifndef SERVICE_STRUCTURE_H_
#define SERVICE_STRUCTURE_H_

//initializes the list used by this cache implementation
void service_cache_init();

struct soap *service_cache_register_node_on_service(int service_id, void (*device_init_service)(struct soap *));
void service_cache_unregister_node_on_service(int service_id);

struct soap *service_cache_get_service(int index);
int service_cache_count_services();

#endif /* SERVICE_STRUCTURE_H_ */
