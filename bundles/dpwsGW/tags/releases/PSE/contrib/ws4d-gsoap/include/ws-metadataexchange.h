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

#ifndef WSMETADATAEXCHANGE_H_
#define WSMETADATAEXCHANGE_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif


#include "soap_misc.h"

/**
 * gSoap WS-Transer/Matadataexchange Plugin API
 *
 * @addtogroup MEXAPI Metadata-Exchange-API
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

extern const char *mex_plugin_id;

typedef void (*NewService_cb) (void);

typedef void (*ForgetService_cb) (void);

struct mex_hooks_t
{
  NewService_cb NewService;
  ForgetService_cb ForgetService;
};

int mex_register_handle (struct soap *, struct mex_hooks_t *hooks);

struct mex_metadata_section
{
  struct ws4d_list_node list;
  char *endpoint;
  struct _wsm__MetadataSection *MetadataSection;
};

int
mex_register_metadata_section (struct soap *soap, const char *endpoint,
                               struct _wsm__MetadataSection *section);

void *mex_get_metadata_section (struct soap *soap, const char *endpoint,
                                const char *dialect);

int
mex_remove_metadata_section (struct soap *soap, const char *endpoint,
                             const char *dialect);

int
mex_process_TransferGet (struct soap *soap, char *endpoint,
                         struct _wsm__Metadata *GetResponseMsg);

int mex_GetMetadata (struct soap *soap, char *MsgId, const char *Address,
                     const char *To, const char *Dialect,
                     struct _wsm__Metadata *metadata, ws4d_time timeout);

int mex_soap_init (struct soap *soap, struct mex_hooks_t *mex_hooks);

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*WSMETADATAEXCHANGE_H_ */
