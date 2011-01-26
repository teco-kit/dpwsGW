/*
 * mutex_handling.h
 *
 *  Created on: 20.12.2009
 *      Author: polygox
 */

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
pthread_cond_t      proxy_cond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t proxy_mutex = PTHREAD_MUTEX_INITIALIZER;



void gateway_mutex_init() {
	/* initialize the mutex needed to synchronize the proxy data structure */
	if (pthread_mutex_init(&proxy_mutex, NULL) != 0) {
		printf("Mutex initialization was not successful, will quit.");
		exit(1);
	}
}

void gateway_mutex_destroy() {
	pthread_mutex_destroy(&proxy_mutex);
}

int gateway_mutex_lock() {
	return pthread_mutex_lock(&proxy_mutex);
}

int gateway_mutex_unlock() {
	//a check for the thread having the lock would be fine...

	return pthread_mutex_unlock(&proxy_mutex);
}

/* maybe useful for optimizations, deadlock situations... */
int gateway_mutex_trylock() {
	return pthread_mutex_trylock(&proxy_mutex);
}

void gateway_broadcast() {
	pthread_cond_broadcast(&proxy_cond);
}
void gateway_wait() {
	pthread_cond_wait(&proxy_cond, &proxy_mutex);
}
