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
#include "ws4d_subscription.h"

int
ws4d_subs_init (struct ws4d_subscription *subs, const char *id)
{
  ws4d_assert (subs && id, WS4D_ERR);

  WS4D_ALLOCLIST_INIT (&subs->alist);

  subs->id = ws4d_strdup (id, &subs->alist);

  return WS4D_OK;
}

int
ws4d_subs_done (struct ws4d_subscription *subs)
{
  ws4d_assert (subs && subs->id, WS4D_ERR);

  /* free value */
  ws4d_free_alist ((void *) subs->id);

  if (subs->endto)
    ws4d_free_alist ((void *) subs->endto);

  if (subs->subsm)
    ws4d_free_alist ((void *) subs->subsm);

  ws4d_alloclist_done (&subs->alist);

  return WS4D_OK;
}

int
ws4d_subs_set_endto (struct ws4d_subscription *subs, const char *endto)
{
  ws4d_assert (subs && endto, WS4D_ERR);

  if (subs->endto)
    ws4d_free_alist ((void *) subs->endto);

  subs->endto = ws4d_strdup (endto, ws4d_subs_get_alist (subs));
  if (!subs->endto)
    return WS4D_EOM;

  return WS4D_OK;
}

const char *
ws4d_subs_get_subsm (struct ws4d_subscription *subs)
{
  ws4d_assert (subs, NULL);

  return subs->subsm;
}

int
ws4d_subs_set_subsm (struct ws4d_subscription *subs, const char *subsm)
{
  ws4d_assert (subs && subsm, WS4D_ERR);

  if (subs->subsm)
    ws4d_free_alist ((void *) subs->subsm);

  subs->subsm = ws4d_strdup (subsm, ws4d_subs_get_alist (subs));
  if (!subs->subsm)
    return WS4D_EOM;

  return WS4D_OK;
}

int
ws4d_subs_set_expires (struct ws4d_subscription *subs, ws4d_time exptime)
{
  ws4d_assert (subs, WS4D_ERR);

  subs->expiration = exptime;

  return WS4D_OK;
}

const char *
ws4d_subs_get_id (struct ws4d_subscription *subs)
{
  ws4d_assert (subs, NULL);

  return subs->id;
}

ws4d_time
ws4d_subs_get_expires (struct ws4d_subscription * subs)
{
  ws4d_assert (subs, WS4D_ERR);

  return subs->expiration;
}

int
ws4d_subs_dup (struct ws4d_subscription *src, struct ws4d_subscription *dst)
{
  ws4d_assert (src && src->id && dst && (src != dst), WS4D_ERR);

  if (dst->id)
  {
    ws4d_free_alist ((void *) dst->id);

    dst->id = ws4d_strdup (src->id, ws4d_subs_get_alist (dst));
    if (!dst->id)
      return WS4D_EOM;
  }
  else
  {
    ws4d_subs_init(dst, src->id);
  }


  if (src->endto)
    {
      if (dst->endto)
        ws4d_free_alist ((void *) dst->endto);

      dst->endto = ws4d_strdup (src->endto, ws4d_subs_get_alist (dst));
      if (!dst->endto)
        return WS4D_EOM;
    }

  if (src->subsm)
    {
      if (dst->subsm)
        ws4d_free_alist ((void *) dst->subsm);

      dst->subsm = ws4d_strdup (src->subsm, ws4d_subs_get_alist (dst));
      if (!dst->subsm)
        return WS4D_EOM;
    }

  if (src->filter_mode)
    {
      /* TODO: copy filter mode */
    }

  if (src->delivery_mode)
    {
      /* TODO: copy delivery mode */
    }

  dst->expiration = src->expiration;

  return WS4D_OK;
}

struct ws4d_subscription *
ws4d_subslist_getsubs (struct ws4d_list_node *list, const char *id)
{
  register struct ws4d_subscription *cur = NULL, *next = NULL;

  ws4d_assert (list && id, NULL);

  ws4d_list_foreach (cur, next, list, struct ws4d_subscription, list)
  {
    if (!strcmp (cur->id, id))
      return cur;
  }

  return NULL;
}

struct ws4d_subscription *
ws4d_subslist_initsubs (struct ws4d_list_node *list, const char *id,
                        ws4d_alloc_list * alist)
{
  struct ws4d_subscription *new_subs;

  ws4d_assert (list && id && alist, NULL);

  /* is key already registered? */
  if (ws4d_subslist_getsubs (list, id) != NULL)
    {
      return NULL;
    }

  /* allocate memory */
  new_subs = ws4d_malloc_alist (sizeof (struct ws4d_subscription), alist);
  if (new_subs == NULL)
    {
      return NULL;
    }

  ws4d_subs_init (new_subs, id);

  /* add subs to list */
  ws4d_list_add (&new_subs->list, list);

  return new_subs;
}

static int
_ws4d_subslist_rmsubs (struct ws4d_subscription *subs)
{
  ws4d_assert (subs, WS4D_ERR);

  ws4d_subs_done (subs);

  /* remove key from list */
  ws4d_list_del (&subs->list);

  /* free kvp structure */
  ws4d_free_alist (subs);

  return WS4D_OK;
}

int
ws4d_subslist_rmsubs (struct ws4d_list_node *list, const char *id)
{
  struct ws4d_subscription *del_subs;

  ws4d_assert (list && id, WS4D_ERR);

  /* check if key exist ? */
  del_subs = ws4d_subslist_getsubs (list, id);
  if (del_subs == NULL)
    {
      return WS4D_ERR;
    }

  /* remove key */
  return _ws4d_subslist_rmsubs (del_subs);
}

int
ws4d_subslist_dup (struct ws4d_list_node *src, struct ws4d_list_node *dst,
                   ws4d_alloc_list * alist)
{
  register struct ws4d_subscription *cur = NULL, *next = NULL;
  int dst_empty;

  ws4d_assert (src && dst && alist, WS4D_ERR);

  /* remember if destination list is empty */
  dst_empty = ws4d_list_empty (dst);

  ws4d_list_foreach (cur, next, src, struct ws4d_subscription, list)
  {
    struct ws4d_subscription *new_subs = NULL;
    int replace = 0, err;

    /* search for existing key in destination list */
    if (!dst_empty)
      {
        new_subs = ws4d_subslist_getsubs (dst, cur->id);
        replace = 1;
      }

    /* create new key for destination list */
    if (new_subs == NULL)
      {
        new_subs =
          ws4d_malloc_alist (sizeof (struct ws4d_subscription), alist);
        if (new_subs == NULL)
          {
            return WS4D_EOM;
          }
      }

    /* duplicate key and value */
    err = ws4d_subs_dup (cur, new_subs);
    if (err != WS4D_OK)
      {
        return err;
      }

    /* if this is a new key, register it */
    if (!replace)
      {
        ws4d_list_add (&new_subs->list, dst);
      }
  }

  return WS4D_OK;
}

int
ws4d_subslist_clear (struct ws4d_list_node *list)
{
  register struct ws4d_subscription *cur = NULL, *next = NULL;

  ws4d_assert (list, WS4D_ERR);

  /* clear all kvps */
  ws4d_list_foreach (cur, next, list, struct ws4d_subscription, list)
  {
    int err;

    err = _ws4d_subslist_rmsubs (cur);
    if (err != WS4D_OK)
      {
        return err;
      }
  }

  return WS4D_OK;
}
