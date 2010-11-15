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

#ifndef WS4D_SUBSMANAGER_H_
#define WS4D_SUBSMANAGER_H_

#include "ws4d_subscription.h"

/**
 * Subscription Manager
 *
 * @addtogroup APISubscriptionManager Subscription manager
 * @ingroup ingroup WS4D_UTILS
 *
 * @{
 */

typedef int (*ws4d_processDelivery_cb) (struct ws4d_subscription * subs,
                                        void *delivery_data);

typedef int (*ws4d_cleanDelivery_cb) (struct ws4d_subscription * subs);

struct ws4d_event_delivery
{
  struct ws4d_list_node list;
  const char *Mode;
  ws4d_processDelivery_cb fprocessDelivery;
  ws4d_cleanDelivery_cb fcleanDelivery;
};

typedef int (*ws4d_processFilter_cb) (struct ws4d_subscription * subs,
                                      void *filter_data);

typedef int (*ws4d_cleanFilter_cb) (struct ws4d_subscription * subs);

struct ws4d_event_filter
{
  struct ws4d_list_node list;
  const char *Type;
  ws4d_processFilter_cb fprocessFilterType;
  ws4d_cleanFilter_cb fcleanFilterType;
};

struct ws4d_subsmanager
{
  struct ws4d_list_node subs_list;
  struct ws4d_list_node supp_delivery_list;
  struct ws4d_event_delivery *default_delivery;
  struct ws4d_list_node supp_filter_list;
  struct ws4d_event_filter *default_filter;
  char *address;
  ws4d_alloc_list alist;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
};

/**
 * TODO: add documentation
 */
int ws4d_subsm_init (struct ws4d_subsmanager *subsm, const char *addr);

/**
 * TODO: add documentation
 */
int ws4d_subsm_done (struct ws4d_subsmanager *subsm);

#ifdef WITH_MUTEXES
void ws4d_subsm_lock (struct ws4d_subsmanager *subsm);
void ws4d_subsm_unlock (struct ws4d_subsmanager *subsm);
#else /*WITH_MUTEXES */

/**
 * TODO: add documentation
 */
#define ws4d_subsm_lock(subsm)

/**
 * TODO: add documentation
 */
#define ws4d_subsm_unlock(subsm)

#endif /*WITH_MUTEXES */

/**
 * TODO: add documentation
 */
#define ws4d_subsm_get_alist(subsm) \
  (&subsm->alist)

/**
 * TODO: add documentation
 */
struct ws4d_subscription *ws4d_subsm_getsubs (struct ws4d_subsmanager *subsm,
                                              const char *id);

/**
 * TODO: add documentation
 */
char *ws4d_subsm_getaddr (struct ws4d_subsmanager *subsm);

/**
 * TODO: add documentation
 */
struct ws4d_subscription *ws4d_subsm_allocsubs (struct ws4d_subsmanager
                                                *subsm);

/**
 * TODO: add documentation
 */
int ws4d_subsm_freesubs (struct ws4d_subsmanager *subsm,
                         struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int ws4d_subsm_regsubs (struct ws4d_subsmanager *subsm,
                        struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int ws4d_subsm_expiresubs (struct ws4d_subsmanager *subsm,
                           struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_register_eventdelivery (struct ws4d_subsmanager *subsm,
                                   const char *Mode,
                                   ws4d_processDelivery_cb fprocessDelivery,
                                   ws4d_cleanDelivery_cb fcleanDelivery);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_register_defeventdelivery (struct ws4d_subsmanager *subsm,
                                      const char *Mode,
                                      ws4d_processDelivery_cb
                                      fprocessDelivery,
                                      ws4d_cleanDelivery_cb fcleanDelivery);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_unregister_eventdelivery (struct ws4d_subsmanager *subsm,
                                     const char *Mode);

/**
 * TODO: add documentation
 */
int ws4d_subsm_unregister_alldelivery (struct ws4d_subsmanager *subsm);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_register_eventfilter (struct ws4d_subsmanager *subsm,
                                 const char *Type,
                                 ws4d_processFilter_cb fprocessFilter,
                                 ws4d_cleanFilter_cb fcleanFilter);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_register_defeventfilter (struct ws4d_subsmanager *subsm,
                                    const char *Type,
                                    ws4d_processFilter_cb fprocessFilter,
                                    ws4d_cleanFilter_cb fcleanFilter);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_unregister_eventfilter (struct ws4d_subsmanager *subsm,
                                   const char *Type);

/**
 * TODO: add documentation
 */
int ws4d_subsm_unregister_allfilter (struct ws4d_subsmanager *subsm);

/**
 * TODO: add documentation
 */
int ws4d_subsm_is_delivery (struct ws4d_subscription *subs, const char *Mode);

/**
 * TODO: add documentation
 */
int ws4d_subsm_is_filter (struct ws4d_subscription *subs, const char *Type);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_clean_delivery (struct ws4d_subsmanager *subsm,
                           struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_clean_filter (struct ws4d_subsmanager *subsm,
                         struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int
ws4d_subsm_check_subs (struct ws4d_subsmanager *subsm,
                       struct ws4d_subscription *subs);

/**
 * TODO: add documentation
 */
int ws4d_subsm_check_allsubs (struct ws4d_subsmanager *subsm);

/** @} */

#endif /* WS4D_SUBSMANAGER_H_ */
