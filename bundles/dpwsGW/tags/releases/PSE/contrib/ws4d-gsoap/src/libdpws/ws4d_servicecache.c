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
#include "ws4d_abstract_eprlist.h"
#include "ws4d_eprllist.h"
#include "ws4d_service.h"

#include "ws4d_servicecache.h"

/**
 * Service Cache Plugin
 */

#define WS4D_SERVICECACHE_ID "WS4D-ServiceCache-0.1"

const char *ws4d_servicecache_id = WS4D_SERVICECACHE_ID;

struct ws4d_servicecache_plugin
{
  struct ws4d_abs_eprlist cache;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
};

INLINE struct ws4d_servicecache_plugin *ws4d_servicecache_plugindata (struct
                                                                      ws4d_epr
                                                                      *epr);

static int servicecache_fcopy (struct ws4d_epr *epr,
                               struct ws4d_epr_plugin *dst,
                               struct ws4d_epr_plugin *src);

static void servicecache_fdelete (struct ws4d_epr *epr,
                                  struct ws4d_epr_plugin *p);

static int servicecache_invalidate (struct ws4d_epr *epr,
                                    struct ws4d_epr_plugin *p);

static struct ws4d_epr_plugin_cb ws4d_servicecache_callbacks =
  { servicecache_fdelete, servicecache_fcopy, servicecache_invalidate };

INLINE struct ws4d_servicecache_plugin *
ws4d_servicecache_plugindata (struct ws4d_epr *epr)
{
  return (struct ws4d_servicecache_plugin *)
    ws4d_epr_get_plugindata (epr, ws4d_servicecache_id);
}

static int
servicecache_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                    struct ws4d_epr_plugin *src)
{
  struct ws4d_servicecache_plugin *data = dst->data;
  struct ws4d_servicecache_plugin *src_data = src->data;
  register struct ws4d_epr *src_epr, *iter;

  WS4D_UNUSED_PARAM (epr);

  /* if destination has alread an servicecache plugin */
  if (data)
    {
      ws4d_eprlist_clear (&data->cache);
    }
  /* if destination has no servicecache plugin */
  else
    {
      dst->id = ws4d_servicecache_id;
      dst->cb = &ws4d_servicecache_callbacks;

      data = ws4d_malloc (sizeof (struct ws4d_servicecache_plugin));
      if (!data)
        return WS4D_ERR;

      ws4d_eprlist_init (&data->cache, ws4d_eprllist_init, NULL);
#ifdef WITH_MUTEXES
      ws4d_mutex_init (&data->lock);
#endif

      dst->data = data;
    }

  ws4d_eprlist_foreach (src_epr, iter, &src_data->cache)
  {
    struct ws4d_epr *copy_epr;
    int is_new = 0;

    copy_epr =
      ws4d_eprlist_get_byAddr (&data->cache, ws4d_epr_get_Addrs (src_epr));

    if (!copy_epr)
      {
        copy_epr = ws4d_eprlist_alloc (&data->cache);
        ws4d_assert (copy_epr, WS4D_EOM);

        is_new = 1;
      }

    ws4d_epr_copy (copy_epr, src_epr);

    if (is_new)
      {
        ws4d_eprlist_add (&data->cache, copy_epr);
      }
  }

  return WS4D_OK;
}

static int
servicecache_invalidate (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct ws4d_servicecache_plugin *data = p->data;

  WS4D_UNUSED_PARAM (epr);

  /* also invalidate services in service cache */
  if (data)
    {
      register struct ws4d_epr *service, *iter;

      ws4d_eprlist_foreach (service, iter, &data->cache)
      {
        ws4d_epr_invalidate (service);
      }
    }

  return WS4D_OK;
}

static void
servicecache_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct ws4d_servicecache_plugin *plugin = p->data;

  WS4D_UNUSED_PARAM (epr);

  if (plugin)
    {
      ws4d_eprlist_done (&plugin->cache);
      ws4d_free (plugin);
    }
}

static int
servicecache_fcreate (struct ws4d_epr *epr,
                      struct ws4d_epr_plugin *plugin, void *arg)
{
  struct ws4d_servicecache_plugin *data = NULL;

  WS4D_UNUSED_PARAM (epr);
  WS4D_UNUSED_PARAM (arg);

  plugin->id = ws4d_servicecache_id;
  plugin->cb = &ws4d_servicecache_callbacks;

  data = ws4d_malloc (sizeof (struct ws4d_servicecache_plugin));
  ws4d_assert (data, WS4D_EOM);

  if (ws4d_eprlist_init (&data->cache, ws4d_eprllist_init, NULL) != WS4D_OK)
    {
      ws4d_free (data);
      return WS4D_ERR;
    }
#ifdef WITH_MUTEXES
  ws4d_mutex_init (&data->lock);
#endif

  plugin->data = data;

  return WS4D_OK;
}

/**
 * internal functions
 */

static struct ws4d_epr *servicecache_allocservice (struct ws4d_epr *device);

static int servicecache_addservice (struct ws4d_epr *device,
                                    struct ws4d_epr *service);

static struct ws4d_epr *servicecache_get_servicebyId (struct
                                                      ws4d_servicecache_plugin
                                                      *plugin,
                                                      const char *Id);

static int servicecache_get_services (struct ws4d_servicecache_plugin *plugin,
                                      struct ws4d_abs_eprlist *result);

static int servicecache_get_servicesByType (struct ws4d_servicecache_plugin
                                            *plugin, ws4d_qnamelist * Types,
                                            struct ws4d_abs_eprlist *result);

static struct ws4d_epr *
servicecache_allocservice (struct ws4d_epr *device)
{
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);
  struct ws4d_epr *result = NULL;

  /* test parameters */
  ws4d_assert (plugin, NULL);

  result = ws4d_eprlist_alloc (&plugin->cache);

  return result;
}

static int
servicecache_addservice (struct ws4d_epr *device, struct ws4d_epr *service)
{
  int err = WS4D_ERR;
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  /* test parameters */
  ws4d_assert (plugin, WS4D_ERR);

  /* register service ep plugin */
  ws4d_assert (ws4d_register_serviceep (service) == WS4D_OK, WS4D_ERR);

  err = ws4d_eprlist_add (&plugin->cache, service);

  return err;
}

static struct ws4d_epr *
servicecache_get_servicebyId (struct ws4d_servicecache_plugin *plugin,
                              const char *Id)
{
  register struct ws4d_epr *epr, *iter;

  /* test parameters */
  ws4d_fail (!plugin, NULL);

  ws4d_eprlist_foreach (epr, iter, &plugin->cache)
  {
    register char *service_id = ws4d_serviceep_getid (epr);

    if (service_id && ((service_id == Id) || !strcmp (service_id, Id)))
      {
        return epr;
      }
  }

  return NULL;
}

static int
servicecache_get_servicesByType (struct ws4d_servicecache_plugin *plugin,
                                 ws4d_qnamelist * Types,
                                 struct ws4d_abs_eprlist *result)
{
  register struct ws4d_epr *service, *iter;

  /* test parameters */
  ws4d_fail (!plugin, WS4D_ERR);

  /* if no types are specified, simply copy whole list */
  if (Types)
    {
      if (ws4d_qnamelist_empty (Types))
        {
          return servicecache_get_services (plugin, result);
        }
    }
  else
    {
      return servicecache_get_services (plugin, result);
    }

  ws4d_eprlist_foreach (service, iter, &plugin->cache)
  {
    if (ws4d_epr_isvalid (service)
        && ws4d_serviceep_matchesTypes (service, Types))
      {
        struct ws4d_epr *copy = ws4d_eprlist_alloc (result);
        if (!copy)
          {
            return WS4D_EOM;
          }

        ws4d_epr_copy (copy, service);
        ws4d_eprlist_add (result, copy);

      }
  }

  return WS4D_OK;
}

static int
servicecache_get_services (struct ws4d_servicecache_plugin *plugin,
                           struct ws4d_abs_eprlist *result)
{
  register struct ws4d_epr *service, *iter;

  /* test parameters */
  ws4d_fail (!plugin, WS4D_ERR);

  ws4d_eprlist_foreach (service, iter, &plugin->cache)
  {
    if (ws4d_epr_isvalid (service))
      {
        struct ws4d_epr *copy = ws4d_eprlist_alloc (result);
        if (!copy)
          {
            return WS4D_EOM;
          }

        ws4d_epr_copy (copy, service);
        ws4d_eprlist_add (result, copy);

      }
  }

  return WS4D_OK;
}

/**
 * Public functions
 */

int
ws4d_register_sc (struct ws4d_epr *device)
{
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  ws4d_assert (device, WS4D_ERR);

  /* check if plugin is already registered */
  if (plugin)
    return WS4D_OK;

  /* register plugin at epr */
  return ws4d_epr_register_plugin (device, servicecache_fcreate, NULL);
}

int
ws4d_has_sc (struct ws4d_epr *device)
{
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  return plugin != NULL;
}

int
ws4d_sc_check_service (struct ws4d_epr *device, const char *Id)
{
  struct ws4d_epr *service;
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  ws4d_fail (!plugin || !Id, WS4D_SERVICECACHE_ERR);

  service = servicecache_get_servicebyId (plugin, Id);
  if (!service)
    {
      service = servicecache_allocservice (device);
      servicecache_addservice (device, service);
      ws4d_serviceep_setid (service, Id);
      ws4d_epr_validate (service);

      return WS4D_SERVICECACHE_NEW;
    }
  else
    {
      if (ws4d_epr_isvalid (service))
        {
          return WS4D_SERVICECACHE_VALID;
        }
      else
        {
          return WS4D_SERVICECACHE_INVALID;
        }
    }
}

int
ws4d_sc_set_Types (struct ws4d_epr *device, char *Id, const char *Types)
{
  struct ws4d_epr *service;
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  service = servicecache_get_servicebyId (plugin, Id);
  if (!service)
    {
      return WS4D_SERVICECACHE_NOTFOUND;
    }
  else
    {
      return ws4d_serviceep_addtypestr (service, Types);
    }
}

/* TODO: to be implemented */
int
ws4d_sc_set_Types2 (struct ws4d_epr *device, char *Id,
                    const ws4d_qnamelist * Types)
{
  WS4D_UNUSED_PARAM (device);
  WS4D_UNUSED_PARAM (Id);
  WS4D_UNUSED_PARAM (Types);

  return WS4D_ERR;
}

int
ws4d_sc_set_Addrs (struct ws4d_epr *device, char *Id, const char *Addrs)
{
  struct ws4d_epr *service;
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  ws4d_assert (plugin, WS4D_ERR)
    service = servicecache_get_servicebyId (plugin, Id);
  if (!service)
    {
      return WS4D_SERVICECACHE_NOTFOUND;
    }
  else
    {
      return ws4d_epr_set_Addrs (service, Addrs);
    }
}

/* TODO: to be implemented */
int
ws4d_sc_set_WSDL (struct ws4d_epr *device, char *Id, const char *WSDL)
{
  WS4D_UNUSED_PARAM (device);
  WS4D_UNUSED_PARAM (Id);
  WS4D_UNUSED_PARAM (WSDL);

  return WS4D_ERR;
}

int
ws4d_sc_get_servicebyId (struct ws4d_epr *device, const char *Id,
                         struct ws4d_epr *result)
{
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  return ws4d_epr_copy (result, servicecache_get_servicebyId (plugin, Id));
}

int
ws4d_sc_get_servicesByType (struct ws4d_epr *device, const char *Types,
                            struct ws4d_abs_eprlist *result)
{
  int ret;
  ws4d_qnamelist service_type_list;
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  /* test parameters */
  ws4d_fail (!plugin, WS4D_ERR);

  ws4d_qnamelist_init (&service_type_list);
  ws4d_qnamelist_addstring (Types, &service_type_list,
                            ws4d_epr_get_alist (device));

  ret = servicecache_get_servicesByType (plugin, &service_type_list, result);

  ws4d_qnamelist_done (&service_type_list);

  return ret;
}

int
ws4d_sc_get_servicesByType2 (struct ws4d_epr *device,
                             const ws4d_qnamelist * Types,
                             struct ws4d_abs_eprlist *result)
{
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);

  /* test parameters */
  ws4d_fail (!plugin, WS4D_ERR);

  return servicecache_get_servicesByType (plugin, (ws4d_qnamelist *) Types,
                                          result);
}

#ifdef WITH_MUTEXES
void
ws4d_servicecache_lock (struct ws4d_epr *device)
{
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);
  if (plugin)
    ws4d_mutex_lock (&plugin->lock);
}
#endif

#ifdef WITH_MUTEXES
void
ws4d_servicecache_unlock (struct ws4d_epr *device)
{
  struct ws4d_servicecache_plugin *plugin =
    ws4d_servicecache_plugindata (device);
  if (plugin)
    ws4d_mutex_unlock (&plugin->lock);
}
#endif
