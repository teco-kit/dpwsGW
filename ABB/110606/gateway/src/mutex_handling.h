/*
 * mutex_handling.h
 *
 *  Created on: 20.12.2009
 *      Author: polygox
 */

#ifndef MUTEX_HANDLING_H_
#define MUTEX_HANDLING_H_

void gateway_mutex_init();

int gateway_mutex_lock();
int gateway_mutex_unlock();

int gateway_mutex_trylock();

void gateway_wait();
void gateway_broadcast();

#endif /* MUTEX_HANDLING_H_ */
