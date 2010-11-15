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
#include "ws4d_service.h"

/**
 * Service Cache Element
 */

#define WS4D_SERVICEEP_ID "WS4D-ServiceEp-0.1"

const char *ws4d_serviceep_id = WS4D_SERVICEEP_ID;

struct ws4d_serviceep_plugin_s
{
  struct ws4d_epr *epr;
  ws4d_qnamelist types;
  char *id;
};

INLINE struct ws4d_serviceep_plugin_s *ws4d_serviceep_plugindata (struct
                                                                  ws4d_epr
                                                                  *epr);

int ws4d_serviceep_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                          struct ws4d_epr_plugin *src);

void ws4d_serviceep_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p);

int ws4d_serviceep_fcreate (struct ws4d_epr *epr,
                            struct ws4d_epr_plugin *plugin, void *arg);

int ws4d_serviceep_plugin (struct ws4d_epr *epr);

static struct ws4d_epr_plugin_cb ws4d_serviceep_callbacks =
  { ws4d_serviceep_fdelete, ws4d_serviceep_fcopy, NULL };

INLINE struct ws4d_serviceep_plugin_s *
ws4d_serviceep_plugindata (struct ws4d_epr *epr)
{
  return (struct ws4d_serviceep_plugin_s *)
    ws4d_epr_get_plugindata (epr, ws4d_serviceep_id);
}

int
ws4d_serviceep_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                      struct ws4d_epr_plugin *src)
{
  struct ws4d_serviceep_plugin_s *dst_data = NULL, *src_data = NULL;
  int res = 0;

  src_data = (struct ws4d_serviceep_plugin_s *) src->data;

  /* test parameters */
  ws4d_assert (src_data, WS4D_ERR);

  dst->id = ws4d_serviceep_id;
  dst->cb = &ws4d_serviceep_callbacks;

  if (dst->data)
    {
      dst_data = dst->data;
    }
  else
    {
      dst_data = ws4d_malloc (sizeof (struct ws4d_serviceep_plugin_s));
      if (!dst_data)
        {
          return WS4D_EOM;
        }

      dst->data = dst_data;

      ws4d_qnamelist_init (&dst_data->types);
    }

  dst_data->epr = epr;

  ws4d_qnamelist_clear (&dst_data->types);
  res = ws4d_qnamelist_copy (&src_data->types, &dst_data->types,
                             ws4d_epr_get_alist (epr));
  if (res != WS4D_OK)
    {
      return res;
    }

  if (src_data->id)
    {
      if (dst_data->id)
        {
          /* TODO: should free memory here */
        }

      dst_data->id = (char *) ws4d_strdup (src_data->id,
                                           ws4d_epr_get_alist (epr));
      if (!dst_data->id)
        {
          return WS4D_EOM;
        }
    }

  return WS4D_OK;
}

void
ws4d_serviceep_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct ws4d_serviceep_plugin_s *plugin = p->data;

  WS4D_UNUSED_PARAM (epr);

  if (plugin)
    {
      ws4d_qnamelist_init (&plugin->types);
      ws4d_free (plugin);
    }
}

int
ws4d_serviceep_fcreate (struct ws4d_epr *epr, struct ws4d_epr_plugin *plugin,
                        void *arg)
{
  struct ws4d_serviceep_plugin_s *data = NULL;

  WS4D_UNUSED_PARAM (arg);

  plugin->id = ws4d_serviceep_id;
  plugin->cb = &ws4d_serviceep_callbacks;

  data = ws4d_malloc (sizeof (struct ws4d_serviceep_plugin_s));
  if (!data)
    {
      return WS4D_ERR;
    }

  data->epr = epr;
  ws4d_qnamelist_init (&data->types);

  plugin->data = data;

  return WS4D_OK;
}

int
ws4d_register_serviceep (struct ws4d_epr *epr)
{
  struct ws4d_serviceep_plugin_s *plugin = ws4d_serviceep_plugindata (epr);

  /* check if plugin is already registered */
  if (plugin)
    return WS4D_OK;

  /* register plugin at epr */
  return ws4d_epr_register_plugin (epr, ws4d_serviceep_fcreate, NULL);
}

int
ws4d_serviceep_addtype (struct ws4d_epr *epr, struct ws4d_qname *type)
{
  struct ws4d_qname *dup;
  struct ws4d_serviceep_plugin_s *plugin = ws4d_serviceep_plugindata (epr);

  /* test parameters */
  ws4d_assert (plugin, WS4D_ERR);

  /* duplicate qname */
  dup = ws4d_qname_dup (type, ws4d_epr_get_alist (epr));
  if (!dup)
    {
      return WS4D_EOM;
    }

  /* add duplicated qname to list */
  return ws4d_qnamelist_add (dup, &plugin->types);
}

int
ws4d_serviceep_addtypestr (struct ws4d_epr *epr, const char *types)
{
  struct ws4d_serviceep_plugin_s *plugin = ws4d_serviceep_plugindata (epr);

  /* test parameters */
  ws4d_assert (plugin, WS4D_ERR);

  /* add types to types qname list */
  return ws4d_qnamelist_addstring ((char *) types, &plugin->types,
                                   ws4d_epr_get_alist (epr));
}

ws4d_qnamelist *
ws4d_serviceep_gettypelist (struct ws4d_epr *epr)
{
  struct ws4d_serviceep_plugin_s *plugin = ws4d_serviceep_plugindata (epr);

  /* test parameters */
  ws4d_assert (plugin, NULL);

  /* return reference to types qname list */
  return &plugin->types;
}

int
ws4d_serviceep_setid (struct ws4d_epr *epr, const char *id)
{
  struct ws4d_serviceep_plugin_s *plugin = ws4d_serviceep_plugindata (epr);

  /* test parameters */
  ws4d_assert (plugin && id && *id, WS4D_ERR);

  /* free old id string */
  if (plugin->id)
    {
      ws4d_free_alist (plugin->id);
    }

  /* duplicate id string */
  plugin->id = (char *) ws4d_strdup (id, ws4d_epr_get_alist (epr));
  ws4d_assert (plugin->id, WS4D_EOM);

  return WS4D_OK;
}

char *
ws4d_serviceep_getid (struct ws4d_epr *epr)
{
  struct ws4d_serviceep_plugin_s *plugin = ws4d_serviceep_plugindata (epr);

  /* test parameters */
  ws4d_assert (plugin, NULL);

  /* return copy of id string */
  return plugin->id;
}

int
ws4d_serviceep_matchesTypes (struct ws4d_epr *service, ws4d_qnamelist * types)
{
  register struct ws4d_qname *qname, *next;
  ws4d_qnamelist *service_types = ws4d_serviceep_gettypelist (service);

  /* test parameters */
  ws4d_assert (service_types && types, WS4D_ERR);

  /* empty list matches */
  if (ws4d_qnamelist_empty (types))
    return WS4D_OK;

  ws4d_list_foreach (qname, next, types, struct ws4d_qname, list)
  {
    if (qname && qname->ns && qname->name)
      {
        register struct ws4d_qname *service_qname, *service_next;
        ws4d_list_foreach (service_qname, service_next, service_types,
                           struct ws4d_qname, list)
        {
          if (service_qname && service_qname->ns && service_qname->name)
            {
              if (!strcmp (service_qname->ns, qname->ns)
                  && !strcmp (service_qname->name, qname->name))
                return 1;
            }
        }
      }
  }

  return 0;
}
