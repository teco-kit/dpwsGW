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

#ifndef WS4D_QNAME_H_
#define WS4D_QNAME_H_

/**
 * QName
 *
 * @addtogroup APIQName QName
 * @ingroup WS4D_UTILS
 *
 * @{
 */

struct ws4d_qname
{
  struct ws4d_list_node list;
  char *prefix;
  char *name;
  char *ns;
};

#define ws4d_qname_getName(qname) (qname)->name
#define ws4d_qname_getPrefix(qname) (qname)->prefix
#define ws4d_qname_getNS(qname) (qname)->ns

/**
 * TODO: add documentation
 */
struct ws4d_qname *ws4d_qname_alloc (int count, ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
void ws4d_qname_free (struct ws4d_qname *qname);

/**
 * TODO: add documentation
 */
char *ws4d_qname_tostring2 (struct ws4d_qname *qname, char *buffer,
                            size_t size);

/**
 * TODO: add documentation
 */
struct ws4d_qname *ws4d_qname_dup (struct ws4d_qname *src,
                                   ws4d_alloc_list * alist);

/**
 * QName list
 *
 * @addtogroup APIQNameList QName list
 * @ingroup APIQName
 *
 * @{
 */

typedef struct ws4d_list_node ws4d_qnamelist;

/**
 * Function to initialize a QNameList head
 *
 * @param head list head to initialize
 * @return SOAP_OK on success SOAP_ERR otherwise
 */
int ws4d_qnamelist_init (ws4d_qnamelist * head);


/**
 * Function to reset QNameList head
 *
 * @param head head of list to reset
 * @return SOAP_OK on success SOAP_ERR otherwise
 */
int ws4d_qnamelist_done (ws4d_qnamelist * head);

/**
 * TODO: add documentation
 */
int ws4d_qnamelist_add (struct ws4d_qname *qname, ws4d_qnamelist * head);

/**
 * TODO: add documentation
 */
int
ws4d_qnamelist_addstring (const char *string, ws4d_qnamelist * head,
                          ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
int
ws4d_qnamelist_copy (ws4d_qnamelist * src, ws4d_qnamelist * dst,
                     ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
int ws4d_qnamelist_remove (struct ws4d_qname *qname);

/**
 * TODO: add documentation
 */
int ws4d_qnamelist_clear (ws4d_qnamelist * head);

/**
 * TODO: add documentation
 */
int ws4d_qnamelist_empty (ws4d_qnamelist * head);

/**
 * TODO: add documentation
 */
char *ws4d_qnamelist_tostring (ws4d_qnamelist * head,
                               ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
char **ws4d_qnamelist_toarray (ws4d_qnamelist * head,
                               ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
#define ws4d_qnamelist_foreach(pos, iter, head) ws4d_list_foreach (pos, iter, head, struct ws4d_qname, list)


/** @} */

/** @} */

#endif /*WS4D_QNAME_H_ */
