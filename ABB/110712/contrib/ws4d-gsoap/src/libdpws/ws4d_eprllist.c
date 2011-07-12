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
#include "ws4d_eprllist.h"

static const char *id = "WS4D-Linked-Epr-List-1.0";

struct ws4d_eprlist_data
{
  struct ws4d_list_node head;
  ws4d_alloc_list alist;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
};

#define eprllist_checkid(list) \
  ((list)->id && (((list)->id == id)||(!strcmp((list)->id, id))))

#define eprllist_iselement(epr, list) \
  (epr)->elementof && ((epr)->elementof == (list))

static int eprllist_fclear (struct ws4d_abs_eprlist *list);

static void
eprllist_fdone (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list),);
  data = list->data;

  eprllist_fclear (list);

  WS4D_INIT_LIST (&data->head);
  ws4d_alloclist_done (&data->alist);

  WS4D_FREE (data);

  return;
}

static struct ws4d_epr *
eprllist_falloc (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list), NULL);
  data = list->data;

  return ws4d_epr_alloc (1, &data->alist);
}

static int
eprllist_ffree (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  ws4d_assert (eprllist_checkid (list), WS4D_ERR);

  return ws4d_epr_free (1, epr);
}

static int
eprllist_fadd (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list), WS4D_ERR);
  data = list->data;

  if (epr->elementof)
    {
      return WS4D_ERR;
    }
  else
    {
      epr->elementof = list;
    }

  ws4d_list_add_tail (&epr->list, &data->head);

  return WS4D_OK;
}

static int
eprllist_fremove (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list), WS4D_ERR);
  data = list->data;

  if (eprllist_iselement (epr, list))
    {
      epr->elementof = NULL;
    }
  else
    {
      return WS4D_ERR;
    }

  ws4d_list_del (&epr->list);

  return WS4D_OK;
}

static int
eprllist_fisempty (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list), WS4D_ERR);
  data = list->data;

  return ws4d_list_empty (&data->head);
}

static struct ws4d_epr *
eprllist_fget_first (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list), NULL);
  data = list->data;

  if (eprllist_fisempty (list))
    {
      return NULL;
    }
  else
    {
      return ws4d_list_entry (data->head.next, struct ws4d_epr, list);
    }
}

static struct ws4d_epr *
eprllist_fget_next (struct ws4d_abs_eprlist *list, struct ws4d_epr *epr)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list), NULL);
  data = list->data;

  if (epr)
    {
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
  else
    {
      return NULL;
    }
}

static struct ws4d_epr *
eprllist_get_byAddr (struct ws4d_abs_eprlist *list, const char *Addr)
{
  struct ws4d_eprlist_data *data;
  register struct ws4d_epr *elem = NULL, *next = NULL;

  ws4d_assert (eprllist_checkid (list), NULL);
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
eprllist_flock (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list),);
  data = list->data;

  ws4d_mutex_lock (&data->lock);
}

static void
eprllist_funlock (struct ws4d_abs_eprlist *list)
{
  struct ws4d_eprlist_data *data;

  ws4d_assert (eprllist_checkid (list),);
  data = list->data;

  ws4d_mutex_unlock (&data->lock);
}
#endif

static int
eprllist_fclear (struct ws4d_abs_eprlist *list)
{
  register struct ws4d_epr *elem = NULL, *next = NULL;

  ws4d_assert (eprllist_checkid (list), WS4D_ERR);

  for (elem = eprllist_fget_first (list), next
       = eprllist_fget_next (list, elem); elem; elem = next, next
       = eprllist_fget_next (list, next))
    {
      eprllist_fremove (list, elem);
      eprllist_ffree (list, elem);
    }

  return WS4D_OK;
}

static struct ws4d_abs_eprlist_cb callbacks = {
  eprllist_fdone,
  eprllist_falloc,
  eprllist_ffree,
  eprllist_fadd,
  eprllist_fremove,
  eprllist_fclear,
  eprllist_fisempty,
  eprllist_fget_first,
  eprllist_fget_next,
  eprllist_get_byAddr,
#ifdef WITH_MUTEXES
  eprllist_flock,
  eprllist_funlock
#else
  NULL,
  NULL
#endif
};

int
ws4d_eprllist_init (struct ws4d_abs_eprlist *head, void *arg)
{
  struct ws4d_eprlist_data *data = NULL;

  WS4D_UNUSED_PARAM (arg);

  head->id = id;

  data = WS4D_MALLOC (sizeof (struct ws4d_eprlist_data));

  WS4D_INIT_LIST (&data->head);
  WS4D_ALLOCLIST_INIT (&data->alist);
#ifdef WITH_MUTEXES
  ws4d_mutex_init (&data->lock);
#endif

  head->data = data;
  head->callbacks = &callbacks;

  return WS4D_OK;
}
