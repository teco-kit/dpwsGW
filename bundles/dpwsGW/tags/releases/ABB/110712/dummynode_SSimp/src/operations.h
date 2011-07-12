/*
 * operations.h
 *
 *  Created on: 02.03.2010
 *      Author: riedel
 */

#ifndef OPERATIONS_H_
#define OPERATIONS_H_
#include <SensorValues_operations.h>

int send_event(int com_fd,uint16_t svc, uint8_t op);
int handle_request(int);

#endif /* OPERATIONS_H_ */
