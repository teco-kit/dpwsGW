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

#ifndef WS4D_HOSTINGSERVICE_H_
#define WS4D_HOSTINGSERVICE_H_

/**
 * Service Hosting
 *
 * @addtogroup APIHosting Service Hosting
 * @ingroup WS4D_UTILS
 *
 * @{
 */

/** @} */

#include "ws4d_abstract_eprlist.h"
#include "ws4d_device_description.h"

/**
 * Hosted Service RelationShip meta data
 *
 * @addtogroup APIHostedRelApi Hosted Service RelationShip meta data
 * @ingroup APIHosting
 *
 * @{
 */

struct ws4d_host
{
  char *Addr;
  char *ServiceId;
  ws4d_qnamelist types;
  char *__any;
};

struct ws4d_relationship
{
  struct ws4d_host host;
  struct ws4d_host *hosted;
  int hosted_count;
};

/** @} */

/**
 * Hosted Service Plugin
 *
 * @addtogroup APIHostedServicePlugin Hosted Service Plugin
 * @ingroup APIHosting
 *
 * @{
 */

/**
 * TODO: add documentation
 */
const char *ws4d_hosted_get_id (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
int ws4d_hosted_add_type (struct ws4d_epr *hostedservice,
                          struct ws4d_qname *type);

/**
 * TODO: add documentation
 */
int ws4d_hosted_add_typestring (struct ws4d_epr *hostedservice,
                                const char *types);

/**
 * TODO: add documentation
 */
ws4d_qnamelist *ws4d_hosted_get_typelist (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
const char *ws4d_hosted_get_typestr (struct ws4d_epr *hostedservice,
                                     ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
int ws4d_hosted_set_uri (struct ws4d_epr *hostedservice, const char *uri);

/**
 * TODO: add documentation
 */
const char *ws4d_hosted_get_uri (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
int ws4d_hosted_set_wsdl (struct ws4d_epr *hostedservice, const char *wsdl);

/**
 * TODO: add documentation
 */
const char *ws4d_hosted_get_wsdl (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
int ws4d_hosted_set_ext (struct ws4d_epr *hostedservice, const char *ext);

/**
 * TODO: add documentation
 */
const char *ws4d_hosted_get_ext (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
int ws4d_hosted_set_transportdata (struct ws4d_epr *hostedservice,
                                   void *transport_data);

/**
 * TODO: add documentation
 */
void *ws4d_hosted_get_transportdata (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
int ws4d_hosted_activate (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
int ws4d_hosted_deactivate (struct ws4d_epr *hostedservice);

/**
 * TODO: add documentation
 */
int ws4d_hosted_isactive (struct ws4d_epr *hostedservice);

/** @} */

/**
 * Hosting Service
 *
 * @addtogroup APIHostingServiceMod Hosting Service
 * @ingroup APIHosting
 *
 * @{
 */

struct ws4d_hostingservice
{
  struct ws4d_abs_eprlist services;
  const char *host;
  int relation_changed;
  struct ws4d_thisDevice device;
  int device_changed;
  struct ws4d_thisModel model;
  int model_changed;
  ws4d_alloc_list alist;
};

/**
 * TODO: add documentation
 */
int ws4d_hosting_init (struct ws4d_hostingservice *hs, const char *host);

/**
 * TODO: add documentation
 */
int ws4d_hosting_done (struct ws4d_hostingservice *hs);

/**
 * TODO: add documentation
 */
int ws4d_hosting_change_host (struct ws4d_hostingservice *hs,
                              const char *host);

/**
 * TODO: add documentation
 */
struct ws4d_epr *ws4d_hosting_add_service (struct ws4d_hostingservice *hs,
                                           const char *serviceid);

/**
 * TODO: add documentation
 */
int ws4d_hosting_bind_service (struct ws4d_hostingservice *hs,
                               struct ws4d_epr *service, char *uri,
                               size_t size);

/**
 * TODO: add documentation
 */
int ws4d_hosting_activate_service (struct ws4d_hostingservice *hs,
                                   struct ws4d_epr *service, char *uri,
                                   size_t size);

/**
 * TODO: add documentation
 */
int ws4d_hosting_deactivate_service (struct ws4d_hostingservice *hs,
                                     struct ws4d_epr *service);

/**
 * TODO: add documentation
 */
int ws4d_hosting_remove_service (struct ws4d_hostingservice *hs,
                                 struct ws4d_epr *service);

/**
 * TODO: add documentation
 */
struct ws4d_epr *ws4d_hosting_get_service (struct ws4d_hostingservice *hs,
                                           const char *serviceid);

/**
 * TODO: add documentation
 */
int ws4d_hosting_get_activeservices (struct ws4d_hostingservice *hs,
                                     struct ws4d_abs_eprlist *list);

/**
 * TODO: add documentation
 */
int ws4d_hosting_get_active_services_count (struct ws4d_hostingservice *hs);

/**
 * TODO: add documentation
 */
struct ws4d_relationship
  *ws4d_hosting_get_relationship (struct ws4d_hostingservice *hs,
                                  ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
void ws4d_hosting_free_relationship (struct ws4d_hostingservice *hs,
                                     struct ws4d_relationship *relationship);

/**
 * TODO: add documentation
 */
struct ws4d_thisDevice
  *ws4d_hosting_get_thisdevice (struct ws4d_hostingservice *hs);

/**
 * TODO: add documentation
 */
struct ws4d_thisDevice
  *ws4d_hosting_change_thisdevice (struct ws4d_hostingservice *hs);

/**
 * TODO: add documentation
 */
struct ws4d_thisModel
  *ws4d_hosting_get_thismodel (struct ws4d_hostingservice *hs);

/**
 * TODO: add documentation
 */
struct ws4d_thisModel
  *ws4d_hosting_change_thismodel (struct ws4d_hostingservice *hs);

/** @} */

#endif /*WS4D_HOSTINGSERVICE_H_ */
