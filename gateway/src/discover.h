/*
 * udp_discover.h
 *
 *  Created on: 15.06.2009
 *      Author: polygox
 */

#ifndef UDP_DISCOVER_H_
#define UDP_DISCOVER_H_

#include "proxy_structures.h"

struct sockaddr;

int discovery_worker_init(void);
void *discovery_get_in_addr(struct sockaddr *sa);
void discovery_worker_shutdown(void);
void discovery_worker_wait(void);
void discovery_notify_unregister(struct remote_device *rem_device);
int gateway_running(void);

#include <ws4d-gSOAP/dpwsH.h>
#include <ws4d-gSOAP/dpws_device.h>

void send_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id, struct soap* msg, u_char* buf, ssize_t len);
ssize_t rcv_buf(struct dpws_s *device, uint16_t service_id, uint8_t op_id, struct soap* msg, char **buf);

#endif /* UDP_DISCOVER_H_ */
