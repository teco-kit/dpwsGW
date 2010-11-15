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

#ifndef WS4D_SUBSMANPROXY_H_
#define WS4D_SUBSMANPROXY_H_

#include "ws4d_misc.h"
#include "ws4d_epr.h"
#include "ws4d_subscription.h"

/**
 * Subscription Manager Proxy
 *
 * @addtogroup APISubscriptionManagerProxy Subscription manager proxy
 * @ingroup ingroup WS4D_UTILS
 *
 * @{
 */

/**
 * TODO: add documentation
 */
struct ws4d_subscription *ws4d_subsproxy_getsubs (struct ws4d_epr *epr,
                                                  const char *id);

/**
 * TODO: add documentation
 */
ws4d_time ws4d_subsproxy_getexpire (struct ws4d_epr *epr, const char *id);

/**
 * TODO: add documentation
 */
const char *ws4d_subsproxy_getsubsman (struct ws4d_epr *epr, const char *id);

/**
 * TODO: add documentation
 */
struct ws4d_subscription *ws4d_subsproxy_addsubs (struct ws4d_epr *epr,
                                                  const char *id);

/**
 * TODO: add documentation
 */
int ws4d_subsproxy_delsubs (struct ws4d_epr *epr, const char *id);

/* TODO: remove parameter alist, will break API */
/**
 * TODO: add documentation
 */
struct ws4d_subscription *ws4d_subsproxy_dupsubs (struct ws4d_epr *epr,
                                                  const char *id,
                                                  struct ws4d_subscription
                                                  *result,
                                                  ws4d_alloc_list * alist);

/** @} */

#endif /* WS4D_SUBSMANPROXY_H_ */
