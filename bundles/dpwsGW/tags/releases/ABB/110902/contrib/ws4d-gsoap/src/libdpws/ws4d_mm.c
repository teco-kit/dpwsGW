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
 *
 *  Created on: 25.05.2009
 *      Author: elmex
 */

#include "ws4d_misc.h"

void *
ws4d_malloc (size_t length)
{
  void *p = NULL;
  p = WS4D_MALLOC (length);
  return memset (p, 0, length);
}

void
ws4d_free (void *p)
{
  if (p)
    {
      WS4D_FREE (p);
    }
}

/**
 * abstract allocator
 */

int
ws4d_allocator_init (struct ws4d_abs_allocator *allocator,
                     int (*finit) (struct ws4d_abs_allocator *
                                   allocator, void *arg), void *arg)
{
  int ret = 0;

  ws4d_assert (allocator && finit, WS4D_ERR);

  memset (allocator, 0, sizeof (struct ws4d_abs_allocator));
  ret = finit (allocator, arg);
  if (ret != WS4D_OK)
    return ret;

  ws4d_assert (allocator->callbacks
               && allocator->callbacks->falloc
               && allocator->callbacks->fdone
               && allocator->callbacks->ffree
               && allocator->callbacks->ffreeall
               && allocator->callbacks->fmemdup
               && allocator->callbacks->fmemref
               && allocator->callbacks->funref, WS4D_ERR);

  return WS4D_OK;
}

void
ws4d_allocator_done (struct ws4d_abs_allocator *allocator)
{
  ws4d_assert (allocator && allocator->callbacks,);

  if (allocator->callbacks && allocator->callbacks->fdone)
    allocator->callbacks->fdone (allocator);

  memset (allocator, 0, sizeof (struct ws4d_abs_allocator));
}

int
ws4d_allocator_freeall (struct ws4d_abs_allocator *allocator)
{
  ws4d_assert (allocator && allocator->callbacks
               && allocator->callbacks->ffreeall, WS4D_ERR);

  return allocator->callbacks->ffreeall (allocator);
}

void *
ws4d_allocator_alloc (struct ws4d_abs_allocator *allocator, size_t length)
{
  ws4d_assert (allocator && allocator->callbacks
               && allocator->callbacks->falloc, NULL);

  return allocator->callbacks->falloc (allocator, length);
}

int
ws4d_allocator_free (struct ws4d_abs_allocator *allocator, void *buf)
{
  ws4d_assert (allocator && allocator->callbacks
               && allocator->callbacks->ffree, WS4D_ERR);

  return allocator->callbacks->ffree (allocator, buf);
}

void *
ws4d_allocator_memdup (struct ws4d_abs_allocator *allocator, const void *src,
                       size_t length)
{
  ws4d_assert (allocator && allocator->callbacks
               && allocator->callbacks->fmemdup, NULL);

  return allocator->callbacks->fmemdup (allocator, src, length);
}

void *
ws4d_allocator_memref (struct ws4d_abs_allocator *allocator, const void *src,
                       size_t length)
{
  ws4d_assert (allocator && allocator->callbacks
               && allocator->callbacks->fmemref, NULL);

  return allocator->callbacks->fmemref (allocator, src, length);
}

int
ws4d_allocator_unref (struct ws4d_abs_allocator *allocator, void *src)
{
  ws4d_assert (allocator && allocator->callbacks
               && allocator->callbacks->funref, WS4D_ERR);

  return allocator->callbacks->funref (allocator, src);
}

/**
 * an element of an allocation list
 *
 */

typedef struct soap_alloc_list_item_s
{
  struct ws4d_list_node list;
  ws4d_alloc_list *alist;
  size_t size;
  void *start;
} soap_alloc_list_item;

void *
ws4d_malloc_alist (size_t length, ws4d_alloc_list * alist)
{
  register soap_alloc_list_item *p = NULL;

  if (alist)
    {
      ws4d_mutex_lock (&alist->lock);
      p = (soap_alloc_list_item *)
        ws4d_malloc (length + offsetof (soap_alloc_list_item, start));

      if (p)
        {
          p->size = length + offsetof (soap_alloc_list_item, start);
          p->alist = alist;
          ws4d_list_add_tail (&p->list, &alist->alist);
          ws4d_mutex_unlock (&alist->lock);
          return &p->start;
        }
      ws4d_mutex_unlock (&alist->lock);
    }
  return NULL;
}

void
ws4d_alloclist_done (ws4d_alloc_list * alist)
{
  register soap_alloc_list_item *item, *next;
  if (alist)
    {
      ws4d_mutex_lock (&alist->lock);
      ws4d_list_foreach (item, next, &alist->alist,
                         soap_alloc_list_item, list)
      {
        ws4d_list_del (&item->list);
        memset (item, 0, item->size);
        ws4d_free (item);
      }
      ws4d_mutex_unlock (&alist->lock);
    }
}


void
ws4d_free_alist (void *p)
{
  soap_alloc_list_item *entry;

  if (p)
    {
      entry = ws4d_container_of (p, soap_alloc_list_item, start);
      if (entry && entry->alist)
        {
#ifdef WITH_MUTEXES
          ws4d_alloc_list *alist = entry->alist;
#endif
          ws4d_mutex_lock (&alist->lock);
          ws4d_list_del (&entry->list);
          memset (entry, 0, entry->size);
          ws4d_free (entry);
          ws4d_mutex_unlock (&alist->lock);
        }
    }
}

void *
ws4d_memdup (const void *src, size_t length, ws4d_alloc_list * alist)
{
  void *res = NULL;

  if (!(res = ws4d_malloc_alist (length, alist)))
    return res;

  return memcpy (res, src, length);
}

char *
ws4d_strdup (const char *src, ws4d_alloc_list * alist)
{
  char *res = NULL;
  size_t len;

  if (!src)
    return res;

  len = strlen (src);
  res = (char *) ws4d_memdup (src, len + 1, alist);
  res[len] = '\0';

  return res;
}

wchar_t *
ws4d_widedup (const wchar_t *src, ws4d_alloc_list * alist)
{
  wchar_t *res = NULL;
  size_t len;

  if (!src)
    return res;

  for(len=0;src[len]!=0; len++);

  res = (wchar_t *) ws4d_memdup (src, (len + 1)*sizeof(wchar_t), alist);

  res[len] = '\0';

  return res;
}

char *
ws4d_strndup (const char *src, size_t size, ws4d_alloc_list * alist)
{
  char *res = NULL;
  size_t len;

  if (!src)
    return res;

  len = strlen (src);
  if (len > size)
    {
      len = size;
    }
  res = (char *) ws4d_memdup (src, len + 1, alist);
  res[len] = '\0';

  return res;
}
