/*
 * proxy_handler.h
 *
 *  Created on: 12.07.2009
 *      Author: genaid
 */

#ifndef PROXY_HANDLER_H_
#define PROXY_HANDLER_H_

struct remote_device;
struct sockaddr_storage;
struct dpws_s;

int dpws_device_build_and_register(char *model, void * addr,
		size_t addr_len, char *interface_name, char *version, char *serial_num,
		char *friendly_name);

int dpws_device_set_node_meta_data(struct dpws_s *device,
		const char *version, const char *serial_number,
		const char *friendly_name);

void dpws_device_check_handles();

int dpws_device_unregister(struct remote_device *rem_device);

#endif /* PROXY_HANDLER_H_ */
