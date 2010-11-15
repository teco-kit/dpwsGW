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

#ifndef WS4D_SCOPELIST_H_
#define WS4D_SCOPELIST_H_

/**
 * Scope List
 *
 * @addtogroup APIScopeList ScopeList
 * @ingroup WS4D_UTILS
 *
 * @{
 */

struct ws4d_scope
{
  struct ws4d_list_node list;
  const char *scope;
};

/**
 * TODO: add documentation
 */
int ws4d_sl_init (struct ws4d_list_node *scope_list);

/**
 * TODO: add documentation
 */
int ws4d_sl_add (struct ws4d_list_node *scope_list, const char *scope,
                 ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
int ws4d_sl_del (struct ws4d_list_node *scope_list, const char *scope);

/**
 * TODO: add documentation
 */
int ws4d_sl_done (struct ws4d_list_node *scope_list);

/**
 * TODO: add documentation
 */
struct ws4d_scope *ws4d_sl_get (struct ws4d_list_node *scope_list,
                                const char *scope);

/**
 * TODO: add documentation
 */
int ws4d_sl_dup (struct ws4d_list_node *src, struct ws4d_list_node *dst,
                 ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
char *ws4d_sl_tostr (struct ws4d_list_node *scope_list,
                     ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
int ws4d_sl_isempty (struct ws4d_list_node *scope_list);

/** @} */

#endif /* WS4D_SCOPELIST_H_ */
