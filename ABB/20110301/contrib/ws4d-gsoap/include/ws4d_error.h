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

#ifndef WS4D_ERROR_H_
#define WS4D_ERROR_H_

/**
 * Errors
 *
 * @addtogroup APIError Errors
 * @ingroup WS4D_UTILS
 *
 * @{
 */

#define WS4D_ERR                       EOF
#define WS4D_OK                          0
#define WS4D_EOM                         1
#define WS4D_TO                          2

#define WS4D_TARGETCACHE_ERR            20
#define WS4D_TARGETCACHE_OK             21
#define WS4D_TARGETCACHE_VALID          22
#define WS4D_TARGETCACHE_INVALID        23
#define WS4D_TARGETCACHE_NEW            24
#define WS4D_TARGETCACHE_UNKNOWN        25

#define WS4D_SERVICECACHE_ERR           30
#define WS4D_SERVICECACHE_VALID         31
#define WS4D_SERVICECACHE_INVALID       32
#define WS4D_SERVICECACHE_NEW           33
#define WS4D_SERVICECACHE_NOTFOUND      34

/** @} */

#endif /*WS4D_ERROR_H_ */
