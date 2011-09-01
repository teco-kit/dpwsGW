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
 *
 *  Created on: 22.08.2008
 *      Author: Elmar Zeeb
 */

#ifndef WS4D_SUBSCRIPTION_H_
#define WS4D_SUBSCRIPTION_H_

#include "ws4d_misc.h"

/**
 * Subscription
 *
 * @addtogroup APISubscription Subscription
 * @ingroup WS4D_UTILS
 *
 * @{
 */


struct ws4d_delivery_type
{
  const char *DeliveryMode;
  void *data;
};

struct ws4d_filter_type
{
  const char *FilterType;
  void *data;
};

struct ws4d_subscription
{
  struct ws4d_list_node list;
  const char *id;
  const char *subsm;
  const char *endto;
  struct ws4d_delivery_type *delivery_mode;
  struct ws4d_filter_type *filter_mode;
  ws4d_time expiration;
  ws4d_alloc_list alist;
};

/**
 * TODO: add documentation
 */
int ws4d_subs_init (struct ws4d_subscription *subs, const char *id);

/**
 * TODO: add documentation
 */
int ws4d_subs_done (struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
#define ws4d_subs_get_alist(subs) \
  (&subs->alist)

/**
 * TODO: add documentation
 */
const char *ws4d_subs_get_id (struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int ws4d_subs_set_endto (struct ws4d_subscription *subs, const char *endto);

/**
 * TODO: add documentation
 */
const char *ws4d_subs_get_subsm (struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int ws4d_subs_set_subsm (struct ws4d_subscription *subs, const char *subsm);

/**
 * TODO: add documentation
 */
ws4d_time ws4d_subs_get_expires (struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int ws4d_subs_set_expires (struct ws4d_subscription *subs, ws4d_time exptime);

/**
 * TODO: add documentation
 */
int ws4d_subs_dup (struct ws4d_subscription *src,
                   struct ws4d_subscription *dst);


/**
 * Subscription list
 *
 * @addtogroup APISubscriptionList Subscription list
 * @ingroup APISubscription
 *
 * @{
 */


/**
 * TODO: add documentation
 */
struct ws4d_subscription *ws4d_subslist_initsubs (struct ws4d_list_node *list,
                                                  const char *id,
                                                  ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
int ws4d_subslist_rmsubs (struct ws4d_list_node *list, const char *id);

/**
 * TODO: add documentation
 */
struct ws4d_subscription *ws4d_subslist_getsubs (struct ws4d_list_node *list,
                                                 const char *id);

/**
 * TODO: add documentation
 */
int ws4d_subslist_dup (struct ws4d_list_node *src, struct ws4d_list_node *dst,
                       ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
int ws4d_subslist_clear (struct ws4d_list_node *list);

/** @} */

/** @} */

#endif /* WS4D_SUBSCRIPTION_H_ */
