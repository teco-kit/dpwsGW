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
#include "ws4d_epr.h"
#include "ws4d_subscription.h"
#include "ws4d_subsmanproxy.h"

/*****************************************************************************
 * Plugin logic
 ****************************************************************************/

/* epr plugin id */
#define WS4D_SUBSPROXYPLUGIN_ID "WS4D-EPR-SubsManProxyPlugin-0.1"
const char *ws4d_subsproxy_plugin_id = WS4D_SUBSPROXYPLUGIN_ID;

/* epr plugin data */
struct ws4d_subsproxy_plugin_s
{
  struct ws4d_list_node subscriptions;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (mutex);
#endif
};
/* forward declarations */
INLINE struct ws4d_subsproxy_plugin_s *ws4d_subsproxy_plugindata (struct
                                                                  ws4d_epr
                                                                  *epr);

int ws4d_subsproxy_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                          struct ws4d_epr_plugin *src);

void ws4d_subsproxy_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p);

int ws4d_subsproxy_fcreate (struct ws4d_epr *epr,
                            struct ws4d_epr_plugin *plugin, void *arg);

static struct ws4d_epr_plugin_cb ws4d_subsproxy_callbacks =
  { ws4d_subsproxy_fdelete, ws4d_subsproxy_fcopy, NULL };

/* get epr kpv plugin data from an epr or register plugin if not found */
INLINE struct ws4d_subsproxy_plugin_s *
ws4d_subsproxy_plugindata (struct ws4d_epr *epr)
{
  struct ws4d_subsproxy_plugin_s *result =
    ws4d_epr_get_plugindata (epr, ws4d_subsproxy_plugin_id);

  if (!result)
    {
      int err = ws4d_epr_register_plugin (epr, ws4d_subsproxy_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return NULL;
        }
      result = ws4d_epr_get_plugindata (epr, ws4d_subsproxy_plugin_id);
    }

  return result;
}

/* copy callback for plugin copy/update functionality */
int
ws4d_subsproxy_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                      struct ws4d_epr_plugin *src)
{
  struct ws4d_subsproxy_plugin_s *dst_data = NULL, *src_data =
    (struct ws4d_subsproxy_plugin_s *) src->data;

  /* test parameters */
  ws4d_assert (src_data, WS4D_ERR);

  /* set plugin id and callbacks */
  dst->id = ws4d_subsproxy_plugin_id;
  dst->cb = &ws4d_subsproxy_callbacks;

  /* allocate plugin data structure if it is not already available */
  if (dst->data)
    {
      dst_data = dst->data;
    }
  else
    {
      dst_data = ws4d_malloc (sizeof (struct ws4d_subsproxy_plugin_s));
      if (!dst_data)
        {
          return WS4D_EOM;
        }

      /* initialize plugin data structure */
      WS4D_INIT_LIST (&dst_data->subscriptions);
      ws4d_mutex_init (&dst_data->mutex);
    }

  /* copy plugin data from src epr to dst epr */
  ws4d_mutex_lock (&src_data->mutex);
  ws4d_mutex_lock (&dst_data->mutex);
  ws4d_subslist_dup (&src_data->subscriptions, &dst_data->subscriptions,
                     ws4d_epr_get_alist (epr));
  ws4d_mutex_unlock (&dst_data->mutex);
  ws4d_mutex_unlock (&src_data->mutex);

  /* set plugin data structure as destination plugin data */
  dst->data = dst_data;

  return WS4D_OK;
}

/* delete callback for epr reset and free functionality */
void
ws4d_subsproxy_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct ws4d_subsproxy_plugin_s *plugin = p->data;

  WS4D_UNUSED_PARAM (epr);

  /* if there is plugin data to free */
  if (plugin)
    {
      /* free plugin data structure elements */
      ws4d_mutex_destroy (&plugin->mutex);
      ws4d_subslist_clear (&plugin->subscriptions);
      /* free plugin data structure */
      ws4d_free (plugin);
    }
}

/* callback for ws4d_epr_register_plugin() function to create plugin */
int
ws4d_subsproxy_fcreate (struct ws4d_epr *epr, struct ws4d_epr_plugin *plugin,
                        void *arg)
{
  struct ws4d_subsproxy_plugin_s *data = NULL;

  WS4D_UNUSED_PARAM (epr);
  WS4D_UNUSED_PARAM (arg);

  /* set plugin id and callbacks */
  plugin->id = ws4d_subsproxy_plugin_id;
  plugin->cb = &ws4d_subsproxy_callbacks;

  /* allocate plugin data structure */
  data = ws4d_malloc (sizeof (struct ws4d_subsproxy_plugin_s));
  if (!data)
    {
      return WS4D_EOM;
    }

  /* initialize plugin data structure */
  WS4D_INIT_LIST (&data->subscriptions);
  ws4d_mutex_init (&data->mutex);

  /* set plugin data structure as plugin data */
  plugin->data = data;

  return WS4D_OK;
}

/*****************************************************************************
 * API
 ****************************************************************************/

struct ws4d_subscription *
ws4d_subsproxy_getsubs (struct ws4d_epr *epr, const char *id)
{
  struct ws4d_subsproxy_plugin_s *plugin = ws4d_subsproxy_plugindata (epr);
  struct ws4d_subscription *subs;

  ws4d_assert (plugin, NULL);

  subs = ws4d_subslist_getsubs (&plugin->subscriptions, id);

  return subs;
}

const char *
ws4d_subsproxy_getsubsman (struct ws4d_epr *epr, const char *id)
{
  struct ws4d_subsproxy_plugin_s *plugin = ws4d_subsproxy_plugindata (epr);
  struct ws4d_subscription *subs;

  ws4d_assert (plugin, NULL);

  subs = ws4d_subslist_getsubs (&plugin->subscriptions, id);
  if (!subs)
    return NULL;

  return ws4d_subs_get_subsm (subs);
}

ws4d_time
ws4d_subsproxy_getexpire (struct ws4d_epr * epr, const char *id)
{
  struct ws4d_subsproxy_plugin_s *plugin = ws4d_subsproxy_plugindata (epr);
  struct ws4d_subscription *subs;

  ws4d_assert (plugin, WS4D_ERR);

  subs = ws4d_subslist_getsubs (&plugin->subscriptions, id);
  if (!subs)
    return WS4D_ERR;

  return subs->expiration;
}

struct ws4d_subscription *
ws4d_subsproxy_addsubs (struct ws4d_epr *epr, const char *id)
{
  struct ws4d_subsproxy_plugin_s *plugin = ws4d_subsproxy_plugindata (epr);

  ws4d_assert (plugin, NULL);

  return ws4d_subslist_initsubs (&plugin->subscriptions, id,
                                 ws4d_epr_get_alist (epr));
}

int
ws4d_subsproxy_delsubs (struct ws4d_epr *epr, const char *id)
{
  struct ws4d_subsproxy_plugin_s *plugin = ws4d_subsproxy_plugindata (epr);

  ws4d_assert (plugin, WS4D_ERR);

  return ws4d_subslist_rmsubs (&plugin->subscriptions, id);
}

struct ws4d_subscription *
ws4d_subsproxy_dupsubs (struct ws4d_epr *epr, const char *id,
                        struct ws4d_subscription *result,
                        ws4d_alloc_list * alist)
{
  int err;
  struct ws4d_subsproxy_plugin_s *plugin = ws4d_subsproxy_plugindata (epr);
  struct ws4d_subscription *subs, *ret = NULL;

  WS4D_UNUSED_PARAM (alist);

  ws4d_assert (plugin, NULL);

  ws4d_mutex_lock (&plugin->mutex);
  subs = ws4d_subslist_getsubs (&plugin->subscriptions, id);
  if (subs)
    {
      err = ws4d_subs_dup (subs, result);
      if (err == WS4D_OK)
        {
          ret = result;
        }
    }
  ws4d_mutex_unlock (&plugin->mutex);

  return ret;
}
