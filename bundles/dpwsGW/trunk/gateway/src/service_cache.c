/*
 * service_structure.c
 *
 *  Created on: 29.12.2009
 *      Author: polygox
 */

/* structure and interface for usage of one service */
#include "service_cache.h"

#include <stdlib.h>

#include <stdsoap2.h>
#include <ws4d-gSOAP/dpws_device.h>
#include <assert.h>

typedef struct service_container {
	struct ws4d_list_node list; //pointer for list management
	int service_id;
	struct soap service;
	int num_users; //for reference counting
} service_container;

static struct ws4d_list_node head;

void service_container_initialize(service_container *sc, int service_id) {
	sc->num_users = 0;
	sc->service_id = service_id;
}

//returns the index of the service in the structure, reference counting is done
static service_container *__build_service(int service_id, void (*device_init_service)(struct soap *)) {
	service_container *sc = calloc(1, sizeof(service_container));
	service_container_initialize(sc, service_id);

	// build service structure with the given service construction function.. //TODO: use service id?/what is "model?
	device_init_service(&(sc->service));

	return sc;
}

//registers a node on a service
static void __register_node(service_container *sc) {
	assert(sc != NULL);
	sc->num_users += 1;
}

//unregisters a node on a service, deletion when no reference left.
int __unregister_node(service_container *sc) {
	assert(sc != NULL);
	assert(sc->num_users > 0);
	return (--(sc->num_users));
}

/* structure and interface for handling multiple services */

static service_container *__list_get_container(int service_id) {
	service_container *sc = NULL, *next = NULL;
	ws4d_list_foreach(sc, next, &head, service_container, list) {
		if (sc->service_id == service_id) {
			return sc;
		}
	}

	return NULL;

}

static int __list_has_container(int service_id) {
	return (__list_get_container(service_id) != NULL);
}

void __list_add_container(service_container *sc) {
	assert(!__list_has_container(sc->service_id));
	ws4d_list_add_tail(&(sc->list), &head);
}

//TODO: it should be possible to use the service id for searching the device_init_service_function
//TODO: check the need for locking
struct soap *service_cache_register_node_on_service(int service_id, void (*device_init_service)(struct soap *)) {
	service_container *sc = __list_get_container(service_id);
	if (sc == NULL) {

#ifdef DEBUG
		printf("Building a service with id %d as the first node registrated on it.\n", service_id);
#endif
		sc = __build_service(service_id, device_init_service);
		__list_add_container(sc);
	}

	__register_node(sc);
	return &(sc->service);
}

void service_cache_unregister_node_on_service(int service_id) {
	service_container *sc = __list_get_container(service_id);
	assert(sc != NULL);

	if (__unregister_node(sc) == 0) {
#ifdef DEBUG
		printf("Deleting the service with id %d as it has no users anymore.\n", service_id);
#endif
		ws4d_list_del(&(sc->list));
		soap_done(&(sc->service));
		free(sc);
	}
}

void service_cache_init() {
	WS4D_INIT_LIST(&head);
}

int service_cache_count_services() {
	service_container *sc = NULL, *next = NULL;
	int count = 0;
	ws4d_list_foreach(sc, next, &head, service_container, list) {
		count += 1;
	}
	return count;
}

struct soap *service_cache_get_service(int index) {
	service_container *sc = NULL, *next = NULL;
	int count = 0;

	ws4d_assert(service_cache_count_services() > index, NULL);

	ws4d_list_foreach(sc, next, &head, service_container, list) {
		if (count == index) {
			return &(sc->service);
		}
		count += 1;
	}

	return NULL;
}
