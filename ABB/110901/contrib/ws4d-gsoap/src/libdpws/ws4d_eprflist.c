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

#define ABSTRACT_EPRLIST_IMPLEMENT
#include "ws4d_abstract_eprlist.h"
#include "ws4d_eprflist.h"

const char *id = "WS4D-Fixed-Epr-List-1.0";

struct ws4d_eprlist_data
{
  struct ws4d_list_node head;
  struct ws4d_list_node alloced;
  struct ws4d_list_node free;
  struct ws4d_epr *buffer;
  int size;
  struct ws4d_epr *(*freplace) (struct ws4d_abs_eprlist * list);
  ws4d_alloc_list alist;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
};

#define eprflist_checkid(list) \
  (list && list->id && ((list->id == id)||(!strcmp(list->id, id))))

#define ws4d_eprlist_is_in(epr, head, is_in) \
do \
{ \
  register struct ws4d_list_node *cur = &epr->list; \
  is_in = 0; \
  while (cur && ((cur->next != head) || (cur->next != &epr->list))) \
  { \
    cur = cur->next; \
  } \
  if (cur->next == head) \
  { \
    is_in = 1; \
  } \
} while(0)

static void
eprflist_fdone (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data = list->data;
  register struct ws4d_epr *elem;
  int i;

  ws4d_assert (eprflist_checkid (list) && data && list->callbacks->fclear,);

  list->callbacks->fclear (list);

  for (i = 0, elem = data->buffer; i < data->size; i++, elem++)
    {
      ws4d_list_del (&elem->list);
      elem->elementof = NULL;
    }

  ws4d_epr_free (data->size, data->buffer);

  data->size = 0;
  data->buffer = NULL;
  data->freplace = 0;

  WS4D_INIT_LIST (&data->head);
  WS4D_INIT_LIST (&data->alloced);
  WS4D_INIT_LIST (&data->free);

  ws4d_alloclist_done (&data->alist);

  WS4D_FREE (data);

  return;
}

static struct ws4d_epr *
eprflist_falloc (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;
  register struct ws4d_epr *res;

  ws4d_assert (eprflist_checkid (list), NULL);
  data = list->data;

  if (ws4d_list_empty (&data->free))
    {
      res = data->freplace (list);
      if (res == NULL)
        {
          return NULL;
        }
    }
  else
    {
      res = ws4d_list_entry (data->free.next, struct ws4d_epr, list);
    }

  ws4d_list_move (&res->list, &data->alloced);

  ws4d_epr_done (res);

  return res;
}

static int
eprflist_ffree (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  struct ws4d_eprlist_data *data;
  int is_alloced = 0;

  ws4d_assert (eprflist_checkid (list) && epr, WS4D_ERR);
  data = list->data;

  if (!epr->elementof || (epr->elementof != list))
    {
      return WS4D_ERR;
    }

  /* check if epr is alloced */
  ws4d_eprlist_is_in (epr, &data->alloced, is_alloced);
  if (!is_alloced)
    {
      return WS4D_ERR;
    }

  ws4d_list_move_tail (&epr->list, &data->free);

  return WS4D_OK;
}

static int
eprflist_fadd (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  struct ws4d_eprlist_data *data;
  int is_alloced = 0;

  ws4d_assert (eprflist_checkid (list) && epr, WS4D_ERR);
  data = list->data;

  if (!epr->elementof || (epr->elementof != list))
    {
      return WS4D_ERR;
    }

  /* check if epr is alloced */
  ws4d_eprlist_is_in (epr, &data->alloced, is_alloced);
  if (!is_alloced)
    {
      return WS4D_ERR;
    }

  ws4d_list_move_tail (&epr->list, &data->head);

  return WS4D_OK;
}

static int
eprflist_fremove (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  struct ws4d_eprlist_data *data;
  int is_inhead = 0;

  ws4d_assert (eprflist_checkid (list) && epr, WS4D_ERR);
  data = list->data;

  if (!epr->elementof || (epr->elementof != list))
    {
      return WS4D_ERR;
    }

  /* check if epr is in head */
  ws4d_eprlist_is_in (epr, &data->alloced, is_inhead);
  if (!is_inhead)
    {
      return WS4D_ERR;
    }

  ws4d_list_move (&epr->list, &data->alloced);

  return WS4D_OK;
}

static int
eprflist_fisempty (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprflist_checkid (list), WS4D_ERR);
  data = list->data;

  return ws4d_list_empty (&data->head);
}

static struct ws4d_epr *
eprflist_fget_first (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprflist_checkid (list), NULL);
  data = list->data;

  if (eprflist_fisempty (list))
    {
      return NULL;
    }
  else
    {
      return ws4d_list_entry (data->head.next, struct ws4d_epr, list);
    }
}

static struct ws4d_epr *
eprflist_fget_next (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprflist_checkid (list) && epr, NULL);
  data = list->data;

  ws4d_list_check (&epr->list);
  if (epr->list.next == &data->head)
    {
      return NULL;
    }
  else
    {
      return ws4d_list_entry (epr->list.next, struct ws4d_epr, list);
    }
}

static struct ws4d_epr *
eprflist_get_byAddr (struct ws4d_abs_eprlist *list, const char *Addr)
{
  struct ws4d_eprlist_data *data;
  register struct ws4d_epr *elem = NULL, *next = NULL;

  ws4d_assert (eprflist_checkid (list) && Addr, NULL);
  data = list->data;

  ws4d_list_foreach (elem, next, &data->head, struct ws4d_epr, list)
  {
    char *Address = (char *) ws4d_epr_get_Addrs (elem);
    if (Address && !strcmp (Addr, Address))
      {
        return elem;
      }
  }

  return NULL;
}

#ifdef WITH_MUTEXES
static void
eprflist_flock (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprflist_checkid (list),);
  data = list->data;

  ws4d_mutex_lock (&data->lock);
}

static void
eprflist_funlock (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprflist_checkid (list),);
  data = list->data;

  ws4d_mutex_unlock (&data->lock);
}
#endif

static int
eprflist_fclear (struct ws4d_abs_eprlist *list)
{
  register struct ws4d_epr *elem = NULL, *next = NULL;

  ws4d_assert (eprflist_checkid (list), WS4D_ERR);

  for (elem = eprflist_fget_first (list), next
       = eprflist_fget_next (list, elem); elem; elem = next, next
       = eprflist_fget_next (list, next))
    {
      eprflist_fremove (list, elem);
      eprflist_ffree (list, elem);
    }

  return WS4D_OK;
}

static struct ws4d_abs_eprlist_cb callbacks = {
  eprflist_fdone,
  eprflist_falloc,
  eprflist_ffree,
  eprflist_fadd,
  eprflist_fremove,
  eprflist_fclear,
  eprflist_fisempty,
  eprflist_fget_first,
  eprflist_fget_next,
  eprflist_get_byAddr,
#ifdef WITH_MUTEXES
  eprflist_flock,
  eprflist_funlock
#else
  NULL,
  NULL
#endif
};

int
ws4d_eprflist_init (struct ws4d_abs_eprlist *head, void *arg)
{
  struct ws4d_eprlist_data *data = NULL;
  register struct ws4d_epr *elem = NULL;
  struct wsa_eprflist_params *params = (struct wsa_eprflist_params *) arg;
  int i = 0;

  ws4d_assert (head && params && (params->size > 0), WS4D_ERR);

  data = WS4D_MALLOC (sizeof (struct ws4d_eprlist_data));

  WS4D_INIT_LIST (&data->head);
  WS4D_INIT_LIST (&data->alloced);
  WS4D_INIT_LIST (&data->free);
  WS4D_ALLOCLIST_INIT (&data->alist);
  data->size = params->size;
  data->buffer = ws4d_epr_alloc (params->size, &data->alist);
  if (params->freplace)
    {
      data->freplace = params->freplace;
    }
  else
    {
      data->freplace = eprflist_fget_first;
    }

  for (i = 0, elem = data->buffer; i < params->size; i++, elem++)
    {
      ws4d_list_add (&elem->list, &data->free);
      elem->elementof = head;
    }

  head->id = id;
  head->data = data;
  head->callbacks = &callbacks;

  return WS4D_OK;
}
