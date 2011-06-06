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
#include "ws4d_epr_kv.h"

/*****************************************************************************
 * Key value list
 ****************************************************************************/
union ws4d_kv_union
{
  int integer;
  char *string;
  struct
  {
    void *data;
    size_t size;
  } _void;
};

struct ws4d_kv_s
{
  struct ws4d_list_node list;
  const char *key;
  unsigned int type;
  union ws4d_kv_union *val;
};

static struct ws4d_kv_s *
ws4d_kv_getkey (struct ws4d_list_node *kvlist, const char *key)
{
  register struct ws4d_kv_s *cur = NULL, *next = NULL;

  ws4d_assert (kvlist && key, NULL);

  /* first search by key reference */
  ws4d_list_foreach (cur, next, kvlist, struct ws4d_kv_s, list)
  {
    if (cur->key == key)
      {
        return cur;
      }
  }

  /* then search by ky value */
  ws4d_list_foreach (cur, next, kvlist, struct ws4d_kv_s, list)
  {
    if (!strcmp (cur->key, key))
      {
        return cur;
      }
  }

  /* not found */
  return NULL;
}

static struct ws4d_kv_s *
ws4d_kv_createkey (struct ws4d_list_node *kvlist, const char *key,
                   ws4d_alloc_list * alist)
{
  struct ws4d_kv_s *new_key;

  ws4d_assert (kvlist && key && alist, NULL);

  /* is key already registered? */
  if (ws4d_kv_getkey (kvlist, key) != NULL)
    {
      return NULL;
    }

  /* allocate memory */
  new_key = ws4d_malloc_alist (sizeof (struct ws4d_kv_s), alist);
  if (new_key == NULL)
    {
      return NULL;
    }
  /* assign key by reference !!! */
  new_key->key = key;

  /* add key to list */
  ws4d_list_add (&new_key->list, kvlist);

  return new_key;
}

static int
ws4d_kv_key_setintval (struct ws4d_kv_s *kventry, int integer,
                       ws4d_alloc_list * alist)
{
  ws4d_assert (kventry && alist, WS4D_ERR);

  /* allocate memory for val union */
  if (kventry->type == WS4D_KEYTYPE_NONE)
    {
      kventry->val = ws4d_malloc_alist (sizeof (union ws4d_kv_union), alist);
      if (kventry->val == NULL)
        {
          return WS4D_EOM;
        }
      kventry->type = WS4D_KEYTYPE_INTEGER;
    }

  /* don't set value of different type */
  if (kventry->type != WS4D_KEYTYPE_INTEGER)
    {
      return WS4D_ERR;
    }

  /* set val */
  kventry->val->integer = integer;

  return WS4D_OK;
}

static int
ws4d_kv_key_setstringval (struct ws4d_kv_s *kventry, const char *string,
                          ws4d_alloc_list * alist)
{
  ws4d_assert (kventry && string && alist, WS4D_ERR);

  /* allocate memory for val union */
  if (kventry->type == WS4D_KEYTYPE_NONE)
    {
      kventry->val = ws4d_malloc_alist (sizeof (union ws4d_kv_union), alist);
      if (kventry->val == NULL)
        {
          return WS4D_EOM;
        }
      kventry->type = WS4D_KEYTYPE_STRING;
    }

  /* don't set value of different type */
  if (kventry->type != WS4D_KEYTYPE_STRING)
    {
      return WS4D_ERR;
    }

  /* free old string */
  if (kventry->val->string)
    {
      ws4d_free_alist (kventry->val->string);
    }

  /* set val */
  kventry->val->string = ws4d_strdup (string, alist);

  return WS4D_OK;
}

static int
ws4d_kv_key_setvoidval (struct ws4d_kv_s *kventry, void *data, size_t size,
                        ws4d_alloc_list * alist)
{
  ws4d_assert (kventry && data && (size > 0) && alist, WS4D_ERR);

  /* allocate memory for val union */
  if (kventry->type == WS4D_KEYTYPE_NONE)
    {
      kventry->val = ws4d_malloc_alist (sizeof (union ws4d_kv_union), alist);
      if (kventry->val == NULL)
        {
          return WS4D_EOM;
        }
      kventry->type = WS4D_KEYTYPE_VOID;
    }

  /* don't set value of different type */
  if (kventry->type != WS4D_KEYTYPE_VOID)
    {
      return WS4D_ERR;
    }

  /* free old string */
  if (kventry->val->_void.data)
    {
      ws4d_free_alist (kventry->val->_void.data);
    }

  /* set val */
  kventry->val->_void.data = ws4d_memdup (data, size, alist);
  kventry->val->_void.size = size;

  return WS4D_OK;
}

static int
ws4d_kv_dupkey (struct ws4d_kv_s *src, struct ws4d_kv_s *dst,
                ws4d_alloc_list * alist)
{
  int err = WS4D_OK;

  ws4d_assert (src && dst && alist, WS4D_ERR);

  /* test if src and dst key hav different types */
  if ((dst->type != WS4D_KEYTYPE_NONE) && (dst->type != src->type))
    {
      return WS4D_ERR;
    }

  /* copy value */
  switch (src->type)
    {
    case WS4D_KEYTYPE_NONE:
      break;
    case WS4D_KEYTYPE_INTEGER:
      err = ws4d_kv_key_setintval (dst, src->val->integer, alist);
      break;
    case WS4D_KEYTYPE_STRING:
      err = ws4d_kv_key_setstringval (dst, src->val->string, alist);
      break;
    case WS4D_KEYTYPE_VOID:
      err =
        ws4d_kv_key_setvoidval (dst, src->val->_void.data,
                                src->val->_void.size, alist);
      break;
    default:
      return WS4D_ERR;
    }

  /* if copy of value failed? */
  if (err != WS4D_OK)
    {
      return err;
    }

  /* copy key by reference */
  dst->key = src->key;

  return WS4D_OK;
}

static int
_ws4d_kv_rmkey (struct ws4d_kv_s *del_key)
{
  ws4d_assert (del_key, WS4D_ERR);

  /* free value */
  switch (del_key->type)
    {
    case WS4D_KEYTYPE_NONE:
      break;
    case WS4D_KEYTYPE_INTEGER:
      ws4d_free_alist (del_key->val);
      break;
    case WS4D_KEYTYPE_STRING:
      ws4d_free_alist (del_key->val->string);
      ws4d_free_alist (del_key->val);
      break;
    case WS4D_KEYTYPE_VOID:
      ws4d_free_alist (del_key->val->_void.data);
      ws4d_free_alist (del_key->val);
      break;
    }

  /* remove key from list */
  ws4d_list_del (&del_key->list);

  /* free kvp structure */
  ws4d_free_alist (del_key);

  return WS4D_OK;
}

static int
ws4d_kv_rmkey (struct ws4d_list_node *kvlist, const char *key)
{
  struct ws4d_kv_s *del_key;

  ws4d_assert (kvlist && key, WS4D_ERR);

  /* check if key exist ? */
  del_key = ws4d_kv_getkey (kvlist, key);
  if (del_key == NULL)
    {
      return WS4D_ERR;
    }

  /* remove key */
  return _ws4d_kv_rmkey (del_key);
}

static int
ws4d_kv_listdup (struct ws4d_list_node *src, struct ws4d_list_node *dst,
                 ws4d_alloc_list * alist)
{
  register struct ws4d_kv_s *cur = NULL, *next = NULL;
  int dst_empty;

  ws4d_assert (src && dst && alist, WS4D_ERR);

  /* remember if destination list is empty */
  dst_empty = ws4d_list_empty (dst);

  ws4d_list_foreach (cur, next, src, struct ws4d_kv_s, list)
  {
    struct ws4d_kv_s *new_key = NULL;
    int replace = 0, err;

    /* search for existing key in destination list */
    if (!dst_empty)
      {
        new_key = ws4d_kv_getkey (dst, cur->key);
        replace = 1;
      }

    /* create new key for destination list */
    if (new_key == NULL)
      {
        new_key = ws4d_malloc_alist (sizeof (struct ws4d_kv_s), alist);
        if (new_key == NULL)
          {
            return WS4D_EOM;
          }
      }

    /* duplicate key and value */
    err = ws4d_kv_dupkey (cur, new_key, alist);
    if (err != WS4D_OK)
      {
        return err;
      }

    /* if this is a new key, register it */
    if (!replace)
      {
        ws4d_list_add (&new_key->list, dst);
      }
  }

  return WS4D_OK;
}

static int
ws4d_kv_listclear (struct ws4d_list_node *kvlist)
{
  register struct ws4d_kv_s *cur = NULL, *next = NULL;

  ws4d_assert (kvlist, WS4D_ERR);

  /* clear all kvps */
  ws4d_list_foreach (cur, next, kvlist, struct ws4d_kv_s, list)
  {
    int err;

    err = _ws4d_kv_rmkey (cur);
    if (err != WS4D_OK)
      {
        return err;
      }
  }

  return WS4D_OK;
}

/*****************************************************************************
 * Plugin logic
 ****************************************************************************/

/* epr plugin id */
#define WS4D_EPRKVPLUGIN_ID "WS4D-EPR-KeyValuePlugin-0.1"
const char *ws4d_eprkv_plugin_id = WS4D_EPRKVPLUGIN_ID;

/* epr plugin data */
struct ws4d_eprkv_plugin_s
{
  struct ws4d_list_node kvlist;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (mutex);
#endif
};

/* forward declarations */
INLINE struct ws4d_eprkv_plugin_s
  *ws4d_eprkv_plugindata (struct ws4d_epr *epr);

int ws4d_eprkv_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                      struct ws4d_epr_plugin *src);

void ws4d_eprkv_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p);

int ws4d_eprkv_fcreate (struct ws4d_epr *epr,
                        struct ws4d_epr_plugin *plugin, void *arg);

/* callbacks for epr plugin functions */
static struct ws4d_epr_plugin_cb ws4d_eprkv_callbacks =
  { ws4d_eprkv_fdelete, ws4d_eprkv_fcopy, NULL };

/* get epr kpv plugin data from an epr */
INLINE struct ws4d_eprkv_plugin_s *
ws4d_eprkv_plugindata (struct ws4d_epr *epr)
{
  return (struct ws4d_eprkv_plugin_s *)
    ws4d_epr_get_plugindata (epr, ws4d_eprkv_plugin_id);
}

/* copy callback for plugin copy/update functionality */
int
ws4d_eprkv_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
                  struct ws4d_epr_plugin *src)
{
  struct ws4d_eprkv_plugin_s *dst_data = NULL, *src_data =
    (struct ws4d_eprkv_plugin_s *) src->data;

  /* test parameters */
  ws4d_assert (src_data, WS4D_ERR);

  /* set plugin id and callbacks */
  dst->id = ws4d_eprkv_plugin_id;
  dst->cb = &ws4d_eprkv_callbacks;

  /* allocate plugin data structure if it is not already available */
  if (dst->data)
    {
      dst_data = dst->data;
    }
  else
    {
      dst_data = ws4d_malloc (sizeof (struct ws4d_eprkv_plugin_s));
      if (!dst_data)
        {
          return WS4D_EOM;
        }

      /* initialize plugin data structure */
      WS4D_INIT_LIST (&dst_data->kvlist);
      ws4d_mutex_init (&dst_data->mutex);
    }

  /* copy plugin data from src epr to dst epr */
  ws4d_mutex_lock (&src_data->mutex);
  ws4d_mutex_lock (&dst_data->mutex);
  ws4d_kv_listdup (&src_data->kvlist, &dst_data->kvlist,
                   ws4d_epr_get_alist (epr));
  ws4d_mutex_unlock (&dst_data->mutex);
  ws4d_mutex_unlock (&src_data->mutex);

  /* set plugin data structure as destination plugin data */
  dst->data = dst_data;

  return WS4D_OK;
}

/* delete callback for epr reset and free functionality */
void
ws4d_eprkv_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
{
  struct ws4d_eprkv_plugin_s *plugin = p->data;

  WS4D_UNUSED_PARAM (epr);

  /* if there is plugin data to free */
  if (plugin)
    {
      /* free plugin data structure elements */
      ws4d_mutex_destroy (&plugin->mutex);
      ws4d_kv_listclear (&plugin->kvlist);
      /* free plugin data structure */
      ws4d_free (plugin);
    }
}

/* callback for ws4d_epr_register_plugin() function to create plugin */
int
ws4d_eprkv_fcreate (struct ws4d_epr *epr,
                    struct ws4d_epr_plugin *plugin, void *arg)
{
  struct ws4d_eprkv_plugin_s *data = NULL;

  WS4D_UNUSED_PARAM (epr);
  WS4D_UNUSED_PARAM (arg);

  /* set plugin id and callbacks */
  plugin->id = ws4d_eprkv_plugin_id;
  plugin->cb = &ws4d_eprkv_callbacks;

  /* allocate plugin data structure */
  data = ws4d_malloc (sizeof (struct ws4d_eprkv_plugin_s));
  if (!data)
    {
      return WS4D_EOM;
    }

  /* initialize plugin data structure */
  WS4D_INIT_LIST (&data->kvlist);
  ws4d_mutex_init (&data->mutex);

  /* set plugin data structure as plugin data */
  plugin->data = data;

  return WS4D_OK;
}

/*****************************************************************************
 * API
 ****************************************************************************/

int
ws4d_epr_create_key (struct ws4d_epr *epr, const char *key)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;

  if (!plugin)
    {
      int err;

      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_createkey (&plugin->kvlist, key, ws4d_epr_get_alist (epr));
  ws4d_mutex_unlock (&plugin->mutex);
  if (kv == NULL)
    {
      return WS4D_ERR;
    }
  else
    {
      return WS4D_OK;
    }
}

int
ws4d_epr_has_key (struct ws4d_epr *epr, const char *key)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  int result;

  if (!plugin)
    {
      int err;

      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  result = (ws4d_kv_getkey (&plugin->kvlist, key) != NULL);
  ws4d_mutex_unlock (&plugin->mutex);

  return result;
}

int
ws4d_epr_get_keytype (struct ws4d_epr *epr, const char *key, int *type)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;

  ws4d_assert (epr && key && type, WS4D_ERR);

  if (!plugin)
    {
      int err;

      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_getkey (&plugin->kvlist, key);
  ws4d_mutex_unlock (&plugin->mutex);

  if (kv != NULL)
    {
      *type = kv->type;
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_epr_set_intkey (struct ws4d_epr *epr, const char *key, int integer)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;
  int err;

  ws4d_assert (epr && key, WS4D_ERR);

  if (!plugin)
    {
      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_getkey (&plugin->kvlist, key);
  if (kv == NULL)
    {
      kv = ws4d_kv_createkey (&plugin->kvlist, key, ws4d_epr_get_alist (epr));
    }
  err = ws4d_kv_key_setintval (kv, integer, ws4d_epr_get_alist (epr));
  ws4d_mutex_unlock (&plugin->mutex);

  return err;
}

int
ws4d_epr_get_intkey (struct ws4d_epr *epr, const char *key, int *integer)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;

  ws4d_assert (epr && integer, WS4D_ERR);

  if (!plugin)
    {
      int err;

      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_getkey (&plugin->kvlist, key);
  ws4d_mutex_unlock (&plugin->mutex);

  if ((kv != NULL) && (kv->type == WS4D_KEYTYPE_INTEGER))
    {
      *integer = kv->val->integer;
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_epr_set_stringkey (struct ws4d_epr *epr, const char *key,
                        const char *string)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;
  int err;

  ws4d_assert (epr && key, WS4D_ERR);

  if (!plugin)
    {
      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_getkey (&plugin->kvlist, key);
  if (kv == NULL)
    {
      kv = ws4d_kv_createkey (&plugin->kvlist, key, ws4d_epr_get_alist (epr));
    }
  err = ws4d_kv_key_setstringval (kv, string, ws4d_epr_get_alist (epr));
  ws4d_mutex_unlock (&plugin->mutex);

  return err;
}

int
ws4d_epr_get_stringkey (struct ws4d_epr *epr, const char *key,
                        const char **string)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;

  ws4d_assert (epr && string, WS4D_ERR);

  if (!plugin)
    {
      int err;

      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_getkey (&plugin->kvlist, key);
  ws4d_mutex_unlock (&plugin->mutex);

  if ((kv != NULL) && (kv->type == WS4D_KEYTYPE_STRING))
    {
      *string = kv->val->string;
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_epr_set_voidkey (struct ws4d_epr *epr, const char *key, void *data,
                      size_t size)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;
  int err;

  ws4d_assert (epr && key, WS4D_ERR);

  if (!plugin)
    {
      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_getkey (&plugin->kvlist, key);
  if (kv == NULL)
    {
      kv = ws4d_kv_createkey (&plugin->kvlist, key, ws4d_epr_get_alist (epr));
    }
  err = ws4d_kv_key_setvoidval (kv, data, size, ws4d_epr_get_alist (epr));
  ws4d_mutex_unlock (&plugin->mutex);

  return err;
}

int
ws4d_epr_get_voidkey (struct ws4d_epr *epr, const char *key, void **data,
                      size_t * size)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  struct ws4d_kv_s *kv;

  ws4d_assert (epr && data && size, WS4D_ERR);

  if (!plugin)
    {
      int err;

      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  kv = ws4d_kv_getkey (&plugin->kvlist, key);
  ws4d_mutex_unlock (&plugin->mutex);

  if ((kv != NULL) && (kv->type == WS4D_KEYTYPE_VOID))
    {
      *data = kv->val->_void.data;
      *size = kv->val->_void.size;
      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_epr_remove_key (struct ws4d_epr *epr, const char *key)
{
  struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
  int err;

  if (!plugin)
    {
      err = ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
      if (err != WS4D_OK)
        {
          return err;
        }
      plugin = ws4d_eprkv_plugindata (epr);
    }

  ws4d_mutex_lock (&plugin->mutex);
  err = ws4d_kv_rmkey (&plugin->kvlist, key);
  ws4d_mutex_unlock (&plugin->mutex);

  return err;
}
