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
 *  Created on: 18.01.2009
 *      Author: Elmar Zeeb
 */

#include "ws4d_misc.h"
#include "ws4d_scopelist.h"

int
ws4d_sl_init (struct ws4d_list_node *scope_list)
{
  WS4D_INIT_LIST (scope_list);

  return WS4D_OK;
}

int
ws4d_sl_done (struct ws4d_list_node *scope_list)
{
  register struct ws4d_scope *cur = NULL, *next = NULL;

  ws4d_assert (scope_list, WS4D_ERR);

  ws4d_list_foreach (cur, next, scope_list, struct ws4d_scope, list)
  {
    ws4d_list_del (&cur->list);
    ws4d_free_alist ((void *) cur->scope);
    ws4d_free_alist (cur);
  }

  return WS4D_OK;
}

struct ws4d_scope *
ws4d_sl_get (struct ws4d_list_node *scope_list, const char *scope)
{
  register struct ws4d_scope *cur = NULL, *next = NULL;

  ws4d_assert (scope_list && scope, NULL);

  ws4d_list_foreach (cur, next, scope_list, struct ws4d_scope, list)
  {
    if (!strcmp (cur->scope, scope))
      {
        return cur;
      }
  }

  /* not found */
  return NULL;
}

int
ws4d_sl_add (struct ws4d_list_node *scope_list, const char *scope,
             ws4d_alloc_list * alist)
{
  struct ws4d_scope *new_scope;

  ws4d_assert (scope_list && scope && alist, WS4D_ERR);

  /* is scope already registered? */
  if (ws4d_sl_get (scope_list, scope) != NULL)
    {
      return WS4D_ERR;
    }

  /* allocate memory */
  new_scope = ws4d_malloc_alist (sizeof (struct ws4d_scope), alist);
  if (new_scope == NULL)
    {
      return WS4D_EOM;
    }
  /* assign key by reference !!! */
  new_scope->scope = ws4d_strdup (scope, alist);

  /* add key to list */
  ws4d_list_add (&new_scope->list, scope_list);

  return WS4D_OK;
}

int
ws4d_sl_del (struct ws4d_list_node *scope_list, const char *scope)
{
  struct ws4d_scope *del_scope;

  ws4d_assert (scope_list && scope, WS4D_ERR);

  /* find scope to free */
  del_scope = ws4d_sl_get (scope_list, scope);
  if (del_scope == NULL)
    {
      return WS4D_ERR;
    }

  ws4d_list_del (&del_scope->list);

  ws4d_free_alist ((void *) del_scope->scope);
  ws4d_free_alist (del_scope);

  return WS4D_OK;
}

int
ws4d_sl_dup (struct ws4d_list_node *src, struct ws4d_list_node *dst,
             ws4d_alloc_list * alist)
{
  register struct ws4d_scope *cur = NULL, *next = NULL;

  ws4d_assert (src && dst && alist, WS4D_ERR);

  ws4d_sl_done (dst);

  ws4d_list_foreach (cur, next, src, struct ws4d_scope, list)
  {
    ws4d_sl_add (dst, cur->scope, alist);
  }

  return WS4D_OK;
}

char *
ws4d_sl_tostr (struct ws4d_list_node *scope_list, ws4d_alloc_list * alist)
{
  int len = 0, first = 1;
  char *result;
  register struct ws4d_scope *cur = NULL, *next = NULL;

  ws4d_assert (scope_list && alist, NULL);

  /* calculate string lenght */
  ws4d_list_foreach (cur, next, scope_list, struct ws4d_scope, list)
  {
    len += strlen (cur->scope);
    len += 1;
  }

  result = ws4d_malloc_alist (len, alist);
  if (!result)
    return NULL;

  /* concat string */
  ws4d_list_foreach (cur, next, scope_list, struct ws4d_scope, list)
  {
    if (first)
      {
        first = 0;
      }
    else
      {
        strcat (result, " ");
      }
    strcat (result, cur->scope);
  }

  return result;
}

int
ws4d_sl_isempty (struct ws4d_list_node *scope_list)
{
  ws4d_assert (scope_list, 1);

  return ws4d_list_empty (scope_list);
}
