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

#ifndef EPRFLIST_H_
#define EPRFLIST_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

/**
 * Fixed Size Endpoint Reference List
 *
 * @addtogroup APIFixedEndpointList Fixed Size Endpoint Reference List
 * @ingroup APIEndpointListImplementation
 *
 * @{
 */

struct wsa_eprflist_params
{
  int size;
  struct ws4d_epr *(*freplace) (struct ws4d_abs_eprlist * list);
};

int ws4d_eprflist_init (struct ws4d_abs_eprlist *head, void *arg);

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*EPRFLIST_H_ */
