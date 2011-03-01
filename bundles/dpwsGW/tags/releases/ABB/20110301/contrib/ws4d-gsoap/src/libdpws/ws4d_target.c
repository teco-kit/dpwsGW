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
 *  Created on: 18.08.2008
 *      Author: Elmar Zeeb
 */

#include "ws4d_misc.h"
#include "ws4d_epr.h"
#include "ws4d_target.h"
#include "ws4d_scopelist.h"

#define WS4D_TARGETEP_ID "WS4D-TargetEp-0.1"

const char *ws4d_targetep_id = WS4D_TARGETEP_ID;

struct ws4d_targetep_msgids
{
  const char *ProbeMsgId;
  const char *ResolveMsgId;
};

struct ws4d_targetep_plugin
{
  int state;
  const char *XAddrs;
  ws4d_qnamelist Types;
  const char *TypeStr;
  struct ws4d_list_node Scopes;
  const char *ScopeStr;
  unsigned int MetadataVersion;
  struct ws4d_targetep_msgids *msgids;
  void *targetservice;
};

INLINE
  struct ws4d_targetep_plugin *ws4d_targetep_plugindata (struct ws4d_epr
                                                         *epr);

int ws4d_targetep_fcreate (struct ws4d_epr *epr,
                           struct ws4d_epr_plugin *plugin, void *arg);

int ws4d_targetep_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                         struct ws4d_epr_plugin *src);

void ws4d_targetep_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p);

static struct ws4d_epr_plugin_cb ws4d_targetep_callbacks = {
  ws4d_targetep_fdelete, ws4d_targetep_fcopy, NULL
};

INLINE struct ws4d_targetep_plugin *
ws4d_targetep_plugindata (struct ws4d_epr *epr)
{
  return (struct ws4d_targetep_plugin *) ws4d_epr_get_plugindata (epr,
                                                                  ws4d_targetep_id);
}

int
ws4d_targetep_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                     struct ws4d_epr_plugin *src)
{
  struct ws4d_targetep_plugin *dst_data = NULL, *src_data = NULL;

  dst->cb = &ws4d_targetep_callbacks;

  src_data = src->data;
  if (dst->data)
    {
      dst_data = dst->data;
    }
  else
    {
      dst_data = ws4d_malloc (sizeof (struct ws4d_targetep_plugin));
      if (!dst_data)
        return WS4D_EOM;

      ws4d_qnamelist_init (&dst_data->Types);
      WS4D_INIT_LIST (&dst_data->Scopes);

      dst->data = dst_data;
    }

  dst_data->MetadataVersion = src_data->MetadataVersion;
  if (src_data->msgids && !dst_data->msgids)
    {
      dst_data->msgids =
        ws4d_malloc_alist (sizeof (struct ws4d_targetep_msgids),
                           ws4d_epr_get_alist (epr));
    }

  if (src_data->msgids && src_data->msgids->ProbeMsgId)
    {
      if (dst_data->msgids->ProbeMsgId)
        {
          ws4d_free_alist ((void *) dst_data->msgids->ProbeMsgId);
          dst_data->msgids->ProbeMsgId = NULL;
        }

      dst_data->msgids->ProbeMsgId =
        ws4d_strdup (src_data->msgids->ProbeMsgId, ws4d_epr_get_alist (epr));
      if (!dst_data->msgids->ProbeMsgId)
        {
          return WS4D_EOM;
        }
    }

  if (src_data->msgids && src_data->msgids->ResolveMsgId)
    {
      if (dst_data->msgids->ResolveMsgId)
        {
          ws4d_free_alist ((void *) dst_data->msgids->ResolveMsgId);
          dst_data->msgids->ResolveMsgId = NULL;
        }

      dst_data->msgids->ResolveMsgId =
        ws4d_strdup (src_data->msgids->ResolveMsgId,
                     ws4d_epr_get_alist (epr));
      if (!dst_data->msgids->ResolveMsgId)
        {
          return WS4D_EOM;
        }
    }

  if (!ws4d_qnamelist_empty (&src_data->Types))
    {
      if (!ws4d_qnamelist_empty (&dst_data->Types))
        {
          ws4d_qnamelist_clear (&dst_data->Types);
        }

      ws4d_qnamelist_copy (&src_data->Types, &dst_data->Types,
                           ws4d_epr_get_alist (epr));
    }

  if (dst_data->TypeStr)
    {
      ws4d_free_alist ((void *) dst_data->TypeStr);
      dst_data->TypeStr = NULL;
    }
  if (src_data->TypeStr)
    {
      dst_data->TypeStr =
        ws4d_strdup (src_data->TypeStr, ws4d_epr_get_alist (epr));
    }

  ws4d_sl_dup (&src_data->Scopes, &dst_data->Scopes,
               ws4d_epr_get_alist (epr));

  if (dst_data->ScopeStr)
    {
      ws4d_free_alist ((void *) dst_data->ScopeStr);
      dst_data->ScopeStr = NULL;
    }
  if (src_data->ScopeStr)
    {
      dst_data->ScopeStr = ws4d_strdup (src_data->ScopeStr,
                                        ws4d_epr_get_alist (epr));
    }

  if (src_data->XAddrs)
    {
      if (dst_data->XAddrs)
        {
          ws4d_free_alist ((void *) dst_data->XAddrs);
          dst_data->XAddrs = NULL;
        }

      dst_data->XAddrs =
        ws4d_strdup (src_data->XAddrs, ws4d_epr_get_alist (epr));
      if (!dst_data->XAddrs)
        {
          return WS4D_EOM;
        }
    }

  dst_data->targetservice = src_data->targetservice;

  return WS4D_OK;
}

void
ws4d_targetep_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct ws4d_targetep_plugin *plugin = NULL;

  WS4D_UNUSED_PARAM (epr);

  if (p->data)
    plugin = p->data;

  if (plugin)
    {
      ws4d_free (plugin);
    }
}

int
ws4d_targetep_fcreate (struct ws4d_epr *epr, struct ws4d_epr_plugin *plugin,
                       void *arg)
{
  struct ws4d_targetep_plugin *data = NULL;

  WS4D_UNUSED_PARAM (epr);
  WS4D_UNUSED_PARAM (arg);

  plugin->id = ws4d_targetep_id;
  plugin->cb = &ws4d_targetep_callbacks;

  data = ws4d_malloc (sizeof (struct ws4d_targetep_plugin));
  if (!data)
    return WS4D_ERR;

  data->state = WS4D_TARGET_INACTIVE;
  ws4d_qnamelist_init (&data->Types);
  ws4d_sl_init (&data->Scopes);
  data->MetadataVersion = -1;

  plugin->data = data;

  return WS4D_OK;
}

int
ws4d_register_targetep (struct ws4d_epr *epr)
{
  if (!epr)
    return WS4D_ERR;

  return ws4d_epr_register_plugin (epr, ws4d_targetep_fcreate, NULL);
}

static int
_ws4d_targetep_changed (struct ws4d_targetep_plugin *plugin)
{
  if (plugin->state == WS4D_TARGET_ACTIVE)
    {
      plugin->MetadataVersion++;
    }

  return WS4D_OK;
}

int
ws4d_targetep_set_XAddrs (struct ws4d_epr *epr, const char *XAddrs)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!XAddrs)
    return WS4D_ERR;

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  if (plugin->XAddrs)
    ws4d_free_alist ((void *) plugin->XAddrs);

  plugin->XAddrs = ws4d_strdup (XAddrs, ws4d_epr_get_alist (epr));

  if (plugin->XAddrs)
    {
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

const char *
ws4d_targetep_get_XAddrs (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!plugin)
    return NULL;

  return plugin->XAddrs;
}

int
ws4d_targetep_set_ProbeMsgId (struct ws4d_epr *epr, const char *ProbeMsgId)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!ProbeMsgId)
    return WS4D_ERR;

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  if (!plugin->msgids)
    {
      plugin->msgids =
        ws4d_malloc_alist (sizeof (struct ws4d_targetep_msgids),
                           ws4d_epr_get_alist (epr));
      if (!plugin->msgids)
        return WS4D_EOM;
    }

  if (plugin->msgids->ProbeMsgId)
    ws4d_free_alist ((void *) plugin->msgids->ProbeMsgId);

  plugin->msgids->ProbeMsgId =
    ws4d_strdup (ProbeMsgId, ws4d_epr_get_alist (epr));

  if (plugin->msgids->ProbeMsgId)
    {
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

const char *
ws4d_targetep_get_ProbeMsgId (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!plugin)
    return NULL;

  if (!plugin->msgids)
    return NULL;

  return plugin->msgids->ProbeMsgId;
}

int
ws4d_targetep_set_ResolveMsgId (struct ws4d_epr *epr,
                                const char *ResolveMsgId)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!ResolveMsgId)
    return WS4D_ERR;

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  if (!plugin->msgids)
    {
      plugin->msgids =
        ws4d_malloc_alist (sizeof (struct ws4d_targetep_msgids),
                           ws4d_epr_get_alist (epr));
      if (!plugin->msgids)
        return WS4D_EOM;
    }

  if (plugin->msgids->ResolveMsgId)
    ws4d_free_alist ((void *) plugin->msgids->ResolveMsgId);

  plugin->msgids->ResolveMsgId = ws4d_strdup (ResolveMsgId,
                                              ws4d_epr_get_alist (epr));

  if (plugin->msgids->ResolveMsgId)
    {
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

const char *
ws4d_targetep_get_ResolveMsgId (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!plugin)
    return NULL;

  if (!plugin->msgids)
    return NULL;

  return plugin->msgids->ResolveMsgId;
}

int
ws4d_targetep_set_Types (struct ws4d_epr *epr, const char *Types)
{
  int err;
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!Types)
    return WS4D_ERR;

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  ws4d_qnamelist_clear (&plugin->Types);
  err =
    ws4d_qnamelist_addstring (Types, &plugin->Types,
                              ws4d_epr_get_alist (epr));
  if (err != WS4D_OK)
    return err;

  if (plugin->TypeStr)
    ws4d_free_alist ((void *) plugin->TypeStr);

  plugin->TypeStr = ws4d_qnamelist_tostring (&plugin->Types,
                                             ws4d_epr_get_alist (epr));

  return WS4D_OK;
}

int
ws4d_targetep_add_type (struct ws4d_epr *epr, struct ws4d_qname *type)
{
  int err;
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin, WS4D_ERR);

  err = ws4d_qnamelist_add (type, &plugin->Types);
  if (err != WS4D_OK)
    return err;

  if (plugin->TypeStr)
    ws4d_free_alist ((void *) plugin->TypeStr);

  plugin->TypeStr = ws4d_qnamelist_tostring (&plugin->Types,
                                             ws4d_epr_get_alist (epr));

  return _ws4d_targetep_changed (plugin);
}

int
ws4d_targetep_add_typestr (struct ws4d_epr *epr, const char *typestr)
{
  int err;
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin, WS4D_ERR);

  err =
    ws4d_qnamelist_addstring (typestr, &plugin->Types,
                              ws4d_epr_get_alist (epr));
  if (err != WS4D_OK)
    return err;

  if (plugin->TypeStr)
    ws4d_free_alist ((void *) plugin->TypeStr);

  plugin->TypeStr = ws4d_qnamelist_tostring (&plugin->Types,
                                             ws4d_epr_get_alist (epr));

  return _ws4d_targetep_changed (plugin);
}

int
ws4d_targetep_del_type (struct ws4d_epr *epr, struct ws4d_qname *type)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin, WS4D_ERR);

  ws4d_qnamelist_remove (type);

  if (plugin->TypeStr)
    ws4d_free_alist ((void *) plugin->TypeStr);

  plugin->TypeStr = ws4d_qnamelist_tostring (&plugin->Types,
                                             ws4d_epr_get_alist (epr));

  return _ws4d_targetep_changed (plugin);
}

const char *
ws4d_targetep_get_Types (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!plugin)
    return NULL;

  if (!plugin->TypeStr)
    plugin->TypeStr = ws4d_qnamelist_tostring (&plugin->Types,
                                               ws4d_epr_get_alist (epr));

  return plugin->TypeStr;
}

ws4d_qnamelist *
ws4d_targetep_get_TypeList (struct ws4d_epr * epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!plugin)
    return NULL;

  return &plugin->Types;
}

int
ws4d_targetep_matches_Types (struct ws4d_epr *epr, const char **Types)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);
  int matches;

  if (Types && *Types)
    {

      do
        {
          register struct ws4d_qname *qname, *next;
          matches = 0;

          ws4d_list_foreach (qname, next, &plugin->Types, struct ws4d_qname,
                             list)
          {
            char buffer[256], *mytype;

            mytype = ws4d_qname_tostring2 (qname, buffer, 256);
            if (mytype)
              {
                if (!strcmp (mytype, *Types))
                  {
                    matches = 1;
                    break;
                  }
              }

          }
          Types++;

        }
      while (*Types && matches);

      if (matches)
        {
          return WS4D_OK;
        }
    }

  return WS4D_ERR;
}

int
ws4d_targetep_add_Scope (struct ws4d_epr *epr, const char *Scope)
{
  int err;
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!Scope)
    return WS4D_ERR;

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  if (memchr (Scope, ' ', strlen (Scope)) == NULL)
    {
      err = ws4d_sl_add (&plugin->Scopes, Scope, ws4d_epr_get_alist (epr));
      if (err != WS4D_OK)
        return err;
    }
  else
    {
      char **scope_list = NULL, **temp = NULL;

      scope_list = ws4d_xsdList_to_Array (Scope, ws4d_epr_get_alist (epr));
      temp = scope_list;

      while (*temp)
        {
          if (*temp)
            {
              ws4d_sl_add (&plugin->Scopes, *temp, ws4d_epr_get_alist (epr));
            }
          temp++;
        }

      ws4d_free_xsdArray (scope_list);
    }

  if (plugin->ScopeStr)
    ws4d_free_alist ((void *) plugin->ScopeStr);

  plugin->ScopeStr =
    ws4d_sl_tostr (&plugin->Scopes, ws4d_epr_get_alist (epr));

  return WS4D_OK;
}

int
ws4d_targetep_set_Scopes (struct ws4d_epr *epr, const char *Scopes)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!Scopes)
    return WS4D_ERR;

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  ws4d_sl_done (&plugin->Scopes);

  return ws4d_targetep_add_Scope (epr, Scopes);
}

int
ws4d_targetep_del_Scope (struct ws4d_epr *epr, const char *Scope)
{
  int err;
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!Scope)
    return WS4D_ERR;

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  err = ws4d_sl_del (&plugin->Scopes, Scope);
  if (err != WS4D_OK)
    return err;

  if (plugin->ScopeStr)
    ws4d_free_alist ((void *) plugin->ScopeStr);

  plugin->ScopeStr =
    ws4d_sl_tostr (&plugin->Scopes, ws4d_epr_get_alist (epr));

  return WS4D_OK;
}

const char *
ws4d_targetep_get_Scopes (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!plugin)
    return NULL;

  return plugin->ScopeStr;
}

int
ws4d_targetep_matches_Scopes (struct ws4d_epr *epr, const char **Scopes,
                              int (*scope_match_func) (const char *s1,
                                                       const char *s2))
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);
  int matches;

  ws4d_assert (scope_match_func, WS4D_ERR);

  if (Scopes && *Scopes)
    {

      do
        {
          register struct ws4d_scope *cur = NULL, *next = NULL;
          matches = 0;

          ws4d_list_foreach (cur, next, &plugin->Scopes,
                             struct ws4d_scope, list)
          {
            if (cur && cur->scope)
              {
                if (!scope_match_func (cur->scope, *Scopes))
                  {
                    matches = 1;
                    break;
                  }
              }
          }
          Scopes++;

        }
      while (*Scopes && matches);

      if (matches)
        {
          return WS4D_OK;
        }
    }

  return WS4D_ERR;
}

int
ws4d_targetep_set_MetadataVersion (struct ws4d_epr *epr, int MetadataVersion)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  if (!plugin)
    {
      ws4d_register_targetep (epr);
      plugin = ws4d_targetep_plugindata (epr);
      if (!plugin)
        return WS4D_EOM;
    }

  plugin->MetadataVersion = MetadataVersion;

  return WS4D_OK;
}

int
ws4d_targetep_get_MetadataVersion (struct ws4d_epr *elem)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (elem);

  if (!plugin)
    return -1;

  return plugin->MetadataVersion;
}

/* TODO: set InstanceId if != 0 */
/* TODO: set MessageNumber if != 0 */
int
ws4d_targetep_activate (struct ws4d_epr *epr, int MessageNumber,
                        int InstanceId, int MetadataVersion)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  WS4D_UNUSED_PARAM (MessageNumber);
  WS4D_UNUSED_PARAM (InstanceId);

  ws4d_assert (plugin, WS4D_ERR);

  ws4d_epr_lock (epr);
  if (plugin->state != WS4D_TARGET_INACTIVE)
    {
      ws4d_epr_unlock (epr);
      return WS4D_ERR;
    }

  plugin->state = WS4D_TARGET_ACTIVE;

  if (MetadataVersion > 0)
    {
      plugin->MetadataVersion = MetadataVersion;
    }
  else
    {
      plugin->MetadataVersion = ws4d_systime_s ();
    }

  ws4d_epr_unlock (epr);
  return WS4D_OK;
}

int
ws4d_targetep_deactivate (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin, WS4D_ERR);

  ws4d_epr_lock (epr);
  if (plugin->state != WS4D_TARGET_ACTIVE)
    {
      ws4d_epr_unlock (epr);
      return WS4D_ERR;
    }

  plugin->state = WS4D_TARGET_INACTIVE;

  ws4d_epr_unlock (epr);
  return WS4D_OK;
}

int
ws4d_targetep_isactive (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin, WS4D_ERR);

  return plugin->state == WS4D_TARGET_ACTIVE;
}

int
ws4d_targetep_changeMetadata (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin, WS4D_ERR);

  return _ws4d_targetep_changed (plugin);
}

int
ws4d_targetep_set_ts (struct ws4d_epr *epr, void *targetservice)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin && targetservice, WS4D_ERR);

  plugin->targetservice = targetservice;

  return WS4D_OK;
}

void *
ws4d_targetep_get_ts (struct ws4d_epr *epr)
{
  struct ws4d_targetep_plugin *plugin = ws4d_targetep_plugindata (epr);

  ws4d_assert (plugin, NULL);

  return plugin->targetservice;
}
