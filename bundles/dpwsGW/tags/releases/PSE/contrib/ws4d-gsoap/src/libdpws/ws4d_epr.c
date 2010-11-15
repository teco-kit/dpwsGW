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
 *
 *  Created on: 14.08.2008
 *      Author: Elmar Zeeb
 */

#include "ws4d_misc.h"
#include "ws4d_epr.h"

int
ws4d_epr_init (struct ws4d_epr *epr)
{
  ws4d_assert (epr, WS4D_ERR);

  epr->Address = NULL;
  epr->PortName = NULL;
  epr->ServiceName = NULL;
  epr->elementof = NULL;
  epr->valid = 0;

  ws4d_qnamelist_init (&epr->PortType);
  WS4D_INIT_LIST (&epr->list);
  WS4D_ALLOCLIST_INIT (&epr->alist);
  WS4D_INIT_LIST (&epr->plugins);
  WS4D_INIT_LIST (&epr->copies);
  WS4D_INIT_LIST (&epr->copyof);
#ifdef WITH_MUTEXES
  ws4d_mutex_init (&epr->lock);
#endif

  return WS4D_OK;
}

struct ws4d_epr *
ws4d_epr_alloc (int count, ws4d_alloc_list * alist)
{
  struct ws4d_epr *result = NULL, *epr = NULL;
  int i;

  if (!alist || count < 1)
    return NULL;

  result = ws4d_malloc_alist (count * sizeof (struct ws4d_epr), alist);

  for (i = 0, epr = result; i < count; i++, epr++)
    {
      ws4d_epr_init (epr);
    }

  return result;
}

#ifdef WITH_MUTEXES
void
ws4d_epr_lock (struct ws4d_epr *epr)
{
  ws4d_assert (epr,);

  ws4d_mutex_lock (&epr->lock);
}

void
ws4d_epr_unlock (struct ws4d_epr *epr)
{
  ws4d_assert (epr,);

  ws4d_mutex_unlock (&epr->lock);
}
#endif

int
ws4d_epr_copy (struct ws4d_epr *dst, struct ws4d_epr *src)
{
  register struct ws4d_epr_plugin *plugin, *next;
  int err = 0;

  ws4d_assert (dst && src && src->Address && (src != dst), WS4D_ERR);

  dst->Address = ws4d_strdup (src->Address, ws4d_epr_get_alist (dst));
  ws4d_qnamelist_copy (&src->PortType, &dst->PortType,
                       ws4d_epr_get_alist (dst));

  if (src->ServiceName)
    {
      dst->ServiceName =
        ws4d_strdup (src->ServiceName, ws4d_epr_get_alist (dst));
    }

  if (src->PortName)
    {
      dst->PortName = ws4d_strdup (src->PortName, ws4d_epr_get_alist (dst));
    }

  /* update existing and append nonexisting plugins */
  ws4d_list_foreach (plugin, next, &src->plugins, struct ws4d_epr_plugin,
                     list)
  {
    struct ws4d_epr_plugin *copy = NULL;

    if (plugin && plugin->id && plugin->cb && plugin->cb->fcopy)
      {
        int is_new = 0;

        copy = ws4d_epr_get_plugin (dst, plugin->id);

        if (!copy)
          {
            is_new = 1;
            copy = ws4d_malloc (sizeof (struct ws4d_epr_plugin));
            if (!copy)
              {
                return WS4D_EOM;
              };

            copy->id = plugin->id;
          }

        err = plugin->cb->fcopy (dst, copy, plugin);
        if ((err == WS4D_OK) && plugin->cb && plugin->cb->fdelete)
          {
            if (is_new)
              {
                ws4d_list_add_tail (&copy->list, &dst->plugins);
              }
          }
        else
          {
            if (is_new)
              {
                ws4d_free (copy);
                continue;
              }
            return err;
          }

      }
    ws4d_list_move_tail (&dst->copyof, &src->copies);
  }

  if (dst->valid != src->valid)
    {
      if (src->valid)
        {
          ws4d_epr_validate (dst);
        }
      else
        {
          ws4d_epr_invalidate (dst);
        }
    }

  return WS4D_OK;
}

int
ws4d_epr_done (struct ws4d_epr *epr)
{
  /* test parameters */
  ws4d_assert (epr, WS4D_ERR);

  /* remove all copy references that refer to this epr */
  if (!ws4d_list_empty (&epr->copies))
    {
      register struct ws4d_epr *copy = NULL, *next = NULL;

      ws4d_list_foreach (copy, next, &epr->copies, struct ws4d_epr, copyof)
      {
        if (!ws4d_list_empty (&epr->copyof))
          {
            ws4d_list_move_tail (&copy->copyof, &epr->copyof);
          }
        else
          {
            ws4d_list_del (&copy->copyof);
          }
      }
    }

  /* remove copyof reference */
  if (!ws4d_list_empty (&epr->copyof))
    ws4d_list_del (&epr->copyof);

  /* delete all plugins */
  ws4d_epr_remove_plugins (epr);

  ws4d_qnamelist_clear (&epr->PortType);

  /* free all memory related to this epr */
  ws4d_alloclist_done (&epr->alist);

  epr->Address = NULL;
  epr->ServiceName = NULL;
  epr->PortName = NULL;

  epr->valid = 0;

  return WS4D_OK;
}

int
ws4d_epr_free (int count, struct ws4d_epr *eprs)
{
  struct ws4d_epr *epr = NULL;
  int i;

  /* test parameters */
  ws4d_assert (eprs && (count > 0), WS4D_ERR);

  /* for all eprs in this array */
  for (i = 0, epr = eprs; i < count; i++, epr++)
    {
      /* reset epr */
      ws4d_epr_done (epr);
    }

  /* free memory of epr-array */
  ws4d_free_alist (eprs);

  return WS4D_OK;
}

int
ws4d_epr_validate (struct ws4d_epr *epr)
{
  if (!epr || (epr->valid == 1))
    return WS4D_ERR;

  epr->valid = 1;
  return WS4D_OK;
}

int
ws4d_epr_invalidate (struct ws4d_epr *epr)
{
  register struct ws4d_epr_plugin *plugin, *nextp;
  register struct ws4d_epr *copy, *nextc;

  if (!epr || (epr->valid == 0))
    return WS4D_ERR;

  epr->valid = 0;

  ws4d_list_foreach (copy, nextc, &epr->copies, struct ws4d_epr, copyof)
  {
    ws4d_epr_invalidate (copy);
  }

  ws4d_list_foreach (plugin, nextp, &epr->plugins,
                     struct ws4d_epr_plugin, list)
  {
    if (plugin)
      {
        if (plugin->cb && plugin->cb->finvalidate)
          {
            plugin->cb->finvalidate (epr, plugin);
          }
      }
  }

  return WS4D_OK;
}

int
ws4d_epr_set_Addrs (struct ws4d_epr *epr, const char *Addrs)
{
  ws4d_assert (epr && Addrs, WS4D_ERR);

  epr->Address = ws4d_strdup ((char *) Addrs, ws4d_epr_get_alist (epr));

  return WS4D_OK;
}

const char *
ws4d_epr_get_Addrs (struct ws4d_epr *epr)
{
  ws4d_assert (epr, NULL);

  return epr->Address;
}

ws4d_qnamelist *
ws4d_epr_get_PortTypes (struct ws4d_epr * epr)
{
  ws4d_assert (epr, NULL);

  return &epr->PortType;
}

int
ws4d_epr_set_ServiceName (struct ws4d_epr *epr, const char *ServiceName)
{
  ws4d_assert (epr && ServiceName, WS4D_ERR);

  epr->ServiceName =
    ws4d_strdup ((char *) ServiceName, ws4d_epr_get_alist (epr));

  return WS4D_OK;
}

const char *
ws4d_epr_get_ServiceName (struct ws4d_epr *epr)
{
  ws4d_assert (epr, NULL);

  return epr->ServiceName;
}

int
ws4d_epr_set_PortName (struct ws4d_epr *epr, const char *PortName)
{
  ws4d_assert (epr && PortName, WS4D_ERR);

  epr->PortName = ws4d_strdup ((char *) PortName, ws4d_epr_get_alist (epr));

  return WS4D_OK;
}

const char *
ws4d_epr_get_PortName (struct ws4d_epr *epr)
{
  ws4d_assert (epr, NULL);

  return epr->PortName;
}

int
ws4d_epr_register_plugin (struct ws4d_epr *epr,
                          int (*fcreate) (struct ws4d_epr *
                                          epr,
                                          struct ws4d_epr_plugin *,
                                          void *), void *arg)
{
  int result;
  struct ws4d_epr_plugin *plugin;

  /* test parameters */
  ws4d_assert (epr && fcreate, WS4D_ERR);

  plugin = ws4d_malloc (sizeof (struct ws4d_epr_plugin));
  if (!plugin)
    {
      return WS4D_EOM;
    }

  result = fcreate (epr, plugin, arg);
  if ((result == WS4D_OK) && plugin->cb && plugin->cb->fdelete)
    {
      ws4d_list_add_tail (&plugin->list, &epr->plugins);
      return WS4D_OK;
    }
  else
    {
      ws4d_free (plugin);
      return result;
    }
}

struct ws4d_epr_plugin *
ws4d_epr_get_plugin (struct ws4d_epr *epr, const char *id)
{
  register struct ws4d_epr_plugin *entry, *next;

  /* test parameters */
  ws4d_assert (epr && id, NULL);

  ws4d_list_foreach (entry, next, &epr->plugins, struct ws4d_epr_plugin, list)
  {
    if (entry->id == id)
      {
        return entry;
      }
  }

  ws4d_list_foreach (entry, next, &epr->plugins, struct ws4d_epr_plugin, list)
  {
    if (!strcmp (entry->id, id))
      {
        return entry;
      }
  }

  return NULL;
}

int
ws4d_epr_remove_plugin (struct ws4d_epr *epr, const char *id)
{
  struct ws4d_epr_plugin *plugin = NULL;

  /* test parameters */
  ws4d_assert (epr && id, WS4D_ERR);

  plugin = ws4d_epr_get_plugin (epr, id);
  if (plugin)
    {
      if (plugin->cb && plugin->cb->fdelete)
        {
          plugin->cb->fdelete (epr, plugin);
        }

      ws4d_list_del (&plugin->list);
      ws4d_free (plugin);

      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

void
ws4d_epr_remove_plugins (struct ws4d_epr *epr)
{
  register struct ws4d_epr_plugin *plugin = NULL, *next = NULL;

  /* test parameters */
  ws4d_assert (epr,);

  ws4d_list_foreach (plugin, next, &epr->plugins, struct ws4d_epr_plugin,
                     list)
  {
    if (plugin)
      {
        if (plugin->cb && plugin->cb->fdelete)
          {
            plugin->cb->fdelete (epr, plugin);
          }

        ws4d_list_del (&plugin->list);
        ws4d_free (plugin);
      }
  }
}

void *
ws4d_epr_get_plugindata (struct ws4d_epr *epr, const char *id)
{
  struct ws4d_epr_plugin *entry;

  /* test parameters */
  ws4d_assert (epr && id, NULL);

  entry = ws4d_epr_get_plugin (epr, id);
  if (entry)
    {
      return entry->data;
    }
  else
    {
      return NULL;
    }
}
