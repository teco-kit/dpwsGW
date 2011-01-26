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

void discovery_set_device(char * discdevice);

#endif /* UDP_DISCOVER_H_ */
