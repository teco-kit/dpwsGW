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

#ifndef WS4D_SERVICECACHE_H_
#define WS4D_SERVICECACHE_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#include "ws4d_mutex.h"
#include "ws4d_epr.h"
#include "ws4d_service.h"

/**
 * Service Cache API
 *
 * @addtogroup APIServiceCache Service Cache
 * @ingroup ingroup WS4D_UTILS
 *
 * @{
 */

/**
 * TODO: add documentation
 */
int ws4d_register_sc (struct ws4d_epr *device);

/**
 * TODO: add documentation
 */
int ws4d_has_sc (struct ws4d_epr *device);

/**
 * TODO: add documentation
 */
int ws4d_sc_check_service (struct ws4d_epr *device, const char *Id);

/**
 * TODO: add documentation
 */
int ws4d_sc_invalidate_service (struct ws4d_epr *device, const char *Id);

/**
 * TODO: add documentation
 */
int ws4d_sc_set_Types (struct ws4d_epr *device, char *Id, const char *Types);

/**
 * TODO: add documentation
 */
int ws4d_sc_set_Types2 (struct ws4d_epr *device, char *Id,
                        const ws4d_qnamelist * Types);

/**
 * TODO: add documentation
 */
int ws4d_sc_set_Addrs (struct ws4d_epr *device, char *Id, const char *Addrs);

/**
 * TODO: add documentation
 */
int ws4d_sc_set_WSDL (struct ws4d_epr *device, char *Id, const char *WSDL);

/**
 * TODO: add documentation
 */
int ws4d_sc_get_servicesByType (struct ws4d_epr *device, const char *Types,
                                struct ws4d_abs_eprlist *result);

/**
 * TODO: add documentation
 */
int ws4d_sc_get_servicesByType2 (struct ws4d_epr *device,
                                 const ws4d_qnamelist * Types,
                                 struct ws4d_abs_eprlist *result);

/**
 * TODO: add documentation
 */
int ws4d_sc_get_servicebyId (struct ws4d_epr *device,
                             const char *Id, struct ws4d_epr *result);

#ifdef WITH_MUTEXES
void ws4d_servicecache_lock (struct ws4d_epr *device);
void ws4d_servicecache_unlock (struct ws4d_epr *device);
#else
/**
 * TODO: add documentation
 */
#define ws4d_servicecache_lock(device)

/**
 * TODO: add documentation
 */
#define ws4d_servicecache_unlock(device)
#endif

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*WS4D_SERVICECACHE_H_ */
