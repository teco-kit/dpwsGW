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

#include <sys/types.h>
#include "stdsoap2.h"
#include "soap_misc.h"

#include "target.nsmap"
#include "ws-addressing.h"

#define WSD_LL_CB
#include "ws-discovery.h"

#ifndef DPWS_HOSTED_SERVICE

#include "ws4d_epr.h"
#include "ws4d_target.h"

#include "ws-discovery_constants.h"

/**
 * @addtogroup Internals Internals
 *
 * @{
 */

#ifndef SO_REUSEPORT
#define SO_REUSEPORT SO_REUSEADDR
#endif

/**
 * physikal address of discovery multicast service
 */
int wsd_bind_udp (struct soap *soap, const char *netdev, short int port,
                  int backlog);

int wsd_bind_multicastudp (struct soap *soap, const char *host,
                           const char *group, short int port, int backlog);

/**
 * Function to bind a soap handle to an udp unicast socket
 *
 * @param soap soap handle to bind
 * @param netdev character string of ip address of network device where the plugin should operate on
 * @param port port where socket should listen for incoming traffic
 * @param backlog maximum length of the queue of pending traffic
 *
 * @return SOAP_OK on succes, a failure otherwise
 */

int
wsd_bind_udp (struct soap *soap, const char *netdev, short int port,
              int backlog)
{
  SOAP_SOCKET fd;

  /* test parameters */
  if (!soap || !netdev)
    return SOAP_ERR;

#ifdef MSG_DONTWAIT
  soap->socket_flags = MSG_DONTWAIT;
#endif

  soap_set_namespaces (soap, target_namespaces);
  soap_set_mode (soap, SOAP_IO_UDP);

  fd = soap_bind (soap, netdev, port, backlog);

  if (fd == SOAP_INVALID_SOCKET)
    {
      /* error */
      soap_print_fault (soap, stderr);
      return SOAP_INVALID_SOCKET;
    }
  return SOAP_OK;
}

/**
 * Function to bind a soap handle to an discovery multicast socket
 *
 * @param soap soap handle to bind
 * @param host character string of ip address of network device where the plugin should operate on
 * @param group character string of ip address of ip multicast group where the handle should be subscribed
 * @param port port where socket should listen for incoming traffic
 * @param backlog maximum length of the queue of pending traffic
 *
 * @return SOAP_OK on succes or SOAP_ERR otherwise
 */

#ifndef WITH_NOIO
int
wsd_bind_multicastudp (struct soap *soap, const char *host,
                       const char *group, short int port, int backlog)
{
  SOAP_SOCKET fd;
  int err = 0;
#ifdef WITH_IPV6
  struct ipv6_mreq mreq6;
#endif
  struct ip_mreq mreq;

  /* test parameters */
  if (!soap || !host || !group)
    return SOAP_ERR;

  /* to let several instances of discovery work on the same */
  /* discovery multicast socket */
  soap->bind_flags = SO_REUSEPORT;

  /* same procedure as with unicast socket */
  err = wsd_bind_udp (soap,
#ifdef WITH_IPV6
                      ws4d_is_ipv4 (host) ? "0.0.0.0" : "::",
#else
                      "0.0.0.0",
#endif
                      port, backlog);
  if (err != SOAP_OK)
    return err;

  fd = soap->master;

  /* add to ws discovery multicast group */
#ifdef WITH_IPV6
  if (ws4d_is_ipv4 (host))
    {
#endif
      memset (&mreq, 0, sizeof (mreq));
      ws4d_inet_pton (AF_INET, group, &mreq.imr_multiaddr.s_addr);
      ws4d_inet_pton (AF_INET, host, &mreq.imr_interface.s_addr);
#ifdef WITH_IPV6
    }
  else
    {
      memset (&mreq6, 0, sizeof (mreq6));
      ws4d_inet_pton (AF_INET6, group, &mreq6.ipv6mr_multiaddr.s6_addr);

      /* TODO: specify interface id here */
      mreq6.ipv6mr_interface = 0;
    }
#endif

#ifdef WITH_IPV6
  if (ws4d_is_ipv4 (host))
    {
#endif
      if (setsockopt (fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mreq,
                      sizeof (mreq)) < 0)
        {
          soap->fclosesocket (soap, (SOAP_SOCKET) fd);
          return SOAP_PLUGIN_ERROR;
        }
#ifdef WITH_IPV6
    }
  else
    {
      if (setsockopt (fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, (void *) &mreq6,
                      sizeof (mreq6)) < 0)
        {
          perror ("setsockopt in wsd_bind_multicastudp()");
          soap->fclosesocket (soap, (SOAP_SOCKET) fd);
          return SOAP_PLUGIN_ERROR;
        }
    }
#endif

  return SOAP_OK;
}
#endif

static int wsd_disconnect (struct soap *soap);

/**
 * Function is used by gSoap to close sockets. Udp sockets used in the
 * Plugin should not be closed but reused. So this function only
 * closes non-udp sockets.
 *
 * @param soap handle of socket
 *
 * @return 0 on success -1 on failure
 */

static int
wsd_disconnect (struct soap *soap)
{
  if ((soap->imode & SOAP_IO_UDP) && soap_valid_socket (soap->master))
    return 0;

  return -1;
}

/**
 * Function is used by gSoap to establish new connections for sending
 * a message. In the case of udp sockets the old socket should be
 * reused. This functions opens new sockets for non-udp sockets.
 *
 * @param soap handle of socket
 * @param endpoint endpoint to connect to
 * @param host from where the connection is established
 * @param port from where the connection is established
 *
 * @return a valid socket on succes or SOAP_INVALID_SOCKET on failure
 */

int
wsd_connect (struct soap *soap, const char *endpoint,
             const char *host, int port)
{
  WS4D_UNUSED_PARAM (endpoint);

  if ((soap->imode & SOAP_IO_UDP) && soap_valid_socket (soap->master))
    {
#ifdef WITH_IPV6
      struct sockaddr_in6 *in = (struct sockaddr_in6 *) soap_getpeer (soap);
#else
      struct sockaddr_in *in = (struct sockaddr_in *) soap_getpeer (soap);
#endif

#ifdef WITH_NOIO
      soap->peerlen = soap_getpeerlen (soap);
#else
      soap->peerlen = sizeof (soap->peer);
#endif
      memset ((void *) in, 0, soap->peerlen);

#ifdef WITH_IPV6
      in->sin6_family = AF_INET6;
      if (ws4d_inet_pton (AF_INET6, host, &in->sin6_addr) != 1)
        {
          return SOAP_INVALID_SOCKET;
        }
      in->sin6_port = htons ((short) port);
#else
      in->sin_family = AF_INET;
      if (ws4d_inet_pton (AF_INET, host, &in->sin_addr) != 1)
        {
          return SOAP_INVALID_SOCKET;
        }
      in->sin_port = htons ((short) port);
#endif

      return soap->master;
    }

  return SOAP_INVALID_SOCKET;
}


#include "ws4d_targetservice.h"

struct wsd_mca_plugin_data
{
#ifdef WSD_TARGET
  struct ws4d_targetservice *targetservice;
#endif

#ifdef WSD_CLIENT
  struct ws4d_tc_int *target_cache;
  struct wsd_dis_hooks_t *dis_hooks;
#endif

  struct wsd_mca_hooks_t *mca_hooks;

  ws4d_alloc_list alist;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
};

#define WSD_MCA_PLUGIN_ID "WS-Discovery-Multicast-PLUGIN-0.1"
const char *wsd_mca_plugin_id = WSD_MCA_PLUGIN_ID;

INLINE struct wsd_mca_plugin_data *wsd_mca_get_plugindata (struct soap *soap);

int wsd_mca_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg);

static void wsd_mca_plugin_delete (struct soap *soap, struct soap_plugin *p);

int wsd_mca_register_handle (struct soap *soap, const char *netdev,
                             int backlog, struct wsd_mca_hooks_t *hooks);

/**
 * Function returns the plugin data of a discovery multicast soap handle
 * registered with wsd_target_init() before.
 *
 * @param soap soap handle registered with wsd_target_init()
 *
 * @return pointer to discovery multicast plugin data structure
 */

INLINE struct wsd_mca_plugin_data *
wsd_mca_get_plugindata (struct soap *soap)
{
  return (struct wsd_mca_plugin_data *) soap_lookup_plugin (soap,
                                                            WSD_MCA_PLUGIN_ID);
}

/**
 * Function to destroy discovery multicast plugin data.
 * Function is called by soap_done().
 *
 * @param soap soap handle
 * @param p pointer to soap plugin structure
 *
 */

static void
wsd_mca_plugin_delete (struct soap *soap, struct soap_plugin *p)
{
  WS4D_UNUSED_PARAM (soap);

  /* eek we leek ! */

  /* clean up address mappings */
  /* clean up types */
  /* clean up scopes */
  /* clean up st_mappings */

  /* free plugin data structure */
  ws4d_free (p->data);
}

/**
 * Function to initialize discovery multicast plugin data.
 * Function is called by soap_register_plugin().
 *
 * @param soap soap handle to initialize as discovery client
 * @param p pointer to soap plugin structure
 * @param arg argument by soap_register_plugin_arg() (ignored)
 *
 * @return returns SOAP_OK on success or a failure otherwise.
 */

int
wsd_mca_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg)
{
  WS4D_UNUSED_PARAM (arg);

  /* set plugin id */
  p->id = wsd_mca_plugin_id;

  /* allocate memory for plugin data */
  p->data = (void *) ws4d_malloc (sizeof (struct wsd_mca_plugin_data));

  p->fdelete = wsd_mca_plugin_delete;

  /* register plugin copy and delete functions */
  soap->fopen = wsd_connect;
  soap->fclose = wsd_disconnect;

  /* plugin data initialization */
  if (p->data)
    {
      struct wsd_mca_plugin_data *data = p->data;

      WS4D_ALLOCLIST_INIT (&data->alist);

#ifdef WSD_CLIENT
      data->dis_hooks = NULL;
#endif
    }
  return SOAP_OK;
}

/**
 * Function to register a soap handle as discovery multicast handle.
 *
 * @param soap soap handle to initialize as discovery client
 * @param netdev character string of ip address of network device where the plugin should operate on
 * @param backlog maximum length of the queue of pending messages
 * @param hooks structure with hooks for incoming discovery multicast messages (mandatory)
 *
 * @return returns SOAP_OK on success or a failure otherwise.
 */

int
wsd_mca_register_handle (struct soap *soap, const char *netdev, int backlog,
                         struct wsd_mca_hooks_t *hooks)
{
  int err = SOAP_OK;
  struct wsd_mca_plugin_data *data = NULL;

  /* test parameters */
  if (!soap || !netdev)
    return SOAP_ERR;

  /* test if plugin is already registerd - handle was already initialized */
  data = wsd_mca_get_plugindata (soap);
  if (data)
  {
      return SOAP_OK;
  }

  /* bind soap handle to udp multicast socket */
#ifdef WITH_IPV6
  if (ws4d_is_ipv4 (netdev))
    {
#endif
      err =
        wsd_bind_multicastudp (soap, netdev, WSD_MC_IPADDR, WSD_MC_UDPPORT,
                               backlog);
#ifdef WITH_IPV6
    }
  else
    {
      err =
        wsd_bind_multicastudp (soap, netdev, WSD_MC_IPADDR6, WSD_MC_UDPPORT,
                               backlog);
    }
#endif
  if (err != SOAP_OK)
    return err;

  /* register soap handle as discovery multicast handle */
  err = soap_register_plugin_arg (soap, wsd_mca_plugin_init, hooks);
  if (err != SOAP_OK)
    return err;

  /* get plugin data */
  data = wsd_mca_get_plugindata (soap);
  if (!data)
    return SOAP_ERR;

  soap_set_imode (soap, SOAP_XML_CANONICAL);

  /* register hooks */
  if (hooks)
    data->mca_hooks = hooks;

  return err;
}

#ifdef WSD_TARGET

static int
wsd_set_header (struct soap *soap, struct ws4d_targetservice *ts)
{
  struct SOAP_ENV__Header *header;
  struct ws4d_appsequence *as;

  if (!soap || !ts || !soap->header)
    return SOAP_ERR;

  ws4d_targetservice_inc_MessageNumber (ts);
  as = ws4d_targetservice_get_appsequence (ts);

  header = soap->header;

  header->wsd__AppSequence =
    soap_malloc (soap, sizeof (struct wsd__AppSequenceType));
  memset (header->wsd__AppSequence, 0, sizeof (struct wsd__AppSequenceType));

  header->wsd__AppSequence->InstanceId = as->InstanceId;
  header->wsd__AppSequence->MessageNumber = as->MessageNumber;
  header->wsd__AppSequence->SequenceId = (char *) as->SequenceId;

  return SOAP_OK;
}

static struct Namespace *
wsd_target_extend_namespaces (struct ws4d_epr *target,
                              const struct Namespace *ext_namespaces,
                              ws4d_alloc_list * alist)
{
  struct Namespace *result = NULL, *type_namespaces = NULL;
  ws4d_qnamelist *qnamelist;

  if (!target || !ext_namespaces || !alist)
    return result;

  qnamelist = ws4d_targetep_get_TypeList (target);

  type_namespaces = soap_qnamelist_namespaces (qnamelist, alist,
                                               (struct Namespace *)
                                               ext_namespaces);

  result = soap_extend_namespaces ((struct Namespace *) ext_namespaces,
                                   type_namespaces, alist);

  return result;
}

int
wsd_target_set_namespaces (struct ws4d_epr *target, struct soap *soap,
                           const struct Namespace *new_namespaces,
                           ws4d_alloc_list * alist)
{
  struct Namespace *temp_namespaces = NULL;

  if (!soap)
    return SOAP_ERR;

  if (!new_namespaces)
    new_namespaces = target_namespaces;

  temp_namespaces =
    wsd_target_extend_namespaces (target, new_namespaces, alist);

  if (temp_namespaces)
    {
      soap_set_namespaces (soap, temp_namespaces);
      return SOAP_OK;
    }
  else
    {
      return SOAP_ERR;
    }
}

/**
 * Function to initialize the server side of the discovery plugin
 * for a soap handle.
 *
 * @param mca soap handle to listen for discovery multicast requests
 * @param netdev character string of ip address of network device where the plugin should operate on
 * @param backlog maximum length of the queue of pending messages
 * @param hooks structure with hooks for discovery multicast requests (mandatory)
 *
 * @return returns SOAP_OK on success or a failure otherwise.
 */
int
wsd_target_init (struct soap *mca, char *netdev, int backlog,
                 struct ws4d_targetservice *ts, struct wsd_mca_hooks_t *hooks)
{
  struct wsd_mca_plugin_data *data = NULL;
  int err;

  if (!mca)
    return SOAP_ERR;
  if (!strcmp (netdev, "0.0.0.0"))
    {
      printf ("\n 0.0.0.0 as netdev address not supported\n");
      return SOAP_ERR;
    }

  err = wsd_mca_register_handle (mca, netdev, backlog, hooks);
  if (err != SOAP_OK)
    return err;

  data = wsd_mca_get_plugindata (mca);

  if (data)
    {
      data->targetservice = ts;
      return SOAP_OK;
    }
  else
    {
      return SOAP_ERR;
    }
}

/*TODO: Why sequenceId here ? */
struct ws4d_epr *
wsd_target_add (struct soap *soap, const char *laddr,
                const char *paddr, const char *SequenceId)
{
  struct wsd_mca_plugin_data *data = wsd_mca_get_plugindata (soap);

  WS4D_UNUSED_PARAM (SequenceId);

  soap_assert (soap, data, NULL);

  return ws4d_targetservice_inittarget (data->targetservice, laddr, paddr);
}


struct ws4d_epr *
wsd_target_get_byAddr (struct soap *soap, const char *laddr)
{
  struct wsd_mca_plugin_data *data = wsd_mca_get_plugindata (soap);

  soap_assert (soap, data, NULL);

  return ws4d_targetservice_gettarget_byAddr (data->targetservice, laddr);
}

/**
 * @addtogroup WsDiscoveryAdvertisment Discovery Hello and Bye
 * @ingroup Internals
 * @{
 */

/**
 * Function sends a ws discovery bye message to announce the appearance of a
 * hosting_target service ont the network or a change of metadata.
 *
 * @param soap soap handle registered with wsd_target_init().
 * @param MsgId pointer to message id of message for ws addressing header.
 * @param target hosting_target
 * @param types types to put in response (mandatory)
 * @param alist memory allocation list
 *
 * @return returns SOAP_OK if message could be send.
 */

int
wsd_send_Hello (struct soap *soap, const char *MsgId,
                struct ws4d_epr *target, const char *types,
                ws4d_alloc_list * alist)
{
  struct wsd__HelloType req;
  const struct Namespace *old_namespaces = NULL;

  /* test parameters */
  if (!soap || !MsgId || !target || !ws4d_targetep_isactive (target)
      || !alist)
    return SOAP_ERR;

  old_namespaces = soap->namespaces;

  /* TODO: use abstract allocator here */
  wsd_target_set_namespaces (target, soap, NULL, alist);

  /* Prepare SOAP Header */
  wsa_header_gen_oneway (soap, MsgId, WSD_EP, WSD_HELLO_ACTION,
                         NULL, sizeof (struct SOAP_ENV__Header));
  wsd_set_header (soap, ws4d_targetep_get_ts (target));

  /* Prepare Body */
  soap_default_wsd__HelloType (soap, &req);
  soap_default_wsa__EndpointReferenceType (soap, &req.wsa__EndpointReference);
  req.wsa__EndpointReference.Address = (char *) ws4d_epr_get_Addrs (target);
  req.wsd__XAddrs = (char *) ws4d_targetep_get_XAddrs (target);
  if (types)
    {
      req.wsd__Types = (char *) types;
    }

  req.wsd__Scopes = soap_malloc (soap, sizeof (struct wsd__ScopesType));
  soap_default_wsd__ScopesType (soap, req.wsd__Scopes);
  req.wsd__Scopes->__item = (char *) "";

  req.wsd__MetadataVersion = ws4d_targetep_get_MetadataVersion (target);

#ifdef DEBUG
  soap_omode (soap, SOAP_XML_INDENT);
#endif
  soap->connect_flags = SO_BROADCAST;

  /* send hello message */
#ifdef WITH_IPV6
  soap_send___wsd__Hello (soap, WSD_MC_ADDR6, NULL, &req);
#else
  soap_send___wsd__Hello (soap, WSD_MC_ADDR, NULL, &req);
#endif

  soap_set_namespaces (soap, old_namespaces);

  return SOAP_OK;
}

/**
 * Function sends a ws discovery bye message to announce the removal of a
 * hosting_target service from network.
 *
 * @param soap soap handle registered with wsd_target_init().
 * @param MsgId pointer to message id of message for ws addressing header.
 * @param target target to use for bye message
 * @param alist allocation list
 *
 * @return returns SOAP_OK if request could be send.
 */

int
wsd_send_Bye (struct soap *soap, const char *MsgId,
              struct ws4d_epr *target, ws4d_alloc_list * alist)
{
  struct wsd__ByeType bye;
  const struct Namespace *old_namespaces = NULL;

  /* test parameters */
  if (!soap || !MsgId || !target || !ws4d_targetep_isactive (target))
    return SOAP_ERR;

  old_namespaces = soap->namespaces;

  /* TODO: use abstract allocator here */
  wsd_target_set_namespaces (target, soap, NULL, alist);

  /* Prepare SOAP Header */
  wsa_header_gen_oneway (soap, MsgId, WSD_EP, WSD_BYE_ACTION,
                         NULL, sizeof (struct SOAP_ENV__Header));
  wsd_set_header (soap, ws4d_targetep_get_ts (target));

  /* Prepare Body */
  soap_default_wsd__ByeType (soap, &bye);
  soap_default_wsa__EndpointReferenceType (soap, &bye.wsa__EndpointReference);
  bye.wsa__EndpointReference.Address = (char *) ws4d_epr_get_Addrs (target);

#ifdef DEBUG
  soap_omode (soap, SOAP_XML_INDENT);
#endif
  soap->connect_flags = SO_BROADCAST;

  /* send bye message */
#ifdef WITH_IPV6
  soap_send___wsd__Bye (soap, WSD_MC_ADDR6, NULL, &bye);
#else
  soap_send___wsd__Bye (soap, WSD_MC_ADDR, NULL, &bye);
#endif

  soap_set_namespaces (soap, old_namespaces);

  return SOAP_OK;
}

/** @} */

char *
wsd_gen_response_addr (void *in_addr, const char *path,
                       ws4d_alloc_list * alist)
{
  int err = 0;
  char *res = NULL;
  int res_len = 0;
  struct ws4d_uri to_uri;
  char portstr[6];
  int port = 0;
#ifdef WITH_IPV6
  char straddr[INET6_ADDRSTRLEN];
  struct sockaddr_in6 *in6 = (struct sockaddr_in6 *) in_addr;
#else
  char straddr[INET_ADDRSTRLEN];
  struct sockaddr_in *in = (struct sockaddr_in *) in_addr;
#endif

  ws4d_assert (in_addr && path && alist, NULL);

  ws4d_uri_init (&to_uri);

#ifdef WITH_IPV6
  if (in6->sin6_family == AF_INET)
    {
      struct sockaddr_in *in = (struct sockaddr_in *) in_addr;
#endif
      ws4d_inet_ntop (AF_INET, &in->sin_addr, straddr, sizeof (straddr));

      port = ntohs (in->sin_port);
#ifdef WITH_IPV6
    }
  else
    {
      inet_ntop (AF_INET6, &in6->sin6_addr, straddr, sizeof (straddr));
      port = ntohs (in6->sin6_port);
    }
#endif

  memset (portstr, 0, 6);
  SNPRINTF (portstr, 6, "%d", port);

  to_uri.scheme = (char *) "soap.udp";
  to_uri.host = straddr;
  to_uri.port = portstr;
  to_uri.path = (char *) path;

  res_len = ws4d_uri_tostrlen (&to_uri) + 1;
  res = ws4d_malloc_alist (res_len, alist);
  if (!res)
    {
      return NULL;
    }

  err = ws4d_uri_tostr (&to_uri, res, res_len);
  if (err != WS4D_OK)
    {
      ws4d_free_alist (res);
      return NULL;
    }

  ws4d_uri_done (&to_uri);

  return res;
}


int
wsd_gen_ProbeMatches (struct soap *soap, struct ws4d_abs_eprlist *matches,
                      struct wsd__ProbeMatchesType *wsd__ProbeMatches)
{
  struct wsd__ProbeMatchType *probeMatch = NULL;
  int count = 0;
  register struct ws4d_epr *target, *next;

  if (!soap || !wsd__ProbeMatches || !matches
      || ws4d_eprlist_isempty (matches))
    return SOAP_ERR;

  /* Prepare Body */
  soap_default_wsd__ProbeMatchesType (soap, wsd__ProbeMatches);

  /* count matches */
  ws4d_eprlist_foreach (target, next, matches)
  {
    count++;
  }

  /* allocate probeMatch element */
  probeMatch =
    soap_malloc (soap, sizeof (struct wsd__ProbeMatchType) * count);
  if (!probeMatch)
    return SOAP_EOM;

  /* at the moment we support only one probeMatch element */
  wsd__ProbeMatches->ProbeMatch = probeMatch;
  wsd__ProbeMatches->__sizeProbeMatch = count;

  ws4d_eprlist_foreach (target, next, matches)
  {
    const char *types;
    const char *scopes;

    /* prepare probeMatch element */
    soap_default_wsd__ProbeMatchType (soap, probeMatch);
    probeMatch->wsa__EndpointReference.Address =
      (char *) ws4d_epr_get_Addrs (target);

    /* we don't set the XAddrs to keep message size small !!! */

    /* set types if specified */
    types = ws4d_targetep_get_Types (target);
    if (types)
      {
        probeMatch->wsd__Types = (char *) types;
      }

    /* set scopes if specified, otherwise empty scopes element */
    probeMatch->wsd__Scopes =
      soap_malloc (soap, sizeof (struct wsd__ScopesType));
    if (!probeMatch->wsd__Scopes)
      return SOAP_EOM;
    soap_default_wsd__ScopesType (soap, probeMatch->wsd__Scopes);

    scopes = ws4d_targetep_get_Scopes (target);
    if (scopes)
      {
        probeMatch->wsd__Scopes->__item = (char *) scopes;
      }
    else
      {
        probeMatch->wsd__Scopes->__item = (char *) "";
      }

    /* set MetadataVersion */
    probeMatch->wsd__MetadataVersion =
      ws4d_targetep_get_MetadataVersion (target);

    probeMatch++;
  }

  return SOAP_OK;

}

int
wsd_gen_ProbeMatches_header (struct soap *soap, const char *MsgId,
                             const char *to, struct ws4d_abs_eprlist *matches,
                             struct soap *req,
                             const struct Namespace *matches_namespaces,
                             ws4d_alloc_list * alist)
{
  register struct ws4d_epr *target, *next;
  void *targetservice = NULL;

  /* test parameters */
  if (!soap || !MsgId || !to || !matches || ws4d_eprlist_isempty (matches)
      || !req || !alist)
    return SOAP_ERR;

  ws4d_eprlist_foreach (target, next, matches)
  {
    if (targetservice == NULL)
      targetservice = ws4d_targetep_get_ts (target);
    wsd_target_set_namespaces (target, soap, matches_namespaces, alist);
  }


  /* Prepare SOAP Header */
  wsa_header_gen_response (soap, MsgId, wsa_anonymousURI,
                           WSD_PROBEMATCHES_ACTION,
                           wsa_header_get_MessageId (req),
                           sizeof (struct SOAP_ENV__Header));
  wsd_set_header (soap, targetservice);

  return SOAP_OK;
}

/**
 * Sends a response to a web service discovery probe request. This
 * function should be called if a probe request matches an entry of
 * the registered mappings.
 *
 * @param soap soap handle to send the response
 * @param MsgId pointer to message id of response for ws addressing header.
 * @param to address the response should be sent to
 * @param target hosting_target
 * @param types types to put in response (mandatory)
 * @param scopes scopes to put in response
 * @param req soap handle that received the request and is registered with wsd_target_init()
 * @param alist allocation list
 * @param wsd__ProbeMatches message to send
 *
 * @return returns SOAP_OK if response could be send.
 */
int
wsd_send_async_ProbeMatches (struct soap *soap, const char *MsgId,
                             const char *to, struct ws4d_abs_eprlist *matches,
                             struct soap *req, ws4d_alloc_list * alist,
                             struct wsd__ProbeMatchesType *wsd__ProbeMatches)
{
  const struct Namespace *old_namespaces = NULL;

  /* test parameters */
  if (!MsgId || !to || !req || !wsd__ProbeMatches)
    return SOAP_ERR;

  old_namespaces = soap->namespaces;
  wsd_gen_ProbeMatches_header (soap, MsgId, to, matches, req, NULL, alist);

  /* TODO: use proper retransmission algorithm */
  SLEEP (1);

#ifdef DEBUG
  soap_omode (soap, SOAP_XML_INDENT);
#endif

  soap->connect_flags = SO_BROADCAST;

  /* send probe response */
  soap_send___wsd__ProbeMatches (soap, to, NULL, wsd__ProbeMatches);

  soap_set_namespaces (soap, old_namespaces);

  return SOAP_OK;
}

/**
 * Function to process probe requests send by discovery clients. Should be
 * called in handler for probe messages.
 *
 * @param soap soap handle registered with wsd_target_init()
 * @param req Message to process
 * @param count number of hosting_target in matching targets buffer
 * @param matching_targets buffer where matching targets are copied to
 *
 * @return returns SOAP_OK if the probe matches or SOAP_ERR otherwise
 */
int
wsd_process_probe (struct soap *soap, struct wsd__ProbeType *req,
                   struct ws4d_abs_eprlist *matching_targets)
{
  int err;
  char *scopes = NULL, *types = NULL, *matchby = NULL;
  struct wsd_mca_plugin_data *mca_data = wsd_mca_get_plugindata (soap);

  /* test parmaters */
  if (!mca_data)
    return SOAP_ERR;

  /* call hook for Probe messages if it exists */
  if (mca_data->mca_hooks && mca_data->mca_hooks->Probe)
    mca_data->mca_hooks->Probe (soap, req);

  if (req && req->wsd__Scopes && req->wsd__Scopes->__item)
    {
      scopes = req->wsd__Scopes->__item;
      matchby = req->wsd__Scopes->MatchBy;
    }

  if (req && req->wsd__Types)
    {
      types = req->wsd__Types;
    }

  err =
    ws4d_targetservice_getmatches (mca_data->targetservice, scopes, matchby,
                                   types, matching_targets);
  if (err)
    return SOAP_ERR;

  return SOAP_OK;
}

/**
 * Sends a response to a web service discovery resolve request. This
 * function should be called if a resolve request matches an entry of
 * the registered mappings.
 *
 * @param soap soap handle to send the response
 * @param MsgId pointer to message id of response for ws addressing header.
 * @param to address the response should be sent to
 * @param target hosting_target
 * @param req soap handle that received the request and is registered with wsd_target_init().
 * @param alist allocation list
 *
 * @return returns SOAP_OK if response could be send.
 */
int
wsd_send_ResolveMatches (struct soap *soap, const char *MsgId,
                         const char *to, struct ws4d_epr *target,
                         struct soap *req, ws4d_alloc_list * alist)
{
  struct wsd__ResolveMatchesType resolveMatches;
  struct wsd__ResolveMatchType resolveMatch;
  struct wsd_mca_plugin_data *data = wsd_mca_get_plugindata (req);
  const struct Namespace *old_namespaces = NULL;

  /* test parameters */
  if (!soap || !data || !MsgId || !to || !target
      || !ws4d_targetep_isactive (target))
    return SOAP_ERR;

  old_namespaces = soap->namespaces;

  wsd_target_set_namespaces (target, soap, NULL, alist);

  /* Prepare SOAP Header */
  wsa_header_gen_response (soap, MsgId, wsa_anonymousURI,
                           WSD_RESOLVEMATCHES_ACTION,
                           wsa_header_get_MessageId (req),
                           sizeof (struct SOAP_ENV__Header));
  wsd_set_header (soap, ws4d_targetep_get_ts (target));

  /* Prepare Body */
  soap_default_wsd__ResolveMatchesType (soap, &resolveMatches);
  resolveMatches.ResolveMatch = &resolveMatch;
  soap_default_wsd__ResolveMatchType (soap, &resolveMatch);
  soap_default_wsa__EndpointReferenceType (soap,
                                           &resolveMatch.wsa__EndpointReference);
  resolveMatch.wsa__EndpointReference.Address =
    (char *) ws4d_epr_get_Addrs (target);
  resolveMatch.wsd__XAddrs = (char *) ws4d_targetep_get_XAddrs (target);
  resolveMatch.wsd__MetadataVersion =
    ws4d_targetep_get_MetadataVersion (target);

#ifdef DEBUG
  soap_omode (soap, SOAP_XML_INDENT);
#endif

  soap->connect_flags = SO_BROADCAST;

  /* send resolve response */
  soap_send___wsd__ResolveMatches (soap, to, NULL, &resolveMatches);

  soap_set_namespaces (soap, old_namespaces);

  return SOAP_OK;
}

/**
 * Function to process resolve requests send by discovery clients. Should be
 * called in handler for resolve messages.
 *
 * @param soap soap handle registered with wsd_target_init()
 * @param req Message to process
 * @param target buffer where matching hosting_target is copied to
 *
 * @return if the address can be resolved, returns a wsd_addr_mapping structure with the mapping
 */
int
wsd_process_resolve (struct soap *soap, struct wsd__ResolveType *req,
                     struct ws4d_epr **target)
{
  struct wsd_mca_plugin_data *data = wsd_mca_get_plugindata (soap);

  /* test parameters */
  if (!data || !req)
    return SOAP_ERR;

  /* call hook for resolve messages if it exists */
  if (data->mca_hooks && data->mca_hooks->Resolve)
    data->mca_hooks->Resolve (soap, req);

  /* test message */
  if (!req || !req->wsa__EndpointReference.Address)
    return SOAP_ERR;

  /* search for mapping */
  *target =
    ws4d_targetservice_gettarget_byAddr (data->targetservice,
                                         req->wsa__EndpointReference.Address);

  return SOAP_OK;
}

#endif

#ifdef WSD_CLIENT

#define WSD_MAX_RES_BUF 10
#define WSD_MAX_DIS_BUF 10

struct wsd_uni_plugin_data
{
  struct ws4d_tc_int *target_cache;

  struct wsd_uni_hooks_t *uni_hooks;
  struct wsd_dis_hooks_t *dis_hooks;
};

#define WSD_UNI_PLUGIN_ID "WS-Discovery-Unicast-PLUGIN-0.1"
const char *wsd_uni_plugin_id = WSD_UNI_PLUGIN_ID;

INLINE struct wsd_uni_plugin_data *wsd_uni_get_plugindata (struct soap *soap);

/**
 * Function returns the plugin data of a discovery unicast soap handle
 * registered with wsd_init_client() before.
 *
 * @param soap soap handle registered with wsd_init_client()
 *
 * @return pointer to discovery unicast plugin data structure
 */

INLINE struct wsd_uni_plugin_data *
wsd_uni_get_plugindata (struct soap *soap)
{
  return (struct wsd_uni_plugin_data *) soap_lookup_plugin (soap,
                                                            WSD_UNI_PLUGIN_ID);
}

#include "ws4d_targetcache.h"

/**
 * Function to destroy discovery unicast plugin data.
 * Function is called by soap_done().
 *
 * @param soap soap handle
 * @param p pointer to soap plugin structure
 *
 */

static void
wsd_uni_plugin_delete (struct soap *soap, struct soap_plugin *p)
{
  WS4D_UNUSED_PARAM (soap);

  /* free plugin data structure */
  ws4d_free (p->data);
}

/* begin forward declarations */
int wsd_uni_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg);

int wsd_uni_register_handle (struct soap *uni, const char *netdev, int mode,
                             int backlog, struct wsd_uni_hooks_t *uni_hooks);
/* end forward declarations */

#define WSD_UNICAST_UDP 1
#define WSD_UNICAST_HTTP 2

/**
 * Function to initialize discovery unicast plugin data.
 * Function is called by soap_register_plugin().
 *
 * @param soap soap handle to initialize as discovery client
 * @param p pointer to soap plugin structure
 * @param arg argument by soap_register_plugin_arg() (ignored)
 *
 * @return returns SOAP_OK on success or a failure otherwise.
 */

int
wsd_uni_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg)
{
  int mode = *((int *) arg);

  /* set plugin id */
  p->id = wsd_uni_plugin_id;

  /* allocate memory for plugin data */
  p->data = (void *) ws4d_malloc (sizeof (struct wsd_uni_plugin_data));

  /* register delete function */
  p->fdelete = wsd_uni_plugin_delete;

  if (mode == WSD_UNICAST_UDP)
    {
      /* overwrite soap open and close functions */
      soap->fopen = wsd_connect;
      soap->fclose = wsd_disconnect;
    }

  /* plugin data initialization */
  if (p->data)
    {
      struct wsd_uni_plugin_data *data = p->data;
      data->target_cache = NULL;
      data->uni_hooks = NULL;
      data->dis_hooks = NULL;
    }

  return SOAP_OK;
}

/**
 * Function to register a soap handle as discovery unicast handle.
 *
 * @param uni soap handle to initialize as discovery client
 * @param netdev character string of ip address of network device where the plugin should operate on
 * @param backlog maximum length of the queue of pending messages
 * @param hooks structure with hooks for NewTarget and InvalidateTarget events (mandatory)
 *
 * @return returns SOAP_OK on success or a failure otherwise.
 */

int
wsd_uni_register_handle (struct soap *uni, const char *netdev, int mode,
                         int backlog, struct wsd_uni_hooks_t *hooks)
{
  int err = 0;
  struct wsd_uni_plugin_data *uni_data = NULL;

  /* test parameters */
  if (!uni || !netdev)
    return SOAP_ERR;

  if ((mode != WSD_UNICAST_UDP) && (mode != WSD_UNICAST_HTTP))
    {
      mode = WSD_UNICAST_UDP;
    }

  if (mode == WSD_UNICAST_UDP)
    {
      /* bind soap handle to udp unicast socket */
      err = wsd_bind_udp (uni, netdev, 0, backlog);
      if (err != SOAP_OK)
        return err;
    }

  /* register soap handle as discovery unicast handle */
  err = soap_register_plugin_arg (uni, wsd_uni_plugin_init, &mode);
  if (err != SOAP_OK)
    return err;

  /* get plugin data */
  uni_data = wsd_uni_get_plugindata (uni);
  if (!uni_data)
    return SOAP_ERR;
  soap_set_imode (uni, SOAP_XML_CANONICAL);

  /* register hooks */
  if (hooks)
    uni_data->uni_hooks = hooks;
  return err;
}

int
wsd_soap_init_implicite (struct soap *mca, struct ws4d_tc_int *dis_cache,
                         const char *netdev, int backlog,
                         struct wsd_mca_hooks_t *mca_hooks,
                         struct wsd_dis_hooks_t *dis_hooks)
{
  int err = 0;
  struct wsd_uni_plugin_data *uni_data = wsd_uni_get_plugindata (mca);
  struct wsd_mca_plugin_data *mca_data = wsd_mca_get_plugindata (mca);

  /* test parameters */
  if (!mca || !netdev || uni_data)
    return SOAP_ERR;

  /*
   * Test for ANY_ADDR. We don't support ANY_ADDR
   * as we wouldn't know the address.
   */
  if (!strcmp (netdev, "0.0.0.0"))
    {
      printf ("\n 0.0.0.0 as netdev address not supported\n");
      return SOAP_ERR;
    }

  /* register multicast handle */
  err = wsd_mca_register_handle (mca, (char *) netdev, backlog, mca_hooks);
  if (err != SOAP_OK)
    return err;

  /* get plugin data */
  mca_data = wsd_mca_get_plugindata (mca);
  if (!mca_data)
    return SOAP_ERR;
  if (dis_hooks)
    mca_data->dis_hooks = dis_hooks;
  if (dis_cache)
    {
      mca_data->target_cache = dis_cache;
    }

  return SOAP_OK;
}

int
wsd_process_hello (struct soap *soap, struct wsd__HelloType *req)
{
  struct ws4d_epr *target = NULL;
  struct wsd_mca_plugin_data *mca_data = wsd_mca_get_plugindata (soap);
  struct wsd_uni_plugin_data *uni_data = NULL;

  char *Address = NULL;

  /* test parameters */
  if (!req)
    return SOAP_ERR;

  if (!mca_data)
    {
      uni_data = wsd_uni_get_plugindata (soap);
      if (uni_data)
        {
          /* ignore discovery proxy :-) */
          return SOAP_OK;
        }
      else
        {
          return SOAP_ERR;
        }
    }

  /* call hook for hello messages if it exists */
  if (mca_data->mca_hooks && mca_data->mca_hooks->Hello)
    mca_data->mca_hooks->Hello (soap, req);

  /* check for logical address in endpoint reference of message */
  if (!req->wsa__EndpointReference.Address)
    return SOAP_SYNTAX_ERROR;

  /* extract relevant data */
  Address = req->wsa__EndpointReference.Address;

  /* is a new target hook installed ? */
  if (mca_data->dis_hooks && mca_data->dis_hooks->NewTarget)
    {
      /* create ws4d_epr for hook */
      target = ws4d_epr_alloc (1, &mca_data->alist);
      soap_assert (soap, target, SOAP_EOM);

      /* copy epr data */
      if (Address)
        {
          ws4d_epr_set_Addrs (target, Address);
        }

      /* set metadata version */
      ws4d_targetep_set_MetadataVersion (target, req->wsd__MetadataVersion);

      /* set types */
      if (req->wsd__Types)
        {
          ws4d_targetep_set_Types (target, req->wsd__Types);
        }

      /* set scopes */
      if (req->wsd__Scopes && req->wsd__Scopes->__item)
        {
          ws4d_targetep_set_Scopes (target, req->wsd__Scopes->__item);
        }

      /* set XAddrs */
      if (req->wsd__XAddrs)
        {
          ws4d_targetep_set_XAddrs (target, req->wsd__XAddrs);
        }

      mca_data->dis_hooks->NewTarget (soap, target);

      /* free target structure for hook */
      ws4d_epr_free (1, target);
    }

  /* is target cache available ? */
  if (mca_data->target_cache)
    {
      int target_state = 0;

      ws4d_tc_lock (mca_data->target_cache);

      /* is target in target cache ??? */
      target_state = ws4d_tc_check_epr (mca_data->target_cache,
                                        Address, req->wsd__MetadataVersion);

      /* if target is in cache then update data */
      if (target_state == WS4D_TARGETCACHE_VALID)
        {
          /* update types */
          if (req->wsd__Types)
            {
              ws4d_tc_update_Types (mca_data->target_cache, Address,
                                    req->wsd__Types);
            }

          /* update scopes */
          if (req->wsd__Scopes && req->wsd__Scopes->__item)
            {
              ws4d_tc_update_Scopes (mca_data->target_cache, Address,
                                     req->wsd__Scopes->__item);
            }

          /* update XAddrs */
          if (req->wsd__XAddrs)
            {
              ws4d_tc_update_XAddrs (mca_data->target_cache, Address,
                                     req->wsd__XAddrs);
            }
        }

      ws4d_tc_unlock (mca_data->target_cache);
    }

  return SOAP_OK;
}

int
wsd_process_bye (struct soap *soap, struct wsd__ByeType *req)
{
  struct ws4d_epr *target = NULL;
  struct wsd_mca_plugin_data *mca_data = wsd_mca_get_plugindata (soap);

  char *Address = NULL;

  /* test parameters */
  if (!mca_data || !req)
    return SOAP_ERR;

  /* call hook for bye messages if it exists */
  if (mca_data->mca_hooks && mca_data->mca_hooks->Bye)
    mca_data->mca_hooks->Bye (soap, req);

  /* check for logical address in endpoint reference of message */
  if (!req->wsa__EndpointReference.Address)
    return SOAP_SYNTAX_ERROR;

  /* extract relevant data */
  Address = req->wsa__EndpointReference.Address;

  /* is a invalid target hook installed ? */
  if (mca_data->dis_hooks && mca_data->dis_hooks->InvalidateTarget)
    {

      /* create ws4d_epr for hook */
      target = ws4d_epr_alloc (1, &mca_data->alist);

      /* copy epr data */
      if (Address)
        {
          ws4d_epr_set_Addrs (target, Address);
        }

      /* call hook for invalid targets */
      mca_data->dis_hooks->InvalidateTarget (soap, target);

      /* free ws4d_epr */
      ws4d_epr_free (1, target);
    }

  /* is target cache available ? */
  if (mca_data->target_cache)
    {
      ws4d_tc_lock (mca_data->target_cache);

      /* invalidate target in target cache */
      ws4d_tc_invalidate_epr (mca_data->target_cache, Address);

      ws4d_tc_unlock (mca_data->target_cache);
    }

  return SOAP_OK;
}

int
wsd_soap_init_explicit (struct soap *uni, struct ws4d_tc_int *dis_cache,
                        const char *netdev, int backlog,
                        struct wsd_uni_hooks_t *uni_hooks,
                        struct wsd_dis_hooks_t *dis_hooks)
{
  int err = 0;
  struct wsd_uni_plugin_data *uni_data = wsd_uni_get_plugindata (uni);
  struct wsd_mca_plugin_data *mca_data = wsd_mca_get_plugindata (uni);

  /* test parameters */
  if (!uni || !netdev || uni_data || mca_data)
    return SOAP_ERR;

  /*
   * Test for ANY_ADDR. We don't support ANY_ADDR
   * as we wouldn't know the address.
   */
  if (!strcmp (netdev, "0.0.0.0"))
    {
      printf ("\n 0.0.0.0 as netdev address not supported\n");
      return SOAP_ERR;
    }

  /* register unicast handle */
  err = wsd_uni_register_handle (uni, (char *) netdev, WSD_UNICAST_UDP,
                                 backlog, uni_hooks);
  if (err != SOAP_OK)
    return err;

  /* get plugin data */
  uni_data = wsd_uni_get_plugindata (uni);
  if (!uni_data)
    return SOAP_ERR;
  if (dis_hooks)
    uni_data->dis_hooks = dis_hooks;
  if (dis_cache)
    {
      uni_data->target_cache = dis_cache;
    }

  soap_set_namespaces (uni, target_namespaces);

  return SOAP_OK;
}

#define WSD_UNICAST_UDP 1
#define WSD_UNICAST_HTTP 2

int
wsd_soap_init_directed (struct soap *uni, struct ws4d_tc_int *dis_cache,
                        const char *netdev, int backlog,
                        struct wsd_uni_hooks_t *uni_hooks,
                        struct wsd_dis_hooks_t *dis_hooks)
{
  int err = 0;
  struct wsd_uni_plugin_data *uni_data = wsd_uni_get_plugindata (uni);
  struct wsd_mca_plugin_data *mca_data = wsd_mca_get_plugindata (uni);

  /* test parameters */
  if (!uni || !netdev || uni_data || mca_data)
    return SOAP_ERR;

  /*
   * Test for ANY_ADDR. We don't support ANY_ADDR
   * as we wouldn't know the address.
   */
  if (!strcmp (netdev, "0.0.0.0"))
    {
      printf ("\n 0.0.0.0 as netdev address not supported\n");
      return SOAP_ERR;
    }

  /* register unicast handle */
  err = wsd_uni_register_handle (uni, (char *) netdev, WSD_UNICAST_HTTP,
                                 backlog, uni_hooks);
  if (err != SOAP_OK)
    return err;

  /* get plugin data */
  uni_data = wsd_uni_get_plugindata (uni);
  if (!uni_data)
    return SOAP_ERR;
  if (dis_hooks)
    uni_data->dis_hooks = dis_hooks;
  if (dis_cache)
    {
      uni_data->target_cache = dis_cache;
    }

  soap_set_namespaces (uni, target_namespaces);

  return SOAP_OK;
}

int
wsd_process_probematches (struct soap *soap,
                          struct wsd__ProbeMatchesType *req)
{
  struct wsd_uni_plugin_data *uni_data = wsd_uni_get_plugindata (soap);
  int i = 0;
  struct wsd__ProbeMatchType *probe_match = NULL;
  ws4d_alloc_list alist;

  /* test parameters */
  if (!uni_data || !req)
    return SOAP_ERR;

  /* call hook for ProbeMatches messages if it exists */
  if (uni_data->uni_hooks && uni_data->uni_hooks->ProbeMatches)
    uni_data->uni_hooks->ProbeMatches (soap, req);

  /* test for relatesto header field, we need this ! */
  if (!wsa_header_get_RelatesTo (soap))
    return SOAP_SYNTAX_ERROR;

  WS4D_ALLOCLIST_INIT (&alist);

  /* iterate over all entries of the ProbeMatches array */
  for (i = 0, probe_match = req->ProbeMatch; (i < req->__sizeProbeMatch)
       && probe_match; i++, probe_match++)
    {
      char *Address = probe_match->wsa__EndpointReference.Address;
      int target_state = 0;

      /* lock target cache */
      ws4d_tc_lock (uni_data->target_cache);

      /* add hosting_target if it is not in discovery cache */
      target_state = ws4d_tc_checkadd_epr (uni_data->target_cache, Address,
                                           probe_match->wsd__MetadataVersion);

      if (target_state == WS4D_TARGETCACHE_NEW)
        {
          /* update types */
          if (probe_match->wsd__Types)
            {
              ws4d_tc_update_Types (uni_data->target_cache, Address,
                                    probe_match->wsd__Types);
            }

          /* update scopes */
          if (probe_match->wsd__Scopes && probe_match->wsd__Scopes->__item)
            {
              ws4d_tc_update_Scopes (uni_data->target_cache, Address,
                                     probe_match->wsd__Scopes->__item);
            }

          /* update XAddrs */
          if (probe_match->wsd__XAddrs)
            {
              ws4d_tc_update_XAddrs (uni_data->target_cache, Address,
                                     probe_match->wsd__XAddrs);
            }
        }

      /*
       * set message id for corresponding probe message,
       * so we can associate the entry to the request
       */
      ws4d_tc_set_ProbeMsgId (uni_data->target_cache, Address,
                              wsa_header_get_RelatesTo (soap));

      ws4d_tc_unlock (uni_data->target_cache);

      /* new target ? */
      if (target_state == WS4D_TARGETCACHE_NEW)
        {
          /* is there a new device callback? */
          if (uni_data->dis_hooks && uni_data->dis_hooks->NewTarget)
            {
              int err;
              struct ws4d_epr *new_target = ws4d_epr_alloc (1, &alist);

              /* get epr from target cache */
              ws4d_tc_lock (uni_data->target_cache);
              err =
                ws4d_tc_get_byAddr (uni_data->target_cache, Address,
                                    new_target);
              ws4d_tc_unlock (uni_data->target_cache);
              if (err == SOAP_OK)
                {
                  /* call hook */
                  uni_data->dis_hooks->NewTarget (soap, new_target);
                }

              ws4d_epr_free (1, new_target);
            }
        }
    }

  ws4d_alloclist_done (&alist);

  return SOAP_OK;
}

int
wsd_probe_async (struct soap *soap, const char *MsgId, const char *To,
                 ws4d_qnamelist * Types, const char *Scope, ws4d_time timeout,
                 ws4d_alloc_list * alist)
{
  struct wsd_uni_plugin_data *uni_data = wsd_uni_get_plugindata (soap);
  struct wsd__ProbeType probe;
  struct wsd__ScopesType ScopesType;
  int ret = 0;

  /* test parameters */
  if (!uni_data || !MsgId)
    return SOAP_ERR;

  /* set timeouts */
  soap->connect_timeout = -1000 * timeout;
  soap->recv_timeout = -1000 * timeout;
  soap->send_timeout = -1000 * timeout;

  soap_set_namespaces (soap, target_namespaces);

  if (Types)
    {
      struct Namespace *ext_namespaces = NULL;
      ext_namespaces = soap_extend_namespaces ((struct Namespace *)
                                               target_namespaces,
                                               soap_qnamelist_namespaces
                                               (Types, alist,
                                                (struct Namespace *)
                                                target_namespaces), alist);
      soap_set_namespaces (soap, (const struct Namespace *) ext_namespaces);
    }

  soap_begin_send (soap);

  /* Prepare SOAP Header */
  wsa_header_gen_request (soap, MsgId, WSD_EP, WSD_PROBE_ACTION,
                          NULL, NULL, sizeof (struct SOAP_ENV__Header));

  /* Prepare Body */
  soap_default_wsd__ProbeType (soap, &probe);

  if (Types)
    {
      /* Prepare Types */
      probe.wsd__Types = (char *) ws4d_qnamelist_tostring (Types, alist);
    }
  else
    {
      probe.wsd__Types = NULL;
    }

  if (Scope && (Scope[0] != 0))
    {
      soap_default__wsd__Scopes (soap, &ScopesType);
      ScopesType.__item = (char *) Scope;
      probe.wsd__Scopes = &ScopesType;
    }

  if (To && !strncmp ("http://", To, 7))
    {
      /* this is and direct probe */
      ret = soap_send___wsd__Probe (soap, To, NULL, &probe);
    }
  else
    {
      /* this is a multicast probe */
      soap->connect_flags = SO_BROADCAST;

      ret = soap_send___wsd__Probe (soap, WSD_MC_ADDR, NULL, &probe);

    }

  soap_end_send (soap);

  if (To && (ret == SOAP_OK) && !strncmp ("http://", To, 7))
    {
      /* receive response to direct probe */
      ret = wsd_process_onemessage (soap, timeout);
    }

  return ret;
}

int
wsd_process (struct soap *soap, ws4d_time timeout)
{
  ws4d_time end;

  end = ws4d_systime_ms () + timeout;

  while (end > ws4d_systime_ms ())
    {
      if (wsd_process_onemessage (soap, end - ws4d_systime_ms ()))
        {
          return soap->error;
        }
    }

  return SOAP_EOF;
}

int
wsd_process_onemessage (struct soap *soap, ws4d_time timeout)
{
  /* test parameters */
  soap_assert (NULL, soap, SOAP_ERR);

  soap->recv_timeout = -1000 * timeout;

  soap_begin (soap);
  if (soap_begin_recv (soap))
    {
      return soap->error;
    }

  if (soap_envelope_begin_in (soap)
      || soap_recv_header (soap)
      || soap_body_begin_in (soap)
      || target_serve_request (soap)
      || (soap->fserveloop && soap->fserveloop (soap)))
    {
      return soap->error;
    }

  soap_end (soap);

  return SOAP_OK;
}

/**
 * Checks if an address is a physikal address
 *
 * @param soap soap handle registered with wsd_init_client().
 * @param Address address to check
 *
 * @return 1 if address is physikal, otherwise 0
 */
int
wsd_is_physical_addr (struct soap *soap, const char *Address)
{
  struct in_addr inaddr;

  /* test parameters */
  if (!soap || !Address)
    return SOAP_ERR;

  /* no logical device addresses */
  if (!strncmp ("urn:uuid:", Address, 9))
    return SOAP_ERR;

  /* try to resolve address with gsoap */
  if (soap->fresolve)
    {
      soap_set_endpoint (soap, Address);
      return soap->fresolve (soap, soap->host, &inaddr);
    }
  else
    {
      return SOAP_OK;
    }
}

/**
 * Sends a resolve request message to resolve a logical address,
 *
 * @param soap soap handle registered with wsd_init_client().
 * @param MsgId pointer to message id of request for ws addressing header.
 * @param Address logical address to resolve.
 *
 * @return returns SOAP_OK if request could be send.
 */

int
wsd_resolve_async (struct soap *soap, const char *MsgId,
                   const char *Address, ws4d_time timeout,
                   ws4d_alloc_list * alist)
{
  struct wsd__ResolveType req;

  WS4D_UNUSED_PARAM (alist);

  /* test parameters */
  if (!soap || !MsgId || !Address)
    return SOAP_ERR;

  /* set timeouts */
  soap->send_timeout = -1000 * timeout;

  soap_begin_send (soap);
  soap->connect_flags = SO_BROADCAST;

  /* Prepare SOAP Header */
  wsa_header_gen_request (soap, MsgId, WSD_EP, WSD_RESOLVE_ACTION,
                          NULL, NULL, sizeof (struct SOAP_ENV__Header));

  /* Prepare SOAP Body */
  soap_default_wsd__ResolveType (soap, &req);
  req.wsa__EndpointReference.Address = (char *) Address;

  /* send resolve request */
  soap_send___wsd__Resolve (soap, WSD_MC_ADDR, NULL, &req);
  soap_end_send (soap);
  return SOAP_OK;
}

int
wsd_process_resolvematches (struct soap *soap,
                            struct wsd__ResolveMatchesType *req)
{
  struct wsd_uni_plugin_data *uni_data = wsd_uni_get_plugindata (soap);

  char *Address = NULL;
  char *XAddrs = NULL;
  int MetadataVersion = 0;

  /* test parameters */
  if (!uni_data || !req)
    return SOAP_ERR;

  /* call hook for ResolveMatches if it exists */
  if (uni_data->uni_hooks && uni_data->uni_hooks->ResolveMatches)
    uni_data->uni_hooks->ResolveMatches (soap, req);

  /* test for relatesto header field, we need this ! */
  if (!wsa_header_get_RelatesTo (soap))
    return SOAP_SYNTAX_ERROR;

  if (!req->ResolveMatch)
    return SOAP_SYNTAX_ERROR;

  /* extract relevant data */
  Address = req->ResolveMatch->wsa__EndpointReference.Address;
  MetadataVersion = req->ResolveMatch->wsd__MetadataVersion;
  XAddrs = req->ResolveMatch->wsd__XAddrs;

  /* test message */
  if (Address && XAddrs)
    {
      int target_state;

      /* extract resolved logical address */
      MetadataVersion = req->ResolveMatch->wsd__MetadataVersion;

      /* test for mapping */
      target_state = ws4d_tc_checkadd_epr (uni_data->target_cache, Address,
                                           MetadataVersion);

      if ((target_state == WS4D_TARGETCACHE_NEW) || (target_state
                                                     ==
                                                     WS4D_TARGETCACHE_VALID))
        {
          /* update types */
          if (req->ResolveMatch->wsd__Types)
            {
              ws4d_tc_update_Types (uni_data->target_cache, Address,
                                    req->ResolveMatch->wsd__Types);
            }

          /* update scopes */
          if (req->ResolveMatch->wsd__Scopes
              && req->ResolveMatch->wsd__Scopes->__item)
            {
              ws4d_tc_update_Scopes (uni_data->target_cache, Address,
                                     req->ResolveMatch->wsd__Scopes->__item);
            }

          /* update XAddrs */
          ws4d_tc_update_XAddrs (uni_data->target_cache, Address, XAddrs);

          /*
           * set message id for corresponding probe message,
           * so we can associate the entry to the request
           */
          ws4d_tc_set_ResolveMsgId (uni_data->target_cache, Address,
                                    wsa_header_get_RelatesTo (soap));
        }
    }

  return SOAP_OK;
}

#endif

#endif

/** @} */
