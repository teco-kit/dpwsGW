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

#ifndef WSDISCOVERY_H_
#define WSDISCOVERY_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#include "soap_misc.h"
#include "ws4d_abstract_eprlist.h"

/**
 * gSoap WS-Discovery Plugin API
 *
 * @addtogroup DISCOVERY_API Discovery-API
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

int wsd_connect (struct soap *soap, const char *endpoint,
                 const char *host, int port);

/**
 * Discovery Hooks API
 *
 * Discovery Hooks are used as callback mechanism in this module to call
 * userspecified functions on certain events.
 *
 * @addtogroup APIHooks Discovery Hooks API
 * @ingroup DISCOVERY_API
 *
 * @{
 */
#ifdef WSD_CLIENT

#ifdef WSD_LL_CB
/**
 * Type of Hello message callback function
 *
 * This function is called if a discovery hello message has received.
 *
 * @param soap soap handle that has received the message
 * @param req message
 */
typedef void (*Hello_cb) (struct soap *, struct wsd__HelloType * req);


/**
 * Type of Bye message callback function
 *
 * This function is called if a discovery bye message has received.
 *
 * @param soap soap handle that has received the message
 * @param req message
 */
typedef void (*Bye_cb) (struct soap *, struct wsd__ByeType * req);


/**
 * Type of ProbeMatches message callback function
 *
 * This function is called if a discovery probematches message has received.
 *
 * @param soap soap handle that has received the message
 * @param res message
 */
typedef void
  (*ProbeMatches_cb) (struct soap *, struct wsd__ProbeMatchesType * res);


/**
 * Type of ResolveMatches message callback function
 *
 * This function is called if a discovery resolvematches message has received.
 *
 * @param soap soap handle that has received the message
 * @param req message
 */
typedef void
  (*ResolveMatches_cb) (struct soap *,
                        struct wsd__ResolveMatchesType * ResolveMatch);
#endif
#endif

#ifdef WSD_TARGET
#ifdef WSD_LL_CB
/**
 * Type of Probe message callback function
 *
 * This function is called if a discovery probe message has received.
 *
 * @param soap soap handle that has received the message
 * @param req message
 */
typedef void (*Probe_cb) (struct soap *, void *req);


/**
 * Type of Resolve message callback function
 *
 * This function is called if a discovery resolve message has received.
 *
 * @param soap soap handle that has received the message
 * @param req message
 */
typedef void (*Resolve_cb) (struct soap *, void *req);
#endif
#endif

#ifdef WSD_CLIENT

/**
 * Type of NewTarget callback function
 *
 * This function is called if a new device was discovered.
 *
 * @param soap soap handle that has found a new hosting_targetg_target
 *hosting_targetm hoshosting_targetarget new hosting_target
 */
typedef void (*NewTarget_cb) (struct soap *, struct ws4d_epr * target);


/**
 * Type of InvalidateTarget callback function
 *
 * This function is called if a device has send a bye message and can be
 * invalidated.
 *
 * @param soap hosting_targetandle that has received hosting_targetihosting_targetget invalidation
 * @param hosting_target hosting_target to invalidate
 */
typedef void (*InvalidateTarget_cb) (struct soap *, struct ws4d_epr * target);
#endif



/**
 * Structure to register callback functions for discovery specific multicast messages
 */
struct wsd_mca_hooks_t
{
#ifdef WSD_LL_CB
#ifdef WSD_CLIENT
  Hello_cb Hello;
  Bye_cb Bye;
#endif
#ifdef WSD_TARGET
  Probe_cb Probe;
  Resolve_cb Resolve;
#endif
#else
  void *_;
#endif
};



/**
 * Structure to register callback functions for discovery specific unicast messages
 */
#ifdef WSD_CLIENT
struct wsd_uni_hooks_t
{
#ifdef WSD_LL_CB
  ProbeMatches_cb ProbeMatches;
  ResolveMatches_cb ResolveMatches;
#else
  void *_;
#endif
};


/**
 * Structure to register callback functions for new and invalid targets
 */
struct wsd_dis_hooks_t
{
  NewTarget_cb NewTarget;
  InvalidateTarget_cb InvalidateTarget;
};
#endif
/** @} */

/**
 * gSOAP Plugins
 *
 * @addtogroup APIPlugins gSOAP Plugins
 * @ingroup DISCOVERY_API
 *
 * @{
 */

extern const char *wsd_mca_plugin_id;

#ifdef WSD_CLIENT
extern const char *wsd_uni_plugin_id;
#endif

/** @} */

/**
 * Target Side API
 *
 * @addtogroup APITarget Target Side API
 * @ingroup DISCOVERY_API
 *
 * @{
 */

#ifdef WSD_TARGET

/**
 * Discovery Target
 *
 *
 *
 * @addtogroup APITargetTarget Discovery Target
 * @ingroup APITarget
 *
 * @{
 */

#include "ws4d_targetservice.h"

int
wsd_target_init (struct soap *mca, char *netdev, int backlog,
                 struct ws4d_targetservice *ts,
                 struct wsd_mca_hooks_t *hooks);

struct ws4d_epr *wsd_target_add (struct soap *soap, const char *laddr,
                                 const char *paddr, const char *SequenceId);


struct ws4d_epr *wsd_target_get_byAddr (struct soap *soap, const char *laddr);

int
wsd_target_set_namespaces (struct ws4d_epr *target, struct soap *soap,
                           const struct Namespace *namespaces,
                           ws4d_alloc_list * alist);
/** @} */

/**
 * @addtogroup APITargetAdvertisment Advertisment Functions
 * @ingroup APITarget
 *
 * @{
 */

int wsd_send_Hello (struct soap *soap, const char *MsgId,
                    struct ws4d_epr *target,
                    const char *types, ws4d_alloc_list * alist);

int wsd_send_Bye (struct soap *soap, const char *MsgId,
                  struct ws4d_epr *target, ws4d_alloc_list * alist);
/** @} */
/**
 * @addtogroup APITargetProcessing Processing Functions
 * @ingroup APITarget
 *
 * @{
 */
int
wsd_process_probe (struct soap *soap, struct wsd__ProbeType *req,
                   struct ws4d_abs_eprlist *matching_targets);


char *wsd_gen_response_addr (void *in_addr, const char *path,
                             ws4d_alloc_list * alist);

int wsd_gen_ProbeMatches (struct soap *soap, struct ws4d_abs_eprlist *matches,
                          struct wsd__ProbeMatchesType *wsd__ProbeMatches);

int
wsd_gen_ProbeMatches_header (struct soap *soap, const char *MsgId,
                             const char *to, struct ws4d_abs_eprlist *matches,
                             struct soap *req,
                             const struct Namespace *namespaces,
                             ws4d_alloc_list * alist);

int
wsd_send_async_ProbeMatches (struct soap *soap, const char *MsgId,
                             const char *to, struct ws4d_abs_eprlist *matches,
                             struct soap *req, ws4d_alloc_list * alist,
                             struct wsd__ProbeMatchesType *wsd__ProbeMatches);

int wsd_process_resolve (struct soap *soap,
                         struct wsd__ResolveType *req,
                         struct ws4d_epr **target);

int wsd_send_ResolveMatches (struct soap *soap,
                             const char *MsgId,
                             const char *to,
                             struct ws4d_epr *target, struct soap *req,
                             ws4d_alloc_list * alist);
/** @} */
/** @} */
#endif
/**
 * Client Side API
 *
 * @addtogroup DISCOVERYAPIClient Client Side API
 * @ingroup DISCOVERY_API
 *
 * @{
 */

/**
 * @addtogroup APIClientProcessing Processing Functions
 * @ingroup DISCOVERYAPIClient
 *
 * TODO: Write more infos here
 *
 * @{
 */
#ifdef WSD_CLIENT
/**
 * Function to process hello messages send by an hosting_target service to anounce
 * its appearance on the  network or change of its metadata. Should be
 * called in handler for hello messages.
 *
 * @param soap soap handle registered with wsd_inihosting_targetnt()
 * @param req Message to process
 *
 * @return returnd SOAP_OK if message could be processed.
 */
int wsd_process_hello (struct soap *soap, struct wsd__HelloType *req);

/**
 * Function to process bye messages send by an hosting_target service to anounce
 * its removal from network. Should be called in handler for bye messages.
 *
 * @param soap soap handle registered with wsd_init_client()
 * @param req Message to process
 *
 * @return returnd SOAP_OK if message could be processed.
 */
int wsd_process_bye (struct soap *soap, struct wsd__ByeType *req);

/**
 * Function processes a ProbeMatches message. Should be called in handler
 * for the ProbeMatches operation.
 *
 * @param soap soap handle registered with wsd_init_client()
 * @param req Message to process
 *
 * @return returnd SOAP_OK if message could be processed.
 */
int wsd_process_probematches (struct soap *soap,
                              struct wsd__ProbeMatchesType *req);

/**
 * Function processes a ResolveMatches message. Should be called in handler
 * for the ResolveMatches operation.
 *
 * @param soap soap handle registered with wsd_init_client()
 * @param req Message to process
 *
 * @return returnd SOAP_OK if message could be processed.
 */
int wsd_process_resolvematches (struct soap *soap,
                                struct wsd__ResolveMatchesType *req);

/** @} */
#endif
#ifdef WSD_CLIENT

#include "ws4d_targetcache.h"

/**
 * Implicit Device Discovery
 *
 *
 *
 * @addtogroup APIImplicitDiscovery Implicit Device Discovery
 * @ingroup DISCOVERYAPIClient
 *
 * @{
 */
/**
 * Function to initialize the client side of the implicite discovery
 * mechanism. Hello and Bye Events of Devices can be received by specifying
 * hooks with the discovery hooks parameter.
 *
 * @param mca soap handle to listen for discovery multicast messages
 * @param dis_cache target cache for discovery
 * @param netdev character string of ip address of network device where the plugin should operate on
 * @param backlog maximum length of the queue of pending messages
 * @param mca_hooks structure with hooks for incoming discovery multicast messages (mandatory)
 * @param dis_hooks structure with hooks for NewTarget and InvalidateTarget events (mandatory)
 *
 * @return returns SOAP_OK on success or a failure otherwise.
 */
int
wsd_soap_init_implicite (struct soap *mca, struct ws4d_tc_int *dis_cache,
                         const char *netdev, int backlog,
                         struct wsd_mca_hooks_t *mca_hooks,
                         struct wsd_dis_hooks_t *dis_hooks);
/** @} */
/**
 * Explicit Device Discovery
 *
 *
 *
 * @addtogroup APIExplicitDiscovery Explicit Device Discovery
 * @ingroup DISCOVERYAPIClient
 *
 * @{
 */
/**
 * Function to initialize the client side of the discovery plugin
 * for a soap handle.
 *
 * @param uni soap handle to initialize as discovery client
 * @param dis_cache cache for storing discovered devices
 * @param netdev character string of ip address of network device where the plugin should operate on
 * @param backlog maximum length of the queue of pending messages
 * @param uni_hooks structure with hooks for incoming discovery unicast messages (mandatory)
 * @param dis_hooks structure with hooks for NewTarget and InvalidateTarget events (mandatory)
 *
 * @return returns SOAP_OK on success or a failure otherwise.
 */
int
wsd_soap_init_explicit (struct soap *uni,
                        struct ws4d_tc_int *dis_cache,
                        const char *netdev, int backlog,
                        struct wsd_uni_hooks_t *uni_hooks,
                        struct wsd_dis_hooks_t *dis_hooks);

int
wsd_soap_init_directed (struct soap *uni,
                        struct ws4d_tc_int *dis_cache,
                        const char *netdev, int backlog,
                        struct wsd_uni_hooks_t *uni_hooks,
                        struct wsd_dis_hooks_t *dis_hooks);


int wsd_probe_async (struct soap *soap, const char *MsgId, const char *To,
                     ws4d_qnamelist * Types, const char *Scope,
                     ws4d_time timeout, ws4d_alloc_list * alist);


int wsd_is_physical_addr (struct soap *soap, const char *Address);

int wsd_resolve_async (struct soap *soap, const char *MsgId,
                       const char *Address, ws4d_time timeout,
                       ws4d_alloc_list * alist);

int wsd_process (struct soap *soap, ws4d_time timeout);

int wsd_process_onemessage (struct soap *soap, ws4d_time timeout);

/** @} */
/** @} */
#endif
/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /* WSDISCOVERY_H_ */
