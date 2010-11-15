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

#include "ws4d_misc.h"
#include "ws4d_service.h"
#include "ws4d_hostingservice.h"

/*******
 * Hosted Service Logic
 *
 */

#define WS4D_HOSTEDEP_ID "WS4D-HostedEP-0.1"

const char *ws4d_hostedep_plugin_id = WS4D_HOSTEDEP_ID;

#define WS4D_HOSTEDEP_INACTIVE (0)
#define WS4D_HOSTEDEP_ACTIVE (1)

struct ws4d_hostedep_plugin_s
{
  int status;
  const char *uri;
  const char *wsdl;
  char *__any;
  void *transport_data;
  ws4d_alloc_list alist;
};


INLINE struct ws4d_hostedep_plugin_s
  *ws4d_hostedep_plugindata (struct ws4d_epr *epr);

int ws4d_hostedep_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                         struct ws4d_epr_plugin *src);

void ws4d_hostedep_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p);

int ws4d_hostedep_fcreate (struct ws4d_epr *epr,
                           struct ws4d_epr_plugin *plugin, void *arg);

int ws4d_hostedep_plugin (struct ws4d_epr *epr);

static struct ws4d_epr_plugin_cb ws4d_hostedep_callbacks =
  { ws4d_hostedep_fdelete, ws4d_hostedep_fcopy, NULL };

INLINE struct ws4d_hostedep_plugin_s *
ws4d_hostedep_plugindata (struct ws4d_epr *epr)
{

  return (struct ws4d_hostedep_plugin_s *)
    ws4d_epr_get_plugindata (epr, ws4d_hostedep_plugin_id);

}

int
ws4d_hostedep_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                     struct ws4d_epr_plugin *src)
{
  struct ws4d_hostedep_plugin_s *dst_data = NULL, *src_data = NULL;

  src_data = (struct ws4d_hostedep_plugin_s *) src->data;

  /* test parameters */
  ws4d_assert (src_data, WS4D_ERR);

  dst->id = ws4d_hostedep_plugin_id;
  dst->cb = &ws4d_hostedep_callbacks;

  if (dst->data)
    {
      dst_data = dst->data;
    }
  else
    {
      dst_data = ws4d_malloc (sizeof (struct ws4d_hostedep_plugin_s));
      if (!dst_data)
        {
          return WS4D_EOM;
        }

      dst_data->status = WS4D_HOSTEDEP_INACTIVE;

      WS4D_ALLOCLIST_INIT (&dst_data->alist);
    }

  dst_data->status = src_data->status;

  if (dst_data->uri)
    {
      ws4d_free_alist ((void *) dst_data->uri);
    }

  if (src_data->uri)
    {
      dst_data->uri = ws4d_strdup (src_data->uri, ws4d_epr_get_alist (epr));
    }

  if (dst_data->wsdl)
    {
      ws4d_free_alist ((void *) dst_data->wsdl);
    }

  if (src_data->wsdl)
    {
      dst_data->wsdl = ws4d_strdup (src_data->wsdl, ws4d_epr_get_alist (epr));
    }

  if (dst_data->__any)
    {
      ws4d_free_alist ((void *) dst_data->__any);
    }

  if (src_data->__any)
    {
      dst_data->__any =
        ws4d_strdup (src_data->__any, ws4d_epr_get_alist (epr));
    }

  dst_data->transport_data = src_data->transport_data;

  dst->data = dst_data;

  return WS4D_OK;
}

void
ws4d_hostedep_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct ws4d_hostedep_plugin_s *plugin = p->data;

  WS4D_UNUSED_PARAM (epr);

  if (plugin)
    {
      ws4d_alloclist_done (&plugin->alist);
      ws4d_free (plugin);
    }
}

int
ws4d_hostedep_fcreate (struct ws4d_epr *epr,
                       struct ws4d_epr_plugin *plugin, void *arg)
{
  struct ws4d_hostedep_plugin_s *data = NULL;

  WS4D_UNUSED_PARAM (epr);
  WS4D_UNUSED_PARAM (arg);

  plugin->id = ws4d_hostedep_plugin_id;
  plugin->cb = &ws4d_hostedep_callbacks;

  data = ws4d_malloc (sizeof (struct ws4d_hostedep_plugin_s));
  if (!data)
    {
      return WS4D_EOM;
    }

  data->status = WS4D_HOSTEDEP_INACTIVE;

  WS4D_ALLOCLIST_INIT (&data->alist);

  plugin->data = data;

  return WS4D_OK;
}

int
ws4d_hostedep_plugin (struct ws4d_epr *epr)
{
  struct ws4d_hostedep_plugin_s *plugin = ws4d_hostedep_plugindata (epr);

  /* check if plugin is already registered */
  if (plugin)
    {
      return WS4D_OK;
    }

  return ws4d_epr_register_plugin (epr, ws4d_hostedep_fcreate, NULL);
}

int
ws4d_hosted_activate (struct ws4d_epr *hostedservice)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, WS4D_ERR);

  if (plugin->status == WS4D_HOSTEDEP_INACTIVE)
    {
      plugin->status = WS4D_HOSTEDEP_ACTIVE;
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_hosted_deactivate (struct ws4d_epr *hostedservice)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, WS4D_ERR);

  if (plugin->status == WS4D_HOSTEDEP_ACTIVE)
    {
      plugin->status = WS4D_HOSTEDEP_INACTIVE;
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_hosted_isactive (struct ws4d_epr *hostedservice)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, 0);

  return plugin->status == WS4D_HOSTEDEP_ACTIVE;
}

int
ws4d_hosted_set_uri (struct ws4d_epr *hostedservice, const char *uri)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, WS4D_ERR);

  if (plugin->uri)
    {
      ws4d_free_alist ((void *) plugin->uri);
    }

  plugin->uri = ws4d_strdup (uri, ws4d_epr_get_alist (hostedservice));
  if (!plugin->uri)
    {
      return WS4D_EOM;
    }

  return WS4D_OK;
}

const char *
ws4d_hosted_get_uri (struct ws4d_epr *hostedservice)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, NULL);

  return plugin->uri;
}

int
ws4d_hosted_set_wsdl (struct ws4d_epr *hostedservice, const char *wsdl)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, WS4D_ERR);

  if (plugin->wsdl)
    {
      ws4d_free_alist ((void *) plugin->wsdl);
    }

  plugin->wsdl = ws4d_strdup (wsdl, ws4d_epr_get_alist (hostedservice));
  if (!plugin->wsdl)
    {
      return WS4D_EOM;
    }

  return WS4D_OK;
}

const char *
ws4d_hosted_get_wsdl (struct ws4d_epr *hostedservice)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, NULL);

  return plugin->wsdl;
}

int
ws4d_hosted_set_ext (struct ws4d_epr *hostedservice, const char *ext)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  /* test parameters */
  ws4d_assert (plugin && ext, WS4D_ERR);

  /* free old ext string */
  if (plugin->__any)
    {
      ws4d_free_alist ((void *) plugin->__any);
    }

  /* duplicate ext string */
  plugin->__any = ws4d_strdup (ext, ws4d_epr_get_alist (hostedservice));
  ws4d_assert (plugin->__any, WS4D_EOM);

  return WS4D_OK;
}

const char *
ws4d_hosted_get_ext (struct ws4d_epr *hostedservice)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  /* test parameters */
  ws4d_assert (plugin, NULL);

  /* return ext string */
  return plugin->__any;
}

int
ws4d_hosted_set_transportdata (struct ws4d_epr *hostedservice,
                               void *transport_data)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, WS4D_ERR);

  plugin->transport_data = transport_data;

  return WS4D_OK;
}

void *
ws4d_hosted_get_transportdata (struct ws4d_epr *hostedservice)
{
  struct ws4d_hostedep_plugin_s *plugin =
    ws4d_hostedep_plugindata (hostedservice);

  ws4d_assert (plugin, NULL);

  return plugin->transport_data;
}

/*
 * Hosting Service
 */

#include "ws4d_hostingservice.h"
#include "ws4d_abstract_eprlist.h"
#include "ws4d_eprllist.h"

#define WS4D_HOSTING_UNCHANGED 0
#define WS4D_HOSTING_CHANGED 1

int
ws4d_hosting_init (struct ws4d_hostingservice *hs, const char *host)
{
  ws4d_assert (hs && host, WS4D_ERR);

  memset (hs, 0, sizeof (struct ws4d_hostingservice));
  WS4D_ALLOCLIST_INIT (&hs->alist);

  hs->device_changed = WS4D_HOSTING_UNCHANGED;
  hs->model_changed = WS4D_HOSTING_UNCHANGED;
  hs->relation_changed = WS4D_HOSTING_UNCHANGED;
  hs->host = ws4d_strdup (host, &hs->alist);
  if (!hs->host)
    {
      return WS4D_EOM;
    }

  return ws4d_eprlist_init (&hs->services, ws4d_eprllist_init, NULL);
}

int
ws4d_hosting_done (struct ws4d_hostingservice *hs)
{
  int ret;

  ws4d_assert (hs, WS4D_ERR);

  ret = ws4d_eprlist_clear (&hs->services);
  if (ret != WS4D_OK)
    {
      return ret;
    }

  ws4d_eprlist_done (&hs->services);
  ws4d_alloclist_done (&hs->alist);

  return WS4D_OK;
}

int
ws4d_hosting_change_host (struct ws4d_hostingservice *hs, const char *host)
{
  const char *new_host;

  ws4d_assert (hs && host, WS4D_ERR);

  new_host = ws4d_strdup (host, &hs->alist);
  if (!new_host)
    {
      return WS4D_EOM;
    }

  if (hs->host)
    {
      ws4d_free_alist ((void *) hs->host);
    }
  hs->host = new_host;

  /* TODO: should trigger some action here */

  hs->relation_changed = WS4D_HOSTING_CHANGED;

  return WS4D_OK;
}

struct ws4d_epr *
ws4d_hosting_add_service (struct ws4d_hostingservice *hs,
                          const char *serviceid)
{
  int err;
  struct ws4d_epr *service = NULL;

  /* test parameters */
  ws4d_assert (hs && serviceid, NULL);

  /* avoid duplicates */
  service = ws4d_hosting_get_service (hs, serviceid);
  if (service)
    {
      return NULL;
    }

  service = ws4d_eprlist_alloc (&hs->services);
  if (!service)
    {
      return NULL;
    }

  err = ws4d_register_serviceep (service);
  if (err != WS4D_OK)
    {
      ws4d_eprlist_free (&hs->services, service);
      return NULL;
    }

  err = ws4d_hostedep_plugin (service);
  if (err != WS4D_OK)
    {
      ws4d_eprlist_free (&hs->services, service);
      return NULL;
    }

  err = ws4d_serviceep_setid (service, serviceid);
  if (err != WS4D_OK)
    {
      ws4d_eprlist_free (&hs->services, service);
      return NULL;
    }

  err = ws4d_eprlist_add (&hs->services, service);
  if (err != WS4D_OK)
    {
      ws4d_eprlist_free (&hs->services, service);
      return NULL;
    }

  return service;
}

int
ws4d_hosting_bind_service (struct ws4d_hostingservice *hs,
                           struct ws4d_epr *service, char *uri, size_t size)
{
  ws4d_assert (hs && service && uri && (size > 0), WS4D_ERR);

  if ((ws4d_serviceep_getid (service) != NULL)
      && (ws4d_hosted_get_uri (service) == NULL))
    {
      struct ws4d_uri uri_s;

      ws4d_uri_init (&uri_s);
      ws4d_parse_uri (uri, &uri_s, WITH_ALL);

      if (!strncmp (uri_s.host, "host", 4))
        {
          uri_s.host = (char *) hs->host;
        }
      else
        {
          if (strncmp (uri_s.host, hs->host, 50))
            {
              return WS4D_ERR;
            }
        }

      ws4d_hosted_set_uri (service, uri);

      ws4d_uri_tostr (&uri_s, uri, size);
      ws4d_epr_set_Addrs (service, uri);

      ws4d_uri_done (&uri_s);
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_hosting_activate_service (struct ws4d_hostingservice *hs,
                               struct ws4d_epr *service, char *uri,
                               size_t size)
{
  int ret;

  ws4d_assert (hs && service, WS4D_ERR);

  ws4d_hosting_bind_service (hs, service, uri, size);

  if (ws4d_hosted_isactive (service))
    {
      return WS4D_ERR;
    }

  ret = ws4d_hosted_activate (service);
  if (ret != WS4D_OK)
    {
      return ret;
    }

  hs->relation_changed = WS4D_HOSTING_CHANGED;

  return WS4D_OK;
}

int
ws4d_hosting_deactivate_service (struct ws4d_hostingservice *hs,
                                 struct ws4d_epr *service)
{
  int ret;

  ws4d_assert (hs && service, WS4D_ERR);

  if (!ws4d_hosted_isactive (service))
    {
      return WS4D_ERR;
    }

  ret = ws4d_hosted_deactivate (service);
  if (ret != WS4D_OK)
    {
      return ret;
    }

  hs->relation_changed = WS4D_HOSTING_CHANGED;

  return WS4D_OK;
}

int
ws4d_hosting_remove_service (struct ws4d_hostingservice *hs,
                             struct ws4d_epr *service)
{
  int ret;

  ws4d_assert (hs && service, WS4D_ERR);

  if (!ws4d_hosted_isactive (service))
    {
      ws4d_hosting_deactivate_service (hs, service);
    }

  ret = ws4d_eprlist_remove (&hs->services, service);
  if (ret != WS4D_OK)
    {
      return ret;
    }

  return ws4d_eprlist_free (&hs->services, service);
}

struct ws4d_epr *
ws4d_hosting_get_service (struct ws4d_hostingservice *hs,
                          const char *serviceid)
{
  register struct ws4d_epr *epr, *iter;

  ws4d_assert (hs && serviceid, NULL);

  ws4d_eprlist_foreach (epr, iter, &hs->services)
  {
    const char *id = ws4d_serviceep_getid (epr);

    if (id && !strcmp (serviceid, id))
      {
        return epr;
      }
  }

  return NULL;
}

int
ws4d_hosting_get_activeservices (struct ws4d_hostingservice *hs,
                                 struct ws4d_abs_eprlist *list)
{
  register struct ws4d_epr *service, *iter;
  int matches = 0;

  ws4d_assert (hs && list, matches);

  ws4d_eprlist_foreach (service, iter, &hs->services)
  {
    if (ws4d_hosted_isactive (service))
      {
        struct ws4d_epr *copy = ws4d_eprlist_alloc (list);

        if (ws4d_epr_copy (copy, service) != WS4D_OK)
          {
            ws4d_eprlist_free (list, copy);
            break;
          }

        if (ws4d_eprlist_add (list, copy) != WS4D_OK)
          {
            ws4d_eprlist_free (list, copy);
            break;
          }

        matches++;
      }
  }

  return matches;
}

int
ws4d_hosting_get_active_services_count (struct ws4d_hostingservice *hs)
{
  register struct ws4d_epr *epr, *iter;
  register int count = 0;

  ws4d_assert (hs, WS4D_ERR);

  ws4d_eprlist_foreach (epr, iter, &hs->services)
  {
    if (ws4d_hosted_isactive (epr))
      {
        count++;
      }
  }

  return count;
}

struct ws4d_relationship *
ws4d_hosting_get_relationship (struct ws4d_hostingservice *hs,
                               ws4d_alloc_list * alist)
{
  struct ws4d_relationship *result;

  ws4d_assert (hs && alist, NULL);

  result = ws4d_malloc_alist (sizeof (struct ws4d_relationship), alist);
  if (!result)
    {
      return NULL;
    }
  memset (result, 0, sizeof (struct ws4d_relationship));

  result->hosted_count = ws4d_hosting_get_active_services_count (hs);
  if (result->hosted_count > 0)
    {
      struct ws4d_host *host;
      register struct ws4d_epr *epr, *iter;

      result->hosted =
        ws4d_malloc_alist (sizeof (struct ws4d_host) * result->hosted_count,
                           alist);
      if (!result->hosted)
        {
          ws4d_free_alist (result);
          return NULL;
        }
      memset (result->hosted, 0,
              sizeof (struct ws4d_host) * result->hosted_count);

      host = result->hosted;

      ws4d_eprlist_foreach (epr, iter, &hs->services)
      {
        ws4d_epr_lock (epr);
        if (ws4d_hosted_isactive (epr))
          {

            host->Addr = ws4d_strdup (ws4d_epr_get_Addrs (epr), alist);
            host->ServiceId = ws4d_strdup (ws4d_serviceep_getid (epr), alist);
            ws4d_qnamelist_init (&host->types);
            ws4d_qnamelist_copy (ws4d_serviceep_gettypelist (epr),
                                 &host->types, alist);
            host->__any = (char *) ws4d_hosted_get_ext (epr);
            host++;
          }
        ws4d_epr_unlock (epr);
      }
    }

  return result;
}

void
ws4d_hosting_free_relationship (struct ws4d_hostingservice *hs,
                                struct ws4d_relationship *relationship)
{
  int i;

  ws4d_assert (hs && relationship,);

  for (i = 0; i < relationship->hosted_count; i++)
    {
      ws4d_free_alist (relationship->hosted[i].Addr);
      ws4d_free_alist (relationship->hosted[i].ServiceId);
      ws4d_free_alist (relationship->hosted[i].__any);
      ws4d_qnamelist_clear (&relationship->hosted[i].types);
    }
  ws4d_free_alist (relationship->hosted);
  ws4d_free_alist (relationship);
}

int
ws4d_hosted_add_type (struct ws4d_epr *hostedservice, struct ws4d_qname *type)
{
  return ws4d_serviceep_addtype (hostedservice, type);
}

int
ws4d_hosted_add_typestring (struct ws4d_epr *hostedservice, const char *types)
{
  return ws4d_serviceep_addtypestr (hostedservice, types);
}

ws4d_qnamelist *
ws4d_hosted_get_typelist (struct ws4d_epr * hostedservice)
{
  return ws4d_serviceep_gettypelist (hostedservice);
}

struct ws4d_thisDevice *
ws4d_hosting_get_thisdevice (struct ws4d_hostingservice *hs)
{
  ws4d_assert (hs, NULL);

  return &hs->device;
}

struct ws4d_thisDevice *
ws4d_hosting_change_thisdevice (struct ws4d_hostingservice *hs)
{
  ws4d_assert (hs, NULL);

  hs->device_changed = WS4D_HOSTING_CHANGED;
  return &hs->device;
}

struct ws4d_thisModel *
ws4d_hosting_get_thismodel (struct ws4d_hostingservice *hs)
{
  ws4d_assert (hs, NULL);

  return &hs->model;
}

struct ws4d_thisModel *
ws4d_hosting_change_thismodel (struct ws4d_hostingservice *hs)
{
  ws4d_assert (hs, NULL);

  hs->model_changed = WS4D_HOSTING_CHANGED;
  return &hs->model;
}
