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

#ifndef _WSEVENTING_H_
#define _WSEVENTING_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif


#include "soap_misc.h"
#include "ws4d_subscription.h"
#include "ws4d_subsmanager.h"

/**
 * gSoap WS-Eventing Plugin API
 *
 * @addtogroup WSEAPI Eventing-API
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

int wse_header_set_Identifier (struct soap *soap, const char *Identifier);

/**
 * @addtogroup WSEAPISERV Server API
 * @ingroup WSEAPI
 *
 * @{
 */

/**
 * @addtogroup WSESUBSM Subscription Manager
 * @ingroup WSEAPISERV
 *
 * @{
 */

extern const char *wse_subsm_plugin_id;


struct wse_subsm_plugin_data *wse_subsm_get_plugindata (struct soap *soap);

int wse_subsm_register_handle (struct soap *soap, const char *address);

typedef int
  (*NewSubscription_cb) (char *wsa_action,
                         struct wsa__EndpointReferenceType subscriber);

typedef int
  (*ReNewSubscription_cb) (char *wsa_action,
                           struct wsa__EndpointReferenceType subscriber);

typedef int
  (*UnSubscribe_cb) (char *wsa_action,
                     struct wsa__EndpointReferenceType subscriber);

struct wse_subsm_hooks_t
{
  NewSubscription_cb NewSubscription;
  ReNewSubscription_cb ReNewSubscription;
  UnSubscribe_cb UnSubscribe;
};

struct wse_subsm_hooks_t *wse_replace_subsmgr_hooks (struct soap *soap,
                                                     struct
                                                     wse_subsm_hooks_t
                                                     *hooks);

struct ws4d_subsmanager *wse_subsm_get (struct soap *soap);

extern const char *wse_subsend_deliveryfailure;
extern const char *wse_subsend_shutdown;
extern const char *wse_subsend_canceling;

/** @} */

/**
 * @addtogroup WSEEVSRC Event Source
 * @ingroup WSEAPISERV
 *
 * @{
 */

extern const char *wse_evsrc_plugin_id;

struct wse_evsrc_plugin_data
{
  struct soap *subsm;
};

struct wse_evsrc_plugin_data *wse_evsrc_get_plugindata (struct soap *soap);

int wse_evsrc_register_handle (struct soap *soap, struct soap *subsm);

/** @} */
/** @} */

/**
 * @addtogroup WSEAPICLI Client API
 * @ingroup WSEAPI
 *
 * @{
 */

/**
 * @addtogroup WSEEVSNK Event Sink
 * @ingroup WSEAPICLI
 *
 * @{
 */
extern const char *wse_evsnk_plugin_id;

struct wse_evsnk_plugin_data
{
  struct wse_evsnk_hooks_t *hooks;
};

struct wse_evsnk_plugin_data *wse_evsnk_get_plugindata (struct soap *soap);

typedef int
  (*EndSubscription_cb) (struct wsa__EndpointReferenceType * subs_man);

struct wse_evsnk_hooks_t
{
  EndSubscription_cb EndSubscription;
};

int wse_evsnk_register_handle (struct soap *soap,
                               struct wse_evsnk_hooks_t *hooks);

/** @} */

/**
 * @addtogroup WSESUBS Subscription Helper Functions
 * @ingroup WSEAPICLI
 *
 * @{
 */


struct wsa__EndpointReferenceType
  *wse_subscribe (struct soap *soap, const char *MsgId,
                  const char *serviceAddress, const char *endToAddress,
                  struct wse__DeliveryType *delivery,
                  struct ws4d_dur *expires, struct wse__FilterType *filter);

char *wse_subs_get_id (struct wsa__EndpointReferenceType *subs_man);

struct ws4d_dur
  *wse_subs_get_status (struct soap *soap, const char *MsgId,
                        const char *subs_man, const char *id);

int
wse_subs_renew (struct soap *soap, const char *MsgId, const char *subs_man,
                const char *id, struct ws4d_dur *expires);

int
wse_subs_unsubscribe (struct soap *soap, const char *MsgId,
                      const char *subs_man, const char *id);

int
wse_subsm_end_subs (struct soap *soap, char *MsgId,
                    struct ws4d_subscription *subs,
                    const char *status, char *reason);
/** @} */
/** @} */

/**
 * @addtogroup WSEPROC Processing Functions
 * @ingroup WSEAPI
 *
 * @{
 */

int wse_process_subscribe_req (struct soap *soap,
                               struct _wse__Subscribe *req,
                               struct ws4d_subscription **subs);

int wse_process_subscribe_res (struct soap *soap, char *to,
                               struct ws4d_subscription *subs,
                               struct _wse__SubscribeResponse *res);


int wse_process_renew_req (struct soap *soap, struct _wse__Renew *req,
                           struct ws4d_subscription **subs_p);

int wse_process_renew_res (struct soap *soap, char *to,
                           struct ws4d_subscription *subs,
                           struct _wse__RenewResponse *res);


int wse_process_getstatus_req (struct soap *soap,
                               struct _wse__GetStatus *req,
                               struct ws4d_subscription **subs_p);

int wse_process_getstatus_res (struct soap *soap, char *to,
                               struct ws4d_subscription *subs,
                               struct _wse__GetStatusResponse *res);


int wse_process_unsubscribe_req (struct soap *soap,
                                 struct _wse__Unsubscribe *req);

int wse_process_unsubscribe_res (struct soap *soap, char *to);

int
wse_process_subscriptionend (struct soap *soap,
                             struct _wse__SubscriptionEnd *_res);
/** @} */
/**
 * @addtogroup WSEFAULT Fault Functions
 * @ingroup WSEAPI
 *
 * @{
 */
enum wse__FaultSubcodeValues
{
  wse__FaultSubcodeValues__wse__DeliveryModeRequestedUnavailable = 0,
  wse__FaultSubcodeValues__wse__InvalidExpirationTime = 1,
  wse__FaultSubcodeValues__wse__UnsupportedExpirationType = 2,
  wse__FaultSubcodeValues__wse__FilteringNotSupported = 3,
  wse__FaultSubcodeValues__wse__FilteringRequestedUnavailable = 4,
  wse__FaultSubcodeValues__wse__EventSourceUnableToProcess = 5,
  wse__FaultSubcodeValues__wse__UnableToRenew = 6,
  wse__FaultSubcodeValues__wse__InvalidMessage = 7
};

int wse_error (struct soap *soap, enum wse__FaultSubcodeValues fault);
/** @} */


/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /* _WSEVENTING_H_ */
