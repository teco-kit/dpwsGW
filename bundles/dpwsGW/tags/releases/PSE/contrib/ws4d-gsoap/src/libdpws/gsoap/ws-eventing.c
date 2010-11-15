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

#include <stdsoap2.h>
#include "soap_misc.h"

#include "wse.nsmap"
#include "ws-addressing.h"
#include "ws-eventing.h"

/**
 * @addtogroup Internals Internals
 *
 * @{
 */

 /**
 * @addtogroup WSEINTSUBSM Subscription Manager
 * @ingroup Internals
 *
 * @{
 */


#ifdef WSE_SERVER

#include "ws4d_subsmanager.h"

#define WSE_SUBSM_PLUGIN_ID "WS-Eventing-SubscriptionManager-PLUGIN-0.1"
const char *wse_subsm_plugin_id = WSE_SUBSM_PLUGIN_ID;

struct wse_subsm_plugin_data
{
  struct ws4d_subsmanager subsm;
  struct wse_subsm_hooks_t *hooks;
  ws4d_alloc_list alist;
};


/**
 * Function to get Subscription Manager plugin data from soap handle
 *
 * @return soap soap handle
 */
INLINE struct wse_subsm_plugin_data *
wse_subsm_get_plugindata (struct soap *soap)
{
  return (struct wse_subsm_plugin_data *) soap_lookup_plugin (soap,
                                                              WSE_SUBSM_PLUGIN_ID);
}

/**
 * Function is calles by soap_done to delete plugin data
 *
 * @param soap soap handel
 * @param p plugin data
 */
static void
wse_subsm_plugin_delete (struct soap *soap, struct soap_plugin *p)
{
  struct wse_subsm_plugin_data *data = p->data;

  ws4d_subsm_done (&data->subsm);

  ws4d_alloclist_done (&data->alist);
  SOAP_FREE (soap, p->data);
}

static int
wse_subsm_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg)
{
  p->id = wse_subsm_plugin_id;
  p->data =
    (void *) SOAP_MALLOC (soap, sizeof (struct wse_subsm_plugin_data));
  p->fdelete = wse_subsm_plugin_delete;
  if (p->data)
    {
      struct Namespace *temp_namespaces = NULL;
      struct wse_subsm_plugin_data *data = p->data;

      ws4d_subsm_init (&data->subsm, (char *) arg);
      WS4D_ALLOCLIST_INIT (&data->alist);
      data->hooks = NULL;

      temp_namespaces =
        soap_extend_namespaces ((struct Namespace *) soap->namespaces,
                                wse_namespaces, &data->alist);
      if (temp_namespaces)
        {
          soap_set_namespaces (soap, temp_namespaces);
        }
    }
  return SOAP_OK;
}


int
wse_subsm_register_handle (struct soap *soap, const char *address)
{
  return soap_register_plugin_arg (soap, wse_subsm_plugin_init,
                                   (void *) address);
}

struct wse_subsm_hooks_t *
wse_replace_subsmgr_hooks (struct soap *soap, struct wse_subsm_hooks_t *hooks)
{
  struct wse_subsm_hooks_t *oldhooks = NULL;
  struct wse_subsm_plugin_data *data = wse_subsm_get_plugindata (soap);

  if (hooks)
    {
      oldhooks = data->hooks;
      data->hooks = hooks;
    }

  return oldhooks;
}

struct ws4d_subsmanager *
wse_subsm_get (struct soap *soap)
{
  struct wse_subsm_plugin_data *data = wse_subsm_get_plugindata (soap);

  if (data)
    return &data->subsm;
  else
    return NULL;
}

static int
wse_subsm_process_deliveryMode (struct ws4d_subsmanager *subsm,
                                struct soap *src,
                                struct ws4d_subscription *subs,
                                struct wse__DeliveryType *Delivery)
{
  struct ws4d_event_delivery *found_delivery = NULL;

  if (!subsm || !src || !Delivery)
    return wse_error (src,
                      wse__FaultSubcodeValues__wse__DeliveryModeRequestedUnavailable);

  /* if delivery mode is given */
  if (Delivery->Mode)
    {
      register struct ws4d_event_delivery *deliveryType, *next;
      const char *delivery_mode;

      delivery_mode = Delivery->Mode;

      ws4d_list_foreach (deliveryType, next, &subsm->supp_delivery_list,
                         struct ws4d_event_delivery, list)
      {
        if (!strcmp (deliveryType->Mode, delivery_mode))
          {
            found_delivery = deliveryType;
            break;
          }
      }
    }
  else                          /* use default delivery mode */
    {
      if (subsm->default_delivery)
        {
          found_delivery = subsm->default_delivery;
        }
    }

  /* if delivery mode was found */
  if (found_delivery)
    {
      if (found_delivery->fprocessDelivery)
        {
          int err;

          subs->delivery_mode =
            ws4d_malloc_alist (sizeof (struct ws4d_delivery_type),
                               ws4d_subsm_get_alist (subsm));
          if (!subs->delivery_mode)
            {
              return SOAP_ERR;
            }

          err = found_delivery->fprocessDelivery (subs, Delivery);
          if (err == WS4D_OK)
            return SOAP_OK;
          else
            return SOAP_ERR;
        }
      else
        {
          return SOAP_ERR;
        }

    }

  return wse_error (src,
                    wse__FaultSubcodeValues__wse__DeliveryModeRequestedUnavailable);
}

static int
wse_subsm_process_filterType (struct ws4d_subsmanager *subsm,
                              struct soap *src,
                              struct ws4d_subscription *subs,
                              struct wse__FilterType *Filter)
{
  struct ws4d_event_filter *found_filter = NULL;

  if (!subsm || !src || !Filter)
    return wse_error (src,
                      wse__FaultSubcodeValues__wse__FilteringRequestedUnavailable);

  /* if filter is given */
  if (Filter->Dialect)
    {
      register struct ws4d_event_filter *filterType, *next;
      const char *filter_dialect;

      filter_dialect = Filter->Dialect;

      ws4d_list_foreach (filterType, next, &subsm->supp_filter_list,
                         struct ws4d_event_filter, list)
      {
        if (!strcmp (filterType->Type, filter_dialect))
          {
            found_filter = filterType;
            break;
          }
      }

    }
  else                          /* use default filter action */
    {
      if (subsm->default_filter)
        {
          found_filter = subsm->default_filter;
        }
    }

  /* if action filter was found */
  if (found_filter)
    {
      if (found_filter->fprocessFilterType)
        {
          int err;

          subs->filter_mode =
            ws4d_malloc_alist (sizeof (struct ws4d_filter_type),
                               ws4d_subsm_get_alist (subsm));
          if (!subs->filter_mode)
            {
              return SOAP_ERR;
            }

          err = found_filter->fprocessFilterType (subs, Filter);
          if (err == WS4D_OK)
            return SOAP_OK;
          else
            return SOAP_ERR;
        }
      else
        {
          return SOAP_OK;
        }
    }

  return wse_error (src,
                    wse__FaultSubcodeValues__wse__FilteringRequestedUnavailable);
}

#define WSE_SUBSEND_DELIVERYFAILURE "http://schemas.xmlsoap.org/ws/2004/08/eventing/DeliveryFailure"
#define WSE_SUBSEND_SHUTDOWN "http://schemas.xmlsoap.org/ws/2004/08/eventing/SourceShuttingDown"
#define WSE_SUBSEND_CANCELING "http://schemas.xmlsoap.org/ws/2004/08/eventing/SourceCanceling"

const char *wse_subsend_deliveryfailure = WSE_SUBSEND_DELIVERYFAILURE;
const char *wse_subsend_shutdown = WSE_SUBSEND_SHUTDOWN;
const char *wse_subsend_canceling = WSE_SUBSEND_CANCELING;

/*TODO: implement reason */
int
wse_subsm_end_subs (struct soap *soap, char *MsgId,
                    struct ws4d_subscription *subs, const char *status,
                    char *reason)
{
  int err;
  struct wse_subsm_plugin_data *data = wse_subsm_get_plugindata (soap);

  WS4D_UNUSED_PARAM (reason);

  soap_assert (NULL, data && MsgId && status, SOAP_ERR);

  if (subs->endto)
    {
      struct soap tmp;
      struct _wse__SubscriptionEnd SubscriptionEnd;

      soap_init (&tmp);
      soap_set_namespaces (soap, wse_namespaces);

      /* Prepare SOAP Header */
      wsa_header_gen_request (&tmp, MsgId, subs->endto,
                              "http://schemas.xmlsoap.org/ws/2004/08/eventing/SubscriptionEnd",
                              NULL, NULL, sizeof (struct SOAP_ENV__Header));

      soap_default__wse__SubscriptionEnd (&tmp, &SubscriptionEnd);
      SubscriptionEnd.SubscriptionManager.Address =
        ws4d_subsm_getaddr (&data->subsm);
      if (status)
        SubscriptionEnd.Status = (char *) status;

      /* TODO: Reason */

      soap_send___wse__SubscriptionEnd (&tmp, subs->endto,
                                        NULL, &SubscriptionEnd);
      soap_end (&tmp);
      soap_done (&tmp);
    }

  ws4d_subsm_lock (&data->subsm);
  err = ws4d_subsm_expiresubs (&data->subsm, subs);
  ws4d_subsm_unlock (&data->subsm);

  return err;
}

#endif

/** @} */

/**
 * @addtogroup WSEINTEVSRC Event Source
 * @ingroup Internals
 *
 * @{
 */


#ifdef WSE_SERVER

#define WSE_EVSRC_PLUGIN_ID "WS-Eventing-EventSource-PLUGIN-0.1"
const char *wse_evsrc_plugin_id = WSE_EVSRC_PLUGIN_ID;
INLINE struct wse_evsrc_plugin_data *
wse_evsrc_get_plugindata (struct soap *soap)
{
  return (struct wse_evsrc_plugin_data *)
    soap_lookup_plugin (soap, WSE_EVSRC_PLUGIN_ID);
}

static void
wse_evsrc_plugin_delete (struct soap *soap, struct soap_plugin *p)
{
  WS4D_UNUSED_PARAM(soap);

  free (p->data);
}

static int
wse_evsrc_plugin_copy (struct soap *soap,
                       struct soap_plugin *dst, struct soap_plugin *src)
{
  struct wse_evsrc_plugin_data *sdata = src->data;

  WS4D_UNUSED_PARAM (soap);

  dst->data = malloc (sizeof (struct wse_evsrc_plugin_data));
  memcpy (dst->data, sdata, sizeof (struct wse_evsrc_plugin_data));

  return SOAP_OK;
}

static int
wse_evsrc_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg)
{
  struct wse_subsm_plugin_data *subsm_data =
    wse_subsm_get_plugindata ((struct soap *) arg);

  p->id = wse_evsrc_plugin_id;
  p->data = malloc (sizeof (struct wse_evsrc_plugin_data));
  memset (p->data, 0, sizeof (struct wse_evsrc_plugin_data));
  p->fcopy = wse_evsrc_plugin_copy;
  p->fdelete = wse_evsrc_plugin_delete;

  if (p->data)
    {
      struct Namespace *temp_namespaces = NULL;
      struct wse_evsrc_plugin_data *data = p->data;

      data->subsm = (struct soap *) arg;

      temp_namespaces =
        soap_extend_namespaces ((struct Namespace *) soap->namespaces,
                                wse_namespaces, &subsm_data->alist);
      if (temp_namespaces)
        {
          soap_set_namespaces (soap, temp_namespaces);
        }
    }
  return SOAP_OK;
}

int
wse_evsrc_register_handle (struct soap *soap, struct soap *subsm)
{
  struct wse_subsm_plugin_data *data = wse_subsm_get_plugindata (subsm);
  if (!data)
    return SOAP_ERR;
  return soap_register_plugin_arg (soap, wse_evsrc_plugin_init, subsm);
}

#endif

/** @} */

/**
 * @addtogroup WSEINTEVSNK Event Sink
 * @ingroup Internals
 *
 * @{
 */


#ifdef WSE_CLIENT

#define WSE_EVSNK_PLUGIN_ID "WS-Eventing-EventSink-PLUGIN-0.1"
const char *wse_evsnk_plugin_id = WSE_EVSNK_PLUGIN_ID;
INLINE struct wse_evsnk_plugin_data *
wse_evsnk_get_plugindata (struct soap *soap)
{
  return (struct wse_evsnk_plugin_data *)
    soap_lookup_plugin (soap, WSE_EVSNK_PLUGIN_ID);
}

static void
wse_evsnk_plugin_delete (struct soap *soap, struct soap_plugin *p)
{
  SOAP_FREE (soap, p->data);
}

static int
wse_evsnk_plugin_copy (struct soap *soap,
                       struct soap_plugin *dst, struct soap_plugin *src)
{
  struct wse_evsnk_plugin_data *ddata = dst->data;
  struct wse_evsnk_plugin_data *sdata = src->data;

  WS4D_UNUSED_PARAM (soap);

  ddata->hooks = sdata->hooks;

  return SOAP_OK;
}

static int
wse_evsnk_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg)
{
  p->id = wse_evsnk_plugin_id;
  p->data =
    (void *) SOAP_MALLOC (soap, sizeof (struct wse_evsnk_plugin_data));
  p->fcopy = wse_evsnk_plugin_copy;
  p->fdelete = wse_evsnk_plugin_delete;
  if (p->data)
    {
      struct wse_evsnk_plugin_data *data = p->data;
      data->hooks = (struct wse_evsnk_hooks_t *) arg;
    }
  return SOAP_OK;
}

int
wse_evsnk_register_handle (struct soap *soap, struct wse_evsnk_hooks_t *hooks)
{
  return soap_register_plugin_arg (soap, wse_evsnk_plugin_init, hooks);
}

#endif

/** @} */

#ifdef WSE_SERVER

/**
 * @addtogroup WSEINTFAULT Fault Functions
 * @ingroup Internals
 *
 * @{
 */

static const struct soap_code_map soap_codes_wse__FaultSubcodeValues[] = {
  {
   (long) wse__FaultSubcodeValues__wse__DeliveryModeRequestedUnavailable,
   "wse:DeliveryModeRequestedUnavailable"},
  {
   (long) wse__FaultSubcodeValues__wse__InvalidExpirationTime,
   "wse:InvalidExpirationTime"},
  {
   (long) wse__FaultSubcodeValues__wse__UnsupportedExpirationType,
   "wse:UnsupportedExpirationType"},
  {
   (long) wse__FaultSubcodeValues__wse__FilteringNotSupported,
   "wse:FilteringNotSupported"},
  {
   (long) wse__FaultSubcodeValues__wse__FilteringRequestedUnavailable,
   "wse:FilteringRequestedUnavailable"},
  {
   (long) wse__FaultSubcodeValues__wse__EventSourceUnableToProcess,
   "wse:EventSourceUnableToProcess"},
  {
   (long) wse__FaultSubcodeValues__wse__UnableToRenew,
   "wse:UnableToRenew"},
  {
   (long) wse__FaultSubcodeValues__wse__InvalidMessage,
   "wse:InvalidMessage"},
  {
   0, NULL}
};

static const char *
soap_wse__FaultSubcodeValues2s (struct
                                soap *soap, enum wse__FaultSubcodeValues n)
{
  const char *s = soap_code_str (soap_codes_wse__FaultSubcodeValues,
                                 (long) n);
  if (s)
    return s;
  return soap_long2s (soap, (long) n);
}

int
wse_error (struct soap *soap, enum wse__FaultSubcodeValues fault)
{
  const char *code = soap_wse__FaultSubcodeValues2s (soap, fault);
  switch (fault)
    {
    case wse__FaultSubcodeValues__wse__DeliveryModeRequestedUnavailable:
      return wsa_sender_fault_subcode (soap, code,
                                       "The requested delivery mode is not supported",
                                       "");
    case wse__FaultSubcodeValues__wse__InvalidExpirationTime:
      return wsa_sender_fault_subcode (soap, code,
                                       "The expiration time requested is invalid",
                                       "");
    case wse__FaultSubcodeValues__wse__UnsupportedExpirationType:
      return wsa_sender_fault_subcode (soap, code,
                                       "Only expiration durations are supported",
                                       "");
    case wse__FaultSubcodeValues__wse__FilteringNotSupported:
      return wsa_sender_fault_subcode (soap, code,
                                       "Filtering is not supported", "");
    case wse__FaultSubcodeValues__wse__FilteringRequestedUnavailable:
      return wsa_sender_fault_subcode (soap, code,
                                       "The requested filter dialect is not supported",
                                       "");
    case wse__FaultSubcodeValues__wse__EventSourceUnableToProcess:
      return wsa_receiver_fault_subcode (soap, code, "", "");
    case wse__FaultSubcodeValues__wse__UnableToRenew:
      return wsa_receiver_fault_subcode (soap, code, "", "");
    case wse__FaultSubcodeValues__wse__InvalidMessage:
      return wsa_sender_fault_subcode (soap, code,
                                       "The message is not valid and cannot be processed",
                                       "");
    }

  return SOAP_FAULT;
}
#endif
/** @} */

/**
 * @addtogroup WSEINTPROC Processing Functions
 * @ingroup Internals
 *
 * @{
 */

#ifdef WSE_SERVER

int
wse_process_subscribe_req (struct soap *soap, struct _wse__Subscribe *req,
                           struct ws4d_subscription **subs)
{
  int err = 0;
  ws4d_time expires;
  struct ws4d_subscription *new_subs = NULL;
  struct wse_evsrc_plugin_data *src_data = wse_evsrc_get_plugindata (soap);
  struct wse_subsm_plugin_data *subsm_data = NULL;

  if (!src_data)
    return wse_error (soap,
                      wse__FaultSubcodeValues__wse__EventSourceUnableToProcess);

  subsm_data = wse_subsm_get_plugindata (src_data->subsm);
  if (!subsm_data)
    return wse_error (soap,
                      wse__FaultSubcodeValues__wse__EventSourceUnableToProcess);

  if (!req)
    return SOAP_OK;

  if (!req->Delivery || !req->Filter)
    return wse_error (soap,
                      wse__FaultSubcodeValues__wse__EventSourceUnableToProcess);

  ws4d_subsm_lock (&subsm_data->subsm);
  new_subs = ws4d_subsm_allocsubs (&subsm_data->subsm);

  /* Process Delivery Type */
  err =
    wse_subsm_process_deliveryMode (&subsm_data->subsm, soap, new_subs,
                                    req->Delivery);
  if (err)
    {
      ws4d_subsm_unlock (&subsm_data->subsm);
      return err;
    }

  /* Process Filter Type */
  err =
    wse_subsm_process_filterType (&subsm_data->subsm, soap, new_subs,
                                  req->Filter);
  if (err)
    {
      ws4d_subsm_unlock (&subsm_data->subsm);
      return err;
    }

  /* Process Expires */
  if (req->Expires)
    {
      ws4d_dur_to_s (req->Expires, &expires);
    }
  else
    {
      expires = 3600;
    }
  ws4d_subs_set_expires (new_subs, expires + ws4d_systime_s ());

  /* Process EndTo */
  if (req->EndTo)
    ws4d_subs_set_endto (new_subs, req->EndTo->Address);

  /* register subscription */
  if (ws4d_subsm_regsubs (&subsm_data->subsm, new_subs))
    {
      ws4d_subsm_unlock (&subsm_data->subsm);
      return wse_error (soap,
                        wse__FaultSubcodeValues__wse__EventSourceUnableToProcess);
    }

  if (subs)
    *subs = new_subs;

  ws4d_subsm_unlock (&subsm_data->subsm);
  return SOAP_OK;
}

int
wse_process_subscribe_res (struct soap *soap,
                           char *to, struct ws4d_subscription *subs,
                           struct _wse__SubscribeResponse *res)
{
  struct wsa__ReferenceParametersType *ep_parm = NULL;
  struct wsa_ReferenceParameterAny *ep_parm_any = NULL;
  const char *subs_id;
  struct wse_evsrc_plugin_data *src_data = wse_evsrc_get_plugindata (soap);
  struct wse_subsm_plugin_data *subsm_data = NULL;

  if (!src_data)
    return SOAP_ERR;
  subsm_data = wse_subsm_get_plugindata (src_data->subsm);

  if (!subsm_data)
    return SOAP_ERR;

  if(SOAP_OK==wsa_response (soap, NULL, to,
                           "http://schemas.xmlsoap.org/ws/2004/08/eventing/SubscribeResponse",
                           wsa_header_get_MessageId (soap),
                           sizeof (struct SOAP_ENV__Header)))
{
  ep_parm = soap_malloc (soap, sizeof (struct wsa__ReferenceParametersType));
  subs_id = ws4d_subs_get_id (subs);
  soap_default_wsa__ReferenceParametersType (soap, ep_parm);

  ep_parm->__sizepar = 1;
  ep_parm_any = soap_malloc (soap, sizeof (struct wsa_ReferenceParameterAny));
  if (!ep_parm_any)
    return SOAP_EOM;

  ep_parm->__par = ep_parm_any;
  /* TODO: fix this creepy hack !!! */
#if defined(DEVPROF_2009_01)
  ep_parm_any->__type = 154;
#else
  ep_parm_any->__type = 122;
#endif
  ep_parm_any->__any = (char *) subs_id;

  res->SubscriptionManager.Address = ws4d_subsm_getaddr (&subsm_data->subsm);
  res->SubscriptionManager.ReferenceParameters = ep_parm;
  ws4d_s_to_dur (ws4d_subs_get_expires (subs) - ws4d_systime_s (),
                 &res->Expires);

  wsa_header_set_Action (soap,
                         "http://schemas.xmlsoap.org/ws/2004/08/eventing/SubscribeResponse"); //??? not done in wsa_gen_header_response
 }
 return soap->error;
}

int
wse_process_renew_req (struct soap *soap,
                       struct _wse__Renew *req,
                       struct ws4d_subscription **subs_p)
{
  ws4d_time expires;
  struct ws4d_subscription *subs = NULL;
  struct wse_subsm_plugin_data *data = wse_subsm_get_plugindata (soap);

  soap_assert (soap, data && soap->header
               && soap->header->wse__Identifier,
               SOAP_ERR) ws4d_subsm_lock (&data->subsm);
  subs = ws4d_subsm_getsubs (&data->subsm, soap->header->wse__Identifier);

  if (subs)
    {
      if (req->Expires)
        {
          ws4d_dur_to_s (req->Expires, &expires);
        }
      else
        {
          expires = 3600;
        }
      ws4d_subs_set_expires (subs, expires + ws4d_systime_s ());
    }
  else
    {
      ws4d_subsm_unlock (&data->subsm);
      return wse_error (soap, wse__FaultSubcodeValues__wse__InvalidMessage);
    }

  if (subs_p)
    *subs_p = subs;

  ws4d_subsm_unlock (&data->subsm);
  return SOAP_OK;
}


int
wse_process_renew_res (struct soap *soap, char *to,
                       struct ws4d_subscription *subs,
                       struct _wse__RenewResponse *res)
{
  struct ws4d_dur *expires;

  wsa_header_gen_response (soap, NULL, to,
                           "http://schemas.xmlsoap.org/ws/2004/08/eventing/RenewResponse",
                           wsa_header_get_MessageId
                           (soap), sizeof (struct SOAP_ENV__Header));

  expires = soap_malloc (soap, sizeof (struct ws4d_dur));
  ws4d_s_to_dur (ws4d_subs_get_expires (subs) - ws4d_systime_s (), expires);
  res->Expires = expires;

  return SOAP_OK;
}

int
wse_process_getstatus_req (struct soap *soap,
                           struct _wse__GetStatus *req,
                           struct ws4d_subscription **subs_p)
{
  struct ws4d_subscription *subs = NULL;
  struct wse_subsm_plugin_data *data = wse_subsm_get_plugindata (soap);

  WS4D_UNUSED_PARAM (req);

  soap_assert (soap, data && soap->header
               && soap->header->wse__Identifier, SOAP_ERR);

  ws4d_subsm_lock (&data->subsm);
  subs = ws4d_subsm_getsubs (&data->subsm, soap->header->wse__Identifier);
  if (!subs)
    {
      ws4d_subsm_unlock (&data->subsm);
      return wse_error (soap, wse__FaultSubcodeValues__wse__InvalidMessage);
    }

  if (subs_p)
    *subs_p = subs;

  ws4d_subsm_unlock (&data->subsm);
  return SOAP_OK;
}

int
wse_process_getstatus_res (struct soap *soap,
                           char *to,
                           struct ws4d_subscription *subs,
                           struct _wse__GetStatusResponse *res)
{
  struct ws4d_dur *expires;

  wsa_header_gen_response (soap, NULL, to,
                           "http://schemas.xmlsoap.org/ws/2004/08/eventing/GetStatusResponse",
                           wsa_header_get_MessageId
                           (soap), sizeof (struct SOAP_ENV__Header));

  expires = soap_malloc (soap, sizeof (struct ws4d_dur));
  ws4d_s_to_dur (ws4d_subs_get_expires (subs) - ws4d_systime_s (), expires);
  res->Expires = expires;

  return SOAP_OK;
}

int
wse_process_unsubscribe_req (struct soap *soap,
                             struct _wse__Unsubscribe *__req)
{
  struct ws4d_subscription *subs = NULL;
  struct wse_subsm_plugin_data *data = wse_subsm_get_plugindata (soap);

  WS4D_UNUSED_PARAM (__req);

  soap_assert (soap, data && soap->header
               && soap->header->wse__Identifier, SOAP_ERR);

  ws4d_subsm_lock (&data->subsm);
  subs = ws4d_subsm_getsubs (&data->subsm, soap->header->wse__Identifier);

  if (subs)
    {
      ws4d_subsm_expiresubs (&data->subsm, subs);
    }
  else
    {
      ws4d_subsm_unlock (&data->subsm);
      return wse_error (soap, wse__FaultSubcodeValues__wse__InvalidMessage);
    }

  ws4d_subsm_unlock (&data->subsm);

  return SOAP_OK;
}

int
wse_process_unsubscribe_res (struct soap *soap, char *to)
{
  return wsa_response (soap, NULL, to,
                                  "http://schemas.xmlsoap.org/ws/2004/08/eventing/UnsubscribeResponse",
                                  wsa_header_get_MessageId
                                  (soap), sizeof (struct SOAP_ENV__Header));;
}
#endif

#ifdef WSE_CLIENT
int
wse_process_subscriptionend (struct soap *soap,
                             struct _wse__SubscriptionEnd *_res)
{
  struct wse_evsnk_plugin_data *data = wse_evsnk_get_plugindata (soap);
  if (_res && data->hooks && data->hooks->EndSubscription)
    data->hooks->EndSubscription (&_res->SubscriptionManager);
  return SOAP_OK;
}
#endif

/** @} */


/**
 * @addtogroup WSEINTSUBS Subscription Helper Functions
 * @ingroup Internals
 *
 * @{
 */

#ifdef WSE_CLIENT

struct wsa__EndpointReferenceType *
wse_subscribe (struct soap *soap, const char *MsgId,
               const char *serviceAddress, const char *endToAddress,
               struct wse__DeliveryType *delivery, struct ws4d_dur *expires,
               struct wse__FilterType *filter)
{
  struct _wse__Subscribe *req = NULL;
  struct _wse__SubscribeResponse *res = NULL;

  /* Prepare SOAP Header */
  soap_set_namespaces (soap, wse_namespaces);
  wsa_header_gen_request (soap, MsgId,
                          serviceAddress,
                          "http://schemas.xmlsoap.org/ws/2004/08/eventing/Subscribe",
                          NULL, NULL, sizeof (struct SOAP_ENV__Header));
  /* Prepare Body */
  req =
    (struct _wse__Subscribe *) soap_malloc (soap,
                                            sizeof (struct _wse__Subscribe));
  soap_default__wse__Subscribe (soap, req);
  req->Delivery = delivery;
  if (expires->days != 0
      || expires->hours != 0
      || expires->minutes != 0
      || expires->months != 0 || expires->seconds != 0 || expires->years != 0)
    {
      req->Expires = expires;
    }
  else
    {
      req->Expires = NULL;
    }
  req->Filter = filter;
  if (endToAddress)
    {
      req->EndTo->Address = (char *) endToAddress;
    }

  res =
    (struct _wse__SubscribeResponse *)
    soap_malloc (soap, sizeof (struct _wse__SubscribeResponse));

  soap_default__wse__SubscribeResponse (soap, res);
  soap_call___wse__Subscribe (soap, serviceAddress, NULL, req, res);
  if (res)
    {
      memcpy (expires, &res->Expires, sizeof (struct ws4d_dur));
      return &res->SubscriptionManager;
    }
  else
    {
      expires = NULL;
      return NULL;
    }
}

#endif

int
wse_header_set_Identifier (struct soap *soap, const char *Identifier)
{
  if (!soap || !soap->header | !Identifier)
    return SOAP_ERR;

  soap->header->wse__Identifier = soap_strdup (soap, Identifier);
  return SOAP_OK;
}

#ifdef WSE_CLIENT

int
wse_subs_renew (struct soap *soap, const char *MsgId, const char *subs_man,
                const char *id, struct ws4d_dur *expires)
{
  struct _wse__Renew *req = NULL;
  struct _wse__RenewResponse *res = NULL;

  /* Prepare SOAP Header */
  soap_set_namespaces (soap, wse_namespaces);
  wsa_header_gen_request (soap, MsgId, subs_man,
                          "http://schemas.xmlsoap.org/ws/2004/08/eventing/Renew",
                          NULL, NULL, sizeof (struct SOAP_ENV__Header));
  wse_header_set_Identifier (soap, id);

  /* Prepare Body */
  req =
    (struct _wse__Renew *) soap_malloc (soap, sizeof (struct _wse__Renew));
  soap_default__wse__Renew (soap, req);
  if (expires->days != 0
      || expires->hours != 0
      || expires->minutes != 0
      || expires->months != 0 || expires->seconds != 0 || expires->years != 0)
    {
      req->Expires = expires;
    }
  else
    {
      req->Expires = NULL;
    }
  res = (struct _wse__RenewResponse *)
    soap_malloc (soap, sizeof (struct _wse__RenewResponse));
  soap_call___wse__Renew (soap, subs_man, NULL, req, res);

  if (res)
    {
      memcpy (expires, &res->Expires, sizeof (struct ws4d_dur));
    }

  return soap->error;
}

#endif

char *
wse_subs_get_id (struct wsa__EndpointReferenceType *subs_man)
{
  int i = 0;
  char *id = NULL;


  if (!subs_man || !subs_man->ReferenceParameters
      || !(subs_man->ReferenceParameters->__sizepar > 0))
    return NULL;

  for (i = 0; i < subs_man->ReferenceParameters->__sizepar; i++)
    {
#if defined(DEVPROF_2009_01)
      if (subs_man->ReferenceParameters->__par[i].__type == 154)
#else
      if (subs_man->ReferenceParameters->__par[i].__type == 122)
#endif
        {
          id = subs_man->ReferenceParameters->__par[i].__any;
        }
    }

  return id;
}

#ifdef WSE_CLIENT

struct ws4d_dur *
wse_subs_get_status (struct soap *soap, const char *MsgId,
                     const char *subs_man, const char *id)
{
  struct _wse__GetStatus req;
  struct _wse__GetStatusResponse res;
  soap_default__wse__GetStatus (soap, &req);

  /* Prepare SOAP Header */
  soap_set_namespaces (soap, wse_namespaces);
  wsa_header_gen_request (soap, MsgId, subs_man,
                          "http://schemas.xmlsoap.org/ws/2004/08/eventing/GetStatus",
                          NULL, NULL, sizeof (struct SOAP_ENV__Header));
  wse_header_set_Identifier (soap, id);

  soap_call___wse__GetStatus (soap, subs_man, NULL, &req, &res);

  return res.Expires;
}

int
wse_subs_unsubscribe (struct soap *soap, const char *MsgId,
                      const char *subs_man, const char *id)
{
  struct _wse__Unsubscribe req;

  soap_default__wse__Unsubscribe (soap, &req);

  /* Prepare SOAP Header */
  soap_set_namespaces (soap, wse_namespaces);
  wsa_header_gen_request (soap, MsgId, subs_man,
                          "http://schemas.xmlsoap.org/ws/2004/08/eventing/Unsubscribe",
                          NULL, NULL, sizeof (struct SOAP_ENV__Header));
  wse_header_set_Identifier (soap, id);

  return soap_call___wse__Unsubscribe (soap, subs_man, NULL, &req, NULL);
}

#endif

/** @} */

/**
 * @addtogroup WSEINTDURSER Custom Duration Serializer
 * @ingroup Internals
 *
 * @{
 */



void
soap_default_xsd__duration (struct soap *soap, struct ws4d_dur *a)
{
  WS4D_UNUSED_PARAM (soap);

  memset (a, 0, sizeof (struct ws4d_dur));
}

void
soap_serialize_xsd__duration (struct soap *soap, struct ws4d_dur const *a)
{
  WS4D_UNUSED_PARAM (soap);
  WS4D_UNUSED_PARAM (a);
}

int
soap_out_xsd__duration (struct soap *soap,
                        const char *tag, int id,
                        const struct ws4d_dur *a, const char *type)
{
  if (!soap_element_begin_out
      (soap, tag,
       soap_embedded_id (soap, id, a, SOAP_TYPE_xsd__duration), type))
    {
      int ret = 0;
      char buf[WS4D_XSDDT_LEN];
      memset (buf, 0, WS4D_XSDDT_LEN);
      ws4d_dur_to_xsddt ((struct ws4d_dur *) a, buf, WS4D_XSDDT_LEN);
      ret = soap_string_out (soap, buf, 0);
      if (ret)
        return soap->error;
    }
  else
    return soap->error;
  return soap_element_end_out (soap, tag);
}


struct ws4d_dur *
soap_in_xsd__duration (struct soap *soap,
                       const char *tag, struct ws4d_dur *a, const char *type)
{
  if (soap_element_begin_in (soap, tag, 0, type))
    return NULL;

  a =
    (struct ws4d_dur *) soap_id_enter (soap, soap->id, a,
                                       SOAP_TYPE_xsd__duration,
                                       sizeof (struct ws4d_dur), 0, NULL,
                                       NULL, NULL);
  if (*soap->href)
    a =
      (struct ws4d_dur *) soap_id_forward (soap, soap->href,
                                           a, 0,
                                           SOAP_TYPE_xsd__duration,
                                           0, sizeof (struct ws4d_dur), 0,
                                           NULL);
  else if (a)
    {
      char *s = soap_strdup (soap, soap_value (soap));
      memset ((void *) a, 0, sizeof (struct ws4d_dur));
      if (s)
        {
          if (ws4d_xsddt_to_dur (s, a) != WS4D_OK)
            {
              return NULL;
            }
        }
      else
        {
          return NULL;
        }
    }

  if (soap->body && soap_element_end_in (soap, tag))
    return NULL;

  return a;
}

void
soap_default_wse__NonNegativeDurationType (struct soap *soap,
                                           struct ws4d_dur *a)
{
  WS4D_UNUSED_PARAM (soap);

  memset (a, 0, sizeof (struct ws4d_dur));
}

void
soap_serialize_wse__NonNegativeDurationType (struct soap *soap,
                                             struct ws4d_dur const *a)
{
  WS4D_UNUSED_PARAM (soap);
  WS4D_UNUSED_PARAM (a);
}

int
soap_out_wse__NonNegativeDurationType (struct soap *soap,
                                       const char *tag,
                                       int id,
                                       const struct ws4d_dur *a,
                                       const char *type)
{
  if (!soap_element_begin_out
      (soap, tag,
       soap_embedded_id (soap, id, a, SOAP_TYPE_xsd__duration), type))
    {
      int ret = 0;
      char buf[WS4D_XSDDT_LEN];
      memset (buf, 0, WS4D_XSDDT_LEN);
      ws4d_dur_to_xsddt ((struct ws4d_dur *) a, buf, WS4D_XSDDT_LEN);
      ret = soap_string_out (soap, buf, 0);
      if (ret)
        return soap->error;
    }
  else
    return soap->error;
  return soap_element_end_out (soap, tag);
}


struct ws4d_dur *
soap_in_wse__NonNegativeDurationType (struct soap *soap,
                                      const char *tag,
                                      struct ws4d_dur *a, const char *type)
{
  if (soap_element_begin_in (soap, tag, 0, type))
    return NULL;

  a =
    (struct ws4d_dur *) soap_id_enter (soap, soap->id, a,
                                       SOAP_TYPE_xsd__duration,
                                       sizeof (struct ws4d_dur), 0, NULL,
                                       NULL, NULL);
  if (*soap->href)
    a =
      (struct ws4d_dur *) soap_id_forward (soap, soap->href,
                                           a, 0,
                                           SOAP_TYPE_xsd__duration,
                                           0, sizeof (struct ws4d_dur), 0,
                                           NULL);
  else if (a)
    {
      const char *s = soap_value (soap);
      memset ((void *) a, 0, sizeof (struct ws4d_dur));
      if (s)
        {
          if (ws4d_xsddt_to_dur (s, a) != WS4D_OK)
            {
              return NULL;
            }
        }
      else
        {
          return NULL;
        }
    }
  if (soap->body && soap_element_end_in (soap, tag))
    return NULL;
  return a;
}

/** @} */

/** @} */
