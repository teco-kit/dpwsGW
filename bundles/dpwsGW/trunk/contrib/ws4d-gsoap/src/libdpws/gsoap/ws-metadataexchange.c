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

#include "stdsoap2.h"
#include "soap_misc.h"

#include "wst.nsmap"
#include "ws-addressing.h"
#include "ws-metadataexchange.h"

/**
 * @addtogroup Internals Internals
 *
 * @{
 */

#ifdef MEX_SERVER

struct mex_plugin_data
{
  struct mex_hooks_t *hooks;
  struct ws4d_list_node mex_list;
  ws4d_alloc_list alist;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif

};

#define MEX_PLUGIN_ID "WS-Metadata-PLUGIN-0.1"
const char *mex_plugin_id = MEX_PLUGIN_ID;

INLINE struct mex_plugin_data *mex_get_plugindata (struct soap *soap);

/**
 * Function to get Subscription Manager plugin data from soap handle
 *
 * @return soap soap handle
 */
INLINE struct mex_plugin_data *
mex_get_plugindata (struct soap *soap)
{
  return (struct mex_plugin_data *) soap_lookup_plugin (soap, MEX_PLUGIN_ID);
}

void mex_remove_metadata (struct soap *soap);

static void
mex_plugin_delete (struct soap *soap, struct soap_plugin *p)
{
  struct mex_plugin_data *data = p->data;

  mex_remove_metadata (soap);
  ws4d_alloclist_done (&data->alist);

  /* free plugin data structure */
  SOAP_FREE (soap, p->data);
}

static int
mex_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg)
{
  WS4D_UNUSED_PARAM (arg);

  /* set plugin id */
  p->id = mex_plugin_id;

  /* allocate memory for plugin data */
  p->data = (void *) SOAP_MALLOC (soap, sizeof (struct mex_plugin_data));
  memset (p->data, 0, sizeof (struct mex_plugin_data));

  p->fdelete = mex_plugin_delete;

  /* plugin data initialization */
  if (p->data)
    {
      struct mex_plugin_data *data = p->data;

      WS4D_INIT_LIST (&data->mex_list);

      data->hooks = NULL;

      WS4D_ALLOCLIST_INIT (&data->alist);
#ifdef WITH_MUTEXES
      ws4d_mutex_init (&data->lock);
#endif
    }
  return SOAP_OK;
}

int
mex_register_handle (struct soap *soap, struct mex_hooks_t *hooks)
{
  int err = SOAP_OK;
  struct Namespace *temp_namespaces = NULL;
  struct mex_plugin_data *data = mex_get_plugindata (soap);

  /* test parameters */
  if (!soap)
    {
      return SOAP_ERR;
    }

  /* is plugin already registered? */
  if (data)
    {
      return SOAP_OK;
    }

  err = wsa_register_handle (soap);
  if (err != SOAP_OK)
    return err;

  /* register soap handle as discovery multicast handle */
  err = soap_register_plugin (soap, mex_plugin_init);
  if (err != SOAP_OK)
    return err;

  /* get plugin data */
  data = mex_get_plugindata (soap);
  if (!data)
    return SOAP_ERR;

  temp_namespaces =
    soap_extend_namespaces ((struct Namespace *) soap->namespaces,
                            wst_namespaces, &data->alist);
  if (temp_namespaces)
    {
      soap_set_namespaces (soap, temp_namespaces);
    }

  /* register hooks */
  if (hooks)
    data->hooks = hooks;

  return err;
}

int
mex_register_metadata_section (struct soap *soap, const char *endpoint,
                               struct _wsm__MetadataSection *section)
{
  struct mex_plugin_data *data = mex_get_plugindata (soap);
  struct mex_metadata_section *_section = NULL;

  if (!data || !endpoint || !section)
    return SOAP_ERR;

  _section = ws4d_malloc_alist (sizeof (struct mex_metadata_section),
                                &data->alist);
  if (!_section)
    return SOAP_EOM;
  memset (_section, 0, sizeof (struct mex_metadata_section));

  _section->MetadataSection = section;
  _section->endpoint = ws4d_strdup (endpoint, &data->alist);
  ws4d_list_add_tail (&_section->list, &data->mex_list);

  return SOAP_OK;
}

static struct mex_metadata_section *
_mex_get_metadata_section (struct soap *soap,
                           const char *endpoint, const char *dialect)
{
  struct mex_plugin_data *data = mex_get_plugindata (soap);
  register struct mex_metadata_section *result = NULL, *next;

  if (!data || !endpoint || !dialect)
    return result;

  ws4d_list_foreach (result, next, &data->mex_list,
                     struct mex_metadata_section, list)
  {
    if (result && result->endpoint && result->MetadataSection->Dialect
        && ((endpoint == result->endpoint)
            || !strcmp (endpoint, result->endpoint))
        && ((dialect == result->MetadataSection->Dialect)
            || !strcmp (dialect, result->MetadataSection->Dialect)))
      {
        return result;
      }
  }
  return NULL;
}

void *
mex_get_metadata_section (struct soap *soap, const char *endpoint,
                          const char *dialect)
{
  struct mex_metadata_section *result = NULL;

  result = _mex_get_metadata_section (soap, endpoint, dialect);
  if (result)
    {
      return result->MetadataSection;
    }

  return NULL;
}

static void
mex_free_metadata_section (struct soap *soap,
                           struct mex_metadata_section *section)
{
  if (!soap || !section)
    return;

  if (section->endpoint)
    {
      ws4d_free_alist (section->endpoint);
    }

  ws4d_list_del (&section->list);
  ws4d_free_alist (section);

  return;
}

int
mex_remove_metadata_section (struct soap *soap, const char *endpoint,
                             const char *dialect)
{
  struct mex_plugin_data *data = mex_get_plugindata (soap);
  struct mex_metadata_section *section = NULL;

  if (!data || !endpoint || !dialect)
    return SOAP_ERR;

  section = _mex_get_metadata_section (soap, endpoint, dialect);
  if (!section)
    return SOAP_ERR;

  mex_free_metadata_section (soap, section);

  return SOAP_OK;
}

void
mex_remove_metadata (struct soap *soap)
{
  struct mex_plugin_data *data = mex_get_plugindata (soap);
  register struct mex_metadata_section *entry, *next;

  if (!data)
    return;

  ws4d_list_foreach (entry, next, &data->mex_list,
                     struct mex_metadata_section, list)
  {
    mex_free_metadata_section (soap, entry);
  }
  return;
}

int
mex_process_TransferGet (struct soap *soap, char *endpoint,
                         struct _wsm__Metadata *GetResponseMsg)
{
  struct mex_plugin_data *data = mex_get_plugindata (soap);
  register struct mex_metadata_section *entry, *next;
  int i, count = 0;

  if (!data)
    return SOAP_ERR;

  ws4d_list_foreach (entry, next, &data->mex_list,
                     struct mex_metadata_section, list)
  {
    if (entry && entry->endpoint
        && ((endpoint == entry->endpoint)
            || !strcmp (endpoint, entry->endpoint)))
      {
        count++;
      }
  }

  GetResponseMsg->wsm__MetadataSection = soap_malloc (soap, count
                                                      *
                                                      sizeof (struct
                                                              _wsm__MetadataSection));
  if (!GetResponseMsg->wsm__MetadataSection)
    return SOAP_OK;
  GetResponseMsg->__sizeMetadataSection = count;

  i = 0;
  ws4d_list_foreach (entry, next, &data->mex_list,
                     struct mex_metadata_section, list)
  {
    if (entry && entry->endpoint
        && ((endpoint == entry->endpoint)
            || !strcmp (endpoint, entry->endpoint)))
      {
        memcpy ((void *) (&GetResponseMsg->wsm__MetadataSection[i]),
                entry->MetadataSection,
                sizeof (struct _wsm__MetadataSection));
        i++;
      }
  }

  wsa_header_gen_response (soap, NULL, (char *) wsa_anonymousURI,
                           "http://schemas.xmlsoap.org/ws/2004/09/transfer/GetResponse",
                           wsa_header_get_MessageId (soap),
                           sizeof (struct SOAP_ENV__Header));
  return SOAP_OK;
}
#endif

/** @} */

/**
 * @addtogroup DpwsMetadataClient Client side
 * @ingroup DpwsMetadata
 * @{
 */

#ifdef MEX_CLIENT
/**
 * Function sends a GetMetadata request to the endpoint specified with
 * address and the dialect specified with Dialect. Then reveives the
 * response, internally analyzes it and returns it.
 *
 * @param soap
 * @param Address
 * @param Dialect
 *
 * @return
 */
int
mex_GetMetadata (struct soap *soap, char *MsgId, const char *Address,
                 const char *To, const char *Dialect,
                 struct _wsm__Metadata *metadata, ws4d_time timeout)
{
  WS4D_UNUSED_PARAM (Dialect);

  /* test parameters */
  soap_assert (soap, soap && Address && metadata, SOAP_ERR);

  /* initialize metadata structure */
  soap_default__wsm__Metadata (soap, metadata);

  /* set timeout */
  soap->recv_timeout = -1000 * timeout;

  /* Prepare SOAP Header */
  soap_set_namespaces (soap, wst_namespaces);
  wsa_header_gen_request (soap, MsgId, To,
                          "http://schemas.xmlsoap.org/ws/2004/09/transfer/Get",
                          NULL, NULL, sizeof (struct SOAP_ENV__Header));

  /* Make request-response call */
  return soap_call___wst__Get (soap, Address, NULL, NULL, metadata);
}

/* TODO: implement mex_hooks */
int
mex_soap_init (struct soap *soap, struct mex_hooks_t *mex_hooks)
{
  int err = SOAP_OK;

  WS4D_UNUSED_PARAM (mex_hooks);

  soap_set_namespaces (soap, wst_namespaces);

  err = wsa_register_handle (soap);
  return err;
}
#endif

/** @} */

/** @} */
