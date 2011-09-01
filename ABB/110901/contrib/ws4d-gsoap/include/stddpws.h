/* WS4D-gSOAP - Implementation of the Devices Profile for Web Services
 * (DPWS) on top of gSOAP
 * Copyright (C) 2007 University of Rostock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef STD_DPWS_H
#define STD_DPWS_H

#include <stdarg.h>

#include "stdsoap2.h"
#include "ws4d_misc.h"
#include "soap_misc.h"

#include "ws4d_epr.h"
#include "ws4d_abstract_eprlist.h"

#include "ws-addressing.h"

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#define DPWS_PORT_MAX_LEN (10)
#define DPWS_URI_MAX_LEN (1024)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifdef DPWS_PEER
#define DPWS_DEVICE
#define DPWS_CLIENT
#endif

#define DPWS_HANDLES_COUNT 3
#endif

/**
 * @addtogroup APISoapHandle Connect SOAP handles with DPWS handles
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

extern const char *dpws_soap_plugin_id;

/**
 * structure to connect soap handles to dpws plugin
 */
struct dpws_soap_s
{
  struct dpws_soap_s *copy_of;
  struct dpws_s *dpws;
  const char *transport;
  int port;
  const char *paddr;
    WS4D_ALLOCATOR_DECL (alloc);
};

/**
 * function to make a soap handle known to the dpws plugin
 *
 * @param dpws dpws plugin handle
 * @param soap soap handle to make known
 *
 * @return SOAP_OK on success or SOAP_ERR otherwise
 */
int dpws_handle_init (struct dpws_s *dpws, struct soap *soap);

/**
 * function to bind a soap handle to a ws4d_uri
 *
 * @param dpws dpws plugin handle
 * @param soap handle to bind
 * @param uri ws4d_uri where handle should be bound
 * @param size size of the ws4d_uri buffer
 * @param backlog
 *
 * @return the socket that was created on success or
 * SOAP_INVLAID_SOCKET on failure
 */
int dpws_handle_bind (struct dpws_s *dpws, struct soap *soap,
                      char *uri, size_t size, int backlog);

/**
 * function to get the address of a handle bound with dpws_handle_bind
 *
 * @param soap handle to get address
 *
 * @return address the handle is bound to or NULL on failure
 */
const char *dpws_handle_get_paddr (struct soap *soap);

/**
 * function to get the gsoap memory allocator a handle bound with dpws_handle_bind
 *
 * @param soap handle to get the allocator
 *
 * @return gsoap memory allocator or NULL on failure
 */

struct ws4d_abs_allocator *dpws_handle_get_allocator (struct soap *soap);

/**
 * function to wait for incoming messages on several soap handles
 *
 * @param dpws dpws plugin handle
 * @param timeout time in milliseconds to wait for incoming messages
 * @param count number of soap handles
 * @param soap_handles array of soap handle references
 *
 * @return a reference to one of soap handles that is ready to receive a message
 * or an error on failure
 */
struct soap *dpws_maccept (struct dpws_s *dpws, ws4d_time timeout,
                           int count, struct soap **soap_handles);

/**
 * function to serve an incoming message with several serve_request functions
 *
 * @param soap soap handle
 * @param count number of soap handles
 * @param serve_requests array of serve_request functions
 *
 * @return SOAP_OK on success or SOAP_ERR otherwise
 */
int dpws_mserve (struct soap *soap, int count,
                 int (*serve_requests[])(struct soap * soap));

/** @} */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
struct wsdl_s;
#endif

#ifdef DPWS_DEVICE
#include "ws4d_hostingservice.h"
#endif

/**
 * dpws plugin structure
 *
 * @ingroup DPWSAPI
 */
struct dpws_s
{
  /* Plugin */
  const char *host;
#ifdef DPWS_DEVICE

  const char *hosting_addr;
  const char *hosting_id;
  struct ws4d_epr *hosting_target;
  struct soap *hosting_handle;

  struct ws4d_hostingservice hosting;

  struct wsdl_s *wsdls;

  struct soap *subsman;
#endif
  /* Discovery */
  struct soap *dis_mca;
#ifdef DPWS_CLIENT
  struct wsd_uni_hooks_t *uni_hooks;
  struct soap *dis_uni;
  struct ws4d_tc_int *tc;
#endif
  /* Misc */
  ws4d_alloc_list alloc_list;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
};

#define dpws_get_alist(dpws) &(dpws)->alloc_list

/**
 * function to initialize a dpws plugin handle
 *
 * @ingroup DPWSAPI
 *
 * @code
 * struct dpws_s device;
 *
 * dpws_init(&device, "ip-address of network device");
 * @endcode
 *
 * @param dpws handle to initialize
 * @param host network interface where plugin is running
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_init (struct dpws_s *dpws, const char *host);

/**
 * function to destroy a dpws handle
 *
 * @ingroup DPWSAPI
 *
 * @code
 * struct dpws_s device;
 *
 * dpws_init(&device, "ip-address of network device");
 * dpws_done(&device);
 * @endcode
 *
 * @param dpws handle to destroy
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_done (struct dpws_s *dpws);

#ifdef WITH_MUTEXES
void dpws_lock (struct dpws_s *dpws);
void dpws_unlock (struct dpws_s *dpws);
#else /*WITH_MUTEXES */

/**
 * Locks a dpws handle
 *
 * @ingroup DPWSAPI
 *
 * A dpws handle can be locked at run time with:
 * @code
 * dpws_lock(dpws);
 * @endcode
 *
 * @param dpws handle to lock
 */
#define dpws_lock(dpws)

/**
 * Unlocks a dpws handle
 *
 * @ingroup DPWSAPI
 *
 * A dpws handle can be unlocked at run time with:
 * @code
 * dpws_unlock(dpws);
 * @endcode
 *
 * @param dpws handle to unlock
 */
#define dpws_unlock(dpws)
#endif /*WITH_MUTEXES */

/** @} */

/**
 * @addtogroup APIHostingService Hosting and target service
 * @ingroup APIDevice
 *
 * The hosting_handle service advertises hosted services in the network.
 *
 * @{
 */

#ifdef DPWS_DEVICE

int dpws_init_target (struct dpws_s *dpws, struct soap *dis_mca, int backlog);

/**
 * function to initialize the hosting service
 *
 * @param device dpws plugin handle
 * @param hosting soap handle for the hosting_handle service
 * @param port port where the service should listen for incoming messages
 * @param id identifier of the hosting_handle services
 * @param uuid unique identifier of the hosting_handle services
 * @param backlog backlog
 * @param https the device should use https
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_add_hosting_service (struct dpws_s *device, struct soap *hosting,
                              int port, const char *id, const char *uuid,
                              int backlog, int https);

/**
 * function to add a type to a device
 *
 * @param device dpws plugin handle
 * @param type type to add to device
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_add_type (struct dpws_s *device, struct ws4d_qname *type);

/**
 * function to add a type to a device
 *
 * @param device dpws plugin handle
 * @param types type to add to device
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_add_typestring (struct dpws_s *device, const char *types);

/**
 * function to get the list of types supported by a device
 *
 * @param device dpws plugin handle
 * @return list of types on success or NULL on failure
 */
const char *dpws_get_typeslist (struct dpws_s *device);

/**
 * function to add a scope to a device
 *
 * @param device dpws plugin handle
 * @param scope scope to add to device
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_add_scope (struct dpws_s *device, const char *scope);

/**
 * function to manually trigger hello message
 *
 * @param device dpws plugin handle
 */
int dpws_send_Hello (struct dpws_s *device, const char *types);

/**
 * function to manually trigger bye message
 *
 * @param device dpws plugin handle
 */
int dpws_send_Bye (struct dpws_s *device);

/**
 * function to activate the anouncment of the device
 *
 * @param device dpws plugin handle
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_activate_hosting_service (struct dpws_s *device);

/**
 * function to dectivate the anouncment of the device
 *
 * @param device dpws plugin handle
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_deactivate_hosting_service (struct dpws_s *device);

/**
 * function to update the metadata in dpws hosting mode
 *
 * @param device dpws plugin handle
 *
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_update_Metadata_hosting (struct dpws_s *device);

/**
 * function to update the metadata in dpws device mode
 *
 * @param device dpws plugin handle
 *
 * @return WS4D_OK on success, an error code otherwise
 */
int dpws_update_Metadata (struct dpws_s *device);

#endif
/** @} */

/**
 * Header generation
 *
 * @addtogroup APIDpwsHeader SOAP Header Generation
 * @ingroup DPWSAPI
 *
 * @{
 */

/**
 * String length of message id strings
 */
#define DPWS_MSGID_SIZE WS4D_UUID_SCHEMA_SIZE

/**
 * function generates a message id and copies it in the passed buffer
 *
 * @code
 * char MsgId[DPWS_MSGID_SIZE];
 *
 * dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
 * @endcode
 *
 * @param buffer pointer to buffer
 * @param size size of buffer, should be at least #DPWS_MSGID_SIZE
 *
 * @return pointer to message id in buffer on succuess, NULL otherwise
 */
char *dpws_header_gen_MessageId (char *buffer, size_t size);

/**
 * function generates SOAP header for an one-way message exchange pattern
 *
 * look at air conditioner src/eventworker.c function deliver_event() for an example
 *
 * @param soap soap handle to generate header
 * @param MessageId message id for one-way message
 * @param To address of endpoint
 * @param Action address of action
 * @param FaultTo address where fault should be send, MUST be NULL for DPWS
 * @param size size of struct SOAP_ENV__Header
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_header_gen_oneway (struct soap *soap, const char *MessageId,
                            const char *To, const char *Action,
                            const char *FaultTo, size_t size);

/**
 * function generates SOAP header for an event delivery
 *
 *
 * @param soap soap handle to generate header
 * @param dpws device
 * @param soap_action_uri address of action
 * @param subscription to fulfill
 * @param size size of struct SOAP_ENV__Header
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
#ifdef DPWS_DEVICE
//int dpws_header_gen_event (struct soap *soap, struct dpws_s *device, char* soap_action_uri, struct ws4d_subscription *subs,size_t size);
#endif

/**
 * function generates SOAP header for a request of a request-response message exchange pattern
 *
 * look at air conditioner src/acs_simpleClient.c function main() for an example
 *
 * @param soap soap handle to generate header
 * @param MessageId message id for request message
 * @param To address of endpoint
 * @param Action address of action
 * @param FaultTo address where fault should be send, MUST be NULL for DPWS
 * @param ReplyTo address where reply should be send, MUST be NULL for DPWS
 * @param size size of struct SOAP_ENV__Header
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_header_gen_request (struct soap *soap, const char *MessageId,
                             const char *To, const char *Action,
                             const char *FaultTo, const char *ReplyTo,
                             size_t size);

/**
 * function generates SOAP header for a response of a request-response message exchange pattern
 *
 * look at air conditioner src/airconditioner.c.c function __acs1__GetStatus() for an example
 *
 * @param soap soap handle to generate header
 * @param MessageId message id for response message
 * @param To address of endpoint
 * @param Action address of action
 * @param RelatesTo Message id of request message
 * @param size size of struct SOAP_ENV__Header
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_header_gen_response (struct soap *soap, const char *MessageId,
                              const char *To, const char *Action,
                              const char *RelatesTo, size_t size);

/** @} */

/**
 * SOAP Fault Generation Functions
 *
 * @addtogroup APIDpwsFault SOAP Fault Generation Functions
 * @ingroup DPWSAPI
 *
 * @{
 */

/*TODO: better docu*/
/**
 * function generates a SOAP sender fault
 *
 * @param soap soap handle to generate fault
 * @param MessageId message id for fault message
 * @param To address of endpoint
 * @param Action address of action
 * @param RelatesTo Message id of request message
 * @param size size of struct SOAP_ENV__Header
 * @param faultstring
 * @param faultdetail
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_sender_fault (struct soap *soap, const char *MessageId,
                       const char *To, const char *Action,
                       const char *RelatesTo, size_t size,
                       const char *faultstring, const char *faultdetail);

/*TODO: better docu*/
/**
 * function generates a SOAP receiver fault
 *
 * @param soap soap handle to generate fault
 * @param MessageId message id for fault message
 * @param To address of endpoint
 * @param Action address of action
 * @param RelatesTo Message id of request message
 * @param size size of struct SOAP_ENV__Header
 * @param faultstring
 * @param faultdetail
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_receiver_fault (struct soap *soap, const char *MessageId,
                         const char *To, const char *Action,
                         const char *RelatesTo, size_t size,
                         const char *faultstring, const char *faultdetail);

/** @} */

#include "ws4d_localizedstring.h"

/**
 * @addtogroup APIClientDiscovery Device discovery Client
 * @ingroup DPWSAPIClient
 *
 *
 * The discovery client api provides functions to initialize and use the
 * discovery implementation in this plugin. There are several ways to initialize
 * the discovery system whether explicit or implicit discovery or both is
 * needed. The discovery system enables a client to probe a network for devices
 * and resolve logical device addresses used in the discovery system to
 * real addresses.
 *
 * @{
 */
#ifdef DPWS_CLIENT

/**
 * function to initialize a dpws plugin handle with a custom target cache
 *
 * @ingroup DPWSAPI
 *
 * @code
 * struct dpws_s device;
 * struct ws4d_tc_int *tc = NULL;
 * int err;
 *
 * err = ws4d_tc_init (tc, NULL);
 * if (err != WS4D_OK)
 *   {
 *     fprintf(stderr, "can't init targetcache\n");
 *     exit(1);
 *   }
 *
 * dpws_init(&device, "ip-address of network device", tc);
 * @endcode
 *
 * @param dpws handle to initialize
 * @param host network interface where plugin is running
 * @param tc taget cache
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */

int dpws_init2 (struct dpws_s *dpws, const char *host,
                struct ws4d_tc_int *tc);

/**
 * function to cerate a service cache interface
 *
 * @param arg arguments for constructor
 *
 * @return returns a service cache interface on success or NULL on failure
 */
struct ws4d_tc_int *dpws_create_tc (void *arg);

/**
 * function to initialize the target cache of an dpws handle
 *
 * @param dpws handle to initialize
 * @param tc target cache created with dpws_create_tc()
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_init_tc (struct dpws_s *dpws, struct ws4d_tc_int *tc);

/**
 * Type of NewTarget callback function
 *
 * This function is called if a new device was discovered.
 *
 * @param soap soap handle that has found a new hosting_targetg_target
 * @param target new hosting_target
 */
typedef void (*dpws_newTarget_cb) (struct soap *, struct ws4d_epr * target);


/**
 * Type of InvalidateTarget callback function
 *
 * This function is called if a device has send a bye message and can be
 * invalidated.
 *
 * @param soap hosting_targetandle that has received hosting_targetihosting_targetget invalidation
 * @param target target to invalidate
 */
typedef void (*dpws_invalidateTarget_cb) (struct soap *,
                                          struct ws4d_epr * target);

struct dpws_discovery_hooks
{
  dpws_newTarget_cb NewTarget;
  dpws_invalidateTarget_cb InvalidateTarget;
};

/**
 * function initializes the implicit discovery client system
 *
 * Example how to get implicit discovery events:
 * @code
 * struct dpws_s dpws;
 * struct soap discovery;
 * int process_timeout = 1000;
 *
 * [...]
 *
 * void new_device(struct soap *soap, struct ws4d_epr *device)
 * {
 *   //New Device
 * }
 *
 * void invalidate_device(struct soap *soap, struct ws4d_epr *device)
 * {
 *   //Device was invalidated
 * }
 *
 * struct wsd_dis_hooks_t discovery_hooks =
 *   { new_device, invalidate_device };
 *
 * [...]
 *
 * soap_init(&discovery);
 * dpws_init(&device, "ip-address of network device");
 *
 * dpws_init_implicit_discovery
 *     (&dpws, &discovery, &discovery_hooks);
 *
 * while (helper_running)
 *   {
 *     err = wsd_process(&discovery, process_timeout);
 *     if ((err != SOAP_EOF) && (err != SOAP_OK) && (err != SOAP_STOP))
 *       {
 *         soap_print_fault(&discovery, stderr);
 *       }
 *   }
 *
 * dpws_done(&device);
 * soap_done(&discovery);
 * @endcode
 *
 * @param dpws dpws plugin handle
 * @param dis_mca soap handle for multicast discovery messages
 * @param dis_hooks hooks to catch incoming discovery messages
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_init_implicit_discovery (struct dpws_s *dpws, struct soap *dis_mca,
                                  struct dpws_discovery_hooks *dis_hooks);


/* TODO: add documentation */
int dpws_init_implicit_discovery2 (struct dpws_s *dpws,
                                   struct ws4d_tc_int *tc,
                                   struct soap *dis_mca,
                                   struct dpws_discovery_hooks *hooks);

/* TODO: add documentation */
int dpws_discovery_process (struct soap *soap, ws4d_time timeout);

/**
 * function to probe a network for devices of a specific type or scope
 *
 * Example how to probe for any device on a network:
 * @code
 * int ret;
 * int timeout = 3000;
 * struct dpws_s dpws;
 * struct ws4d_abs_eprlist results;
 *
 * dpws_init(&device, "ip-address of network device");
 * ws4d_eprlist_init (&results, ws4d_eprllist_init, NULL);
 *
 * ret = dpws_probe (&dpws, NULL, NULL, timeout, 1000, NULL, NULL,
 *                   &results);
 * if (ret != WS4D_OK)
 * {
 *   if (ret == WS4D_TO)
 *   {
 *     //No device matches
 *   }
 *   else
 *   {
 *     //Internal error when probing for devices
 *   }
 * }
 * else
 * {
 *   //work with results here
 * }
 *
 * ws4d_eprlist_done (&results);
 * dpws_done(&device);
 * @endcode
 *
 * Example how to probe for devices with specific types on a network:
 * @code
 * int ret;
 * int timeout = 3000;
 * struct dpws_s dpws;
 * struct ws4d_abs_eprlist results;
 * ws4d_qnamelist type_list;
 *
 * dpws_init(&device, "ip-address of network device");
 * ws4d_eprlist_init (&results, ws4d_eprllist_init, NULL);
 *
 * ws4d_qnamelist_init (&type_list);
 * ws4d_qnamelist_addstring ("\"namespace\":type", &type_list, &alist);
 *
 * ret = dpws_probe (&dpws, &type_list, NULL, timeout, 1000, NULL, NULL,
 *                   &results);
 * if (ret != WS4D_OK)
 * {
 *   if (ret == WS4D_TO)
 *   {
 *     //No device matches
 *   }
 *   else
 *   {
 *     //Internal error when probing for devices
 *   }
 * }
 * else
 * {
 *   //work with results here
 * }
 *
 * ws4d_qnamelist_done (&type_list);
 * ws4d_eprlist_done (&results);
 * dpws_done(&device);
 * @endcode
 *
 * @param dpws dpws plugin handle
 * @param Types list of device types
 * @param Scope device scope
 * @param timeout time to wait for response messages
 * @param backlog
 * @param hooks discovery hooks for callback
 * @param tc target cache created with dpws_create_tc()
 * @param result buffer for resulting endpoint list
 *
 * @return WS4D_OK on success, WS4D_TO on timeout without devices or WS4D_ERR on failure
 */
int dpws_probe (struct dpws_s *dpws, ws4d_qnamelist * Types,
                const char *Scope, ws4d_time timeout, int backlog,
                struct dpws_discovery_hooks *hooks, struct ws4d_tc_int *tc,
                struct ws4d_abs_eprlist *result);

/**
 * function to directly probe a device for a specific type or scope
 *
 * Example how to dprobe a device for a specific type:
 * @code
 * int ret;
 * int timeout = 3000;
 * struct dpws_s dpws;
 * struct ws4d_epr *device = NULL;
 * ws4d_qnamelist type_list;
 *
 * dpws_init(&device, "ip-address of network device");
 *
 * device = ws4d_epr_alloc (1, &alist);
 * ws4d_epr_set_Addrs (device, "urn:uuid:[uuid]");
 *
 * ws4d_qnamelist_init (&type_list);
 * ws4d_qnamelist_addstring ("\"namespace\":type", &type_list, &alist);
 *
 * ret = dpws_dprobe (&dpws, device, &type_list, NULL, NULL, timeout);
 * if (res != SOAP_OK)
 *  {
 *    if (res == SOAP_EOF)
 *      {
 *        //Device doesn't match
 *      }
 *    else
 *      {
 *        //Device doesn't respond
 *      }
 *  }
 * else
 *  {
 *    //Device matches
 *  }
 *
 * ws4d_epr_free(1, device);
 * ws4d_qnamelist_done (&type_list);
 * dpws_done(&device);
 * @endcode
 *
 * @param dpws dpws plugin handle
 * @param epr device to probe
 * @param Types list of device types
 * @param Scope device scope
 * @param tc target cache created with dpws_create_tc()
 * @param timeout time to wait for response messages
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */

int dpws_dprobe (struct dpws_s *dpws, struct ws4d_epr *epr,
                 ws4d_qnamelist * Types, const char *Scope,
                 struct ws4d_tc_int *tc, ws4d_time timeout);

/**
 * function to reslove a logical device address to a real address
 *
 * Example how to resolve a device uuid:
 * @code
 * char *XAddrs;
 * int timeout = 3000;
 * struct dpws_s dpws;
 * struct ws4d_epr *device = NULL;
 *
 * dpws_init(&device, "ip-address of network device");
 *
 * device = ws4d_epr_alloc (1, &alist);
 * ws4d_epr_set_Addrs (device, "urn:uuid:[uuid]");
 *
 * XAddrs = (char *) dpws_resolve_addr (&dpws, device, NULL, timeout);
 * if (!XAddrs)
 *  {
 *    //Device can't be resolved
 *  }
 *
 * ws4d_epr_free(1, device);
 * dpws_done(&device);
 * @endcode
 *
 * @param dpws dpws plugin handle
 * @param epr endpoint to resolve
 * @param tc target cache created with dpws_create_tc()
 * @param timeout time to wait for response messages
 *
 * @return point to address string on success or NULL on fialure
 */
const char *dpws_resolve_addr (struct dpws_s *dpws, struct ws4d_epr *epr,
                               struct ws4d_tc_int *tc, ws4d_time timeout);

/**
 * function to check if a device is secured
 *
 * Example how to resolve a device uuid:
 * @code
 * char *XAddrs;
 * int timeout = 3000;
 * struct dpws_s dpws;
 * struct ws4d_epr *device = NULL;
 *
 * dpws_init(&device, "ip-address of network device");
 *
 * device = ws4d_epr_alloc (1, &alist);
 * ws4d_epr_set_Addrs (device, "urn:uuid:[uuid]");
 *
 * XAddrs = (char *) dpws_resolve_addr (&dpws, device, NULL, timeout);
 * if (!XAddrs)
 *  {
 *    //Device can't be resolved
 *  }
 *
 * if (dpws_device_issecured(XAddrs))
 *  {
 *    printf("device is secured");
 *  }
 * else
 *  {
 *    printf("device is NOT secured!");
 *  }
 *
 * ws4d_epr_free(1, device);
 * dpws_done(&device);
 * @endcode
 *
 * @param dpws dpws plugin handle
 * @param epr endpoint to resolve
 * @param tc target cache created with dpws_create_tc()
 * @param timeout time to wait for response messages
 *
 * @return point to address string on success or NULL on fialure
 */
int dpws_device_issecured (const char *XAddrs);
#endif
/** @} */

#include "ws4d_device_description.h"

/**
 * @addtogroup APIClientDescription Device and service description Client
 * @ingroup DPWSAPIClient
 *
 *
 * The device and service description client API offers functions to retreive
 * description metadata from a device or a service hosted on a device.
 *
 * @{
 */

#if defined(DEVPROF_2009_01)
#define DPWS_MEX_DEVICE "http://docs.oasis-open.org/ws-dd/ns/dpws/2009/01/ThisDevice"
#define DPWS_MEX_MODEL "http://docs.oasis-open.org/ws-dd/ns/dpws/2009/01/ThisModel"
#define DPWS_MEX_RELATIONSHIP "http://docs.oasis-open.org/ws-dd/ns/dpws/2009/01/Relationship"
#define DPWS_MEX_RELATIONSHIP_TYPE "http://docs.oasis-open.org/ws-dd/ns/dpws/2009/01/host"
#define DPWS_MEX_WSDL "http://schemas.xmlsoap.org/wsdl"
#else
#define DPWS_MEX_DEVICE "http://schemas.xmlsoap.org/ws/2006/02/devprof/ThisDevice"
#define DPWS_MEX_MODEL "http://schemas.xmlsoap.org/ws/2006/02/devprof/ThisModel"
#define DPWS_MEX_RELATIONSHIP "http://schemas.xmlsoap.org/ws/2006/02/devprof/Relationship"
#define DPWS_MEX_RELATIONSHIP_TYPE "http://schemas.xmlsoap.org/ws/2006/02/devprof/host"
#define DPWS_MEX_WSDL "http://schemas.xmlsoap.org/wsdl"
#endif
#ifdef DPWS_CLIENT

/**
 * function retreives the metadata from a device
 *
 * @param dpws dpws plugin handle
 * @param soap soap handle
 * @param device endpoint reference of a device
 * @param metadata
 * @param timeout timeout in milli seconds
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_metadata_get (struct dpws_s *dpws, struct soap *soap,
                       struct ws4d_epr *device,
                       void *metadata, ws4d_time timeout);

/* TODO: add documentation */
int dpws_metadata_getThisDevice (void *metadata,
                                 ws4d_alloc_list * alist,
                                 struct ws4d_thisDevice *result);

/* TODO: add documentation */
int dpws_metadata_getThisModel (void *metadata,
                                ws4d_alloc_list * alist,
                                struct ws4d_thisModel *result);

/* TODO: add documentation */
int dpws_metadata_getservices_bytype (struct dpws_s *dpws,
                                      struct ws4d_epr *device,
                                      ws4d_qnamelist * types,
                                      void *metadata,
                                      struct ws4d_abs_eprlist *result);

/* TODO: add documentation */
int dpws_metadata_getservices (struct dpws_s *dpws, struct ws4d_epr *device,
                               void *metadata,
                               struct ws4d_abs_eprlist *result);

/* TODO: add documentation */
struct ws4d_epr *dpws_servicecache_getByAddr (struct ws4d_epr *device,
                                              const char *Addr);

/* TODO: add documentation */
int dpws_servicecache_matchesTypes (struct ws4d_epr *service,
                                    ws4d_qnamelist * types);

/* TODO: add documentation */
int dpws_find_cached_service (struct dpws_s *dpws, struct ws4d_epr *device,
                              ws4d_qnamelist * types,
                              struct ws4d_abs_eprlist *result);

/**
 * function findes services of specific type int a devices relationship metadata
 *
 * @param dpws dpws plugin handle
 * @param device endpoint reference of a device
 * @param types service types to look for
 * @param timeout time ot wait for incoming messages
 * @param result buffer for resulting service endpoint references
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_find_services (struct dpws_s *dpws, struct ws4d_epr *device,
                        ws4d_qnamelist * types, ws4d_time timeout,
                        struct ws4d_abs_eprlist *result);

/* TODO: add documentation */
int dpws_find_service (struct dpws_s *dpws, struct ws4d_epr *device,
                       const char *id, ws4d_time timeout,
                       struct ws4d_epr *result);

struct dpws_service_plugin_s
{
  struct ws4d_epr *epr;
  struct ws4d_list_node hosted_services;
  ws4d_alloc_list alist;
};

/* TODO: add documentation */
ws4d_qnamelist *dpws_service_gettypelist (struct ws4d_epr *epr);

/* TODO: add documentation */
char *dpws_service_gettypestring (struct ws4d_epr *epr,
                                  ws4d_alloc_list * alist);

/* TODO: add documentation */
char *dpws_service_getid (struct ws4d_epr *epr, ws4d_alloc_list * alist);

#endif

/** @} */

/**
 * @addtogroup APIHostedService Service and device description
 * @ingroup APIDevice
 *
 * Hosted services are services a device provides for the network. The
 * following functions are use to register such services with the dpws plugin.
 *
 * @{
 */

/**
 * function to create a dpws service structure
 *
 * @param device dpws plugin handle
 * @param serviceid id of service to create
 *
 * @return pointer to service structure on success or NULL on failure
 */
struct ws4d_epr *dpws_service_init (struct dpws_s *device,
                                    const char *serviceid);

/**
 * function to destroy a dpws service structure
 *
 * @param dpws dpws plugin handle
 * @param service service structure
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_service_done (struct dpws_s *dpws, struct ws4d_epr *service);

/**
 * function to bind a dpws service to an specific address
 *
 * @param device dpws plugin handle
 * @param service service structure
 * @param handle soap handle of the service
 * @param uri uri of the hosted service
 * @param size size of the uri string
 * @param backlog
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_service_bind (struct dpws_s *device, struct ws4d_epr *service,
                       struct soap *handle, char *uri, size_t size,
                       int backlog);

/**
 * function to add a service type as ws4d_qname
 *
 * @param service service structure
 * @param type type to add to service
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_service_add_type (struct ws4d_epr *service, struct ws4d_qname *type);

/**
 * function to add a service type as string
 *
 * @param service service structure
 * @param types type to add to service
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_service_add_typestring (struct ws4d_epr *service, const char *types);

/**
 * function to set the wsdl
 *
 * @param service service structure
 * @param wsdl path to wsdl of service to add
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_service_set_wsdl (struct ws4d_epr *service, const char *wsdl);

/**
 * function to set extended metadata elements of an service
 *
 * @param service service structure
 * @param ext xml element to add to service metadata
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_service_set_ext (struct ws4d_epr *service, const char *ext);

/**
 * function to register a hosted service with the hosting_handle service to activate service announcement
 *
 * @param device dpws plugin handle
 * @param service service structure to add
 * @param uri uri of the hosted service
 * @param size size of the uri string
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_add_hosted_service (struct dpws_s *device,
                             struct ws4d_epr *service, char *uri,
                             size_t size);

/**
 * function to unregister a hosted service to deactivate service announcement
 *
 * @param device dpws plugin handle
 * @param service service structure
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_del_hosted_service (struct dpws_s *device, struct ws4d_epr *service);

/**
 * function to get a dpws service structure by service id
 *
 * @param device dpws plugin handle
 * @param serviceid id of service
 *
 * @return pointer to service structure on success or NULL on failure
 */
struct ws4d_epr *dpws_get_hosted_service (struct dpws_s *device,
                                          const char *serviceid);

/* TODO: add documentation */
struct ws4d_thisModel *dpws_change_thismodel (struct dpws_s *dpws);

/* TODO: add documentation */
int dpws_updatemetadata_Model (struct dpws_s *dpws);

struct wsdl_s
{
  const char *name;
  const char *content;
};

/* TODO: add documentation */
struct ws4d_thisDevice *dpws_change_thisdevice (struct dpws_s *dpws);

/* TODO: add documentation */
int dpws_updatemetadata_Device (struct dpws_s *dpws);

/** @} */

#include "ws4d_subscription.h"
#include "ws4d_subsmanproxy.h"

/**
 * function to add the wse:Identifier to the SOAP header
 *
 * look at air conditioner src/eventworker.c function deliver_event() for an example
 *
 * @param soap soap handle to generate header
 * @param dpws dpws plugin handle
 * @param subs subcription to extract the identifier
 *
 * @return SOAP_OK on success or SOAP_ERR on failure
 */
int dpws_header_gen_wseIdentifier (struct soap *soap, struct dpws_s *device,
                                   struct ws4d_subscription *subs);

/* TODO: add documentation */
struct ws4d_delivery_type *dpws_gen_delivery_push (struct soap *soap,
                                                   const char *notifyTo);

/* TODO: add documentation */
struct ws4d_filter_type *dpws_gen_filter_action (struct soap *soap,
                                                 const char *action_list);

/* TODO: add documentation */
const char *dpws_subscribe (struct soap *soap,
                            struct ws4d_epr *service,
                            const char *endToAddress,
                            ws4d_time * expires,
                            struct ws4d_delivery_type *delivery,
                            struct ws4d_filter_type *filter);

/* TODO: add documentation */
int dpws_subs_unsubscribe (struct soap *soap, struct ws4d_epr *service,
                           const char *id);

/* TODO: add documentation */
int dpws_subs_unsubscribe2 (struct soap *soap, const char *subsman,
                            const char *id);

/* TODO: add documentation */
int dpws_subs_renew (struct soap *soap, struct ws4d_epr *service,
                     const char *id, ws4d_time * expires);

/* TODO: add documentation */
int dpws_subs_renew2 (struct soap *soap, const char *subsman, const char *id,
                      ws4d_time * expires);

/* TODO: add documentation */
int dpws_subs_get_status (struct soap *soap, struct ws4d_epr *service,
                          const char *id, ws4d_time * expires);

/* TODO: add documentation */
int dpws_subs_get_status2 (struct soap *soap, const char *subsman,
                           const char *id, ws4d_time * expires);

/* TODO: add documentation */
const char *dpws_subs_get_subsman (struct ws4d_epr *service, const char *id);

#ifdef DPWS_DEVICE

/* TODO: add documentation */
struct ws4d_subsmanager *dpws_get_subsman (struct dpws_s *device);

/* TODO: add documentation */
int dpws_activate_eventsource (struct dpws_s *device, struct soap *evsrc);

/* TODO: add documentation */
struct ws4d_subscription *dpws_subsm_get_first_by_action (struct dpws_s
                                                          *device,
                                                          const char *action);

/* TODO: add documentation */
struct ws4d_subscription *dpws_subsm_get_next_by_action (struct dpws_s
                                                         *device,
                                                         struct
                                                         ws4d_subscription
                                                         *cur,
                                                         const char *action);
/* TODO: add documentation */
#define dpws_for_each_subs(cur, n, device, ac_string) \
  for (cur = dpws_subsm_get_first_by_action(device, ac_string), \
       n = dpws_subsm_get_next_by_action (device, cur, ac_string); \
       cur; \
       cur = n, \
       n = dpws_subsm_get_next_by_action (device, n, ac_string))

/* TODO: add documentation */
char *dpws_subsm_get_deliveryPush_address (struct dpws_s *device,
                                           struct ws4d_subscription *subs);

/* TODO: add documentation */
char *dpws_subsm_get_deliveryPush_identifier (struct dpws_s *device,
                                              struct ws4d_subscription *subs);

#define DPWS_SUBS_END_FAILURE wse_subsend_deliveryfailure
#define DPWS_SUBS_END_SHUTDOWN wse_subsend_shutdown
#define DPWS_SUBS_END_CANCELING wse_subsend_canceling

/* TODO: add documentation */
int dpws_end_subscription (struct dpws_s *device,
                           struct ws4d_subscription *subs, const char *status,
                           char *reason);

/* TODO: add documentation */
int dpws_check_subscriptions (struct dpws_s *device);

#endif

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /* STD_DPWS_H */
