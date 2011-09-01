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

#ifndef _ABSTRACT_EPRLIST_H_
#define _ABSTRACT_EPRLIST_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#include "ws4d_misc.h"
#include "ws4d_epr.h"

struct ws4d_abs_eprlist;

struct ws4d_abs_eprlist_cb
{
  void (*fdone) (struct ws4d_abs_eprlist * list);

  struct ws4d_epr *(*falloc) (struct ws4d_abs_eprlist * list);

  int (*ffree) (struct ws4d_abs_eprlist * list, struct ws4d_epr * epr);

  int (*fadd) (struct ws4d_abs_eprlist * list, struct ws4d_epr * epr);

  int (*fremove) (struct ws4d_abs_eprlist * list, struct ws4d_epr * epr);

  int (*fclear) (struct ws4d_abs_eprlist * list);

  int (*fisempty) (struct ws4d_abs_eprlist * list);

  struct ws4d_epr *(*fget_first) (struct ws4d_abs_eprlist * list);

  struct ws4d_epr *(*fget_next) (struct ws4d_abs_eprlist * list,
                                 struct ws4d_epr * epr);

  struct ws4d_epr *(*fget_byAddr) (struct ws4d_abs_eprlist * list,
                                   const char *Addr);

  void (*flock) (struct ws4d_abs_eprlist * list);

  void (*funlock) (struct ws4d_abs_eprlist * list);
};

struct ws4d_abs_eprlist
{
  const char *id;
  void *data;
  struct ws4d_abs_eprlist_cb *callbacks;
};

#ifndef ABSTRACT_EPRLIST_IMPLEMENT

/**
 * Abstract Endpoint Reference Lists
 *
 * @addtogroup APIEndpointLists Abstract Endpoint Reference Lists
 * @ingroup WS4D_UTILS
 *
 * @{
 */

/**
 * Function to initialize an Enpoint Reference List head
 *
 * @code
 * ws4d_abs_eprlist head;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 * @endcode
 *
 * @param head head of eprlist
 * @param finit function to create eprlist
 * @param arg argumets to finit function
 *
 * @return WS4D_OK on success, error code otherwise
 */
static INLINE int
ws4d_eprlist_init (struct ws4d_abs_eprlist *head,
                   int (*finit) (struct ws4d_abs_eprlist *
                                 head, void *arg), void *arg)
{
  int ret = 0;

  ws4d_assert (head && finit, WS4D_ERR);

  memset (head, 0, sizeof (struct ws4d_abs_eprlist));
  ret = finit (head, arg);
  if (ret != WS4D_OK)
    {
      return ret;
    }

  ws4d_assert (head->callbacks
               && head->callbacks->fdone
               && head->callbacks->falloc
               && head->callbacks->ffree
               && head->callbacks->fadd
               && head->callbacks->fremove
               && head->callbacks->fclear
               && head->callbacks->fisempty
               && head->callbacks->fget_first
               && head->callbacks->fget_next
               && head->callbacks->fget_byAddr, WS4D_ERR);

  return WS4D_OK;
}

/**
 * Function to destroy abstract eprlist and free allocated memory
 *
 * @code
 * ws4d_abs_eprlist head;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 */
static INLINE void
ws4d_eprlist_done (struct ws4d_abs_eprlist *head)
{
  if (!head || !head->callbacks)
    {
      return;
    }

  if (head->callbacks->fdone)
    {
      head->callbacks->fdone (head);
    }

  memset (head, 0, sizeof (struct ws4d_abs_eprlist));
}

/**
 * function to allocate a new element within an eprlist
 *
 * @code
 * ws4d_abs_eprlist head;
 * ws4d_epr *epr;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 * epr = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 *
 * @return pointer to allocated epr structure or NULL on failure
 */
static INLINE struct ws4d_epr *
ws4d_eprlist_alloc (struct ws4d_abs_eprlist *head)
{
  ws4d_assert (head && head->callbacks && head->callbacks->falloc, NULL);

  return head->callbacks->falloc (head);
}


/**
 * function to free memory of an eprlist element
 *
 * @code
 * ws4d_abs_eprlist head;
 * ws4d_epr *epr;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * epr = ws4d_eprlist_alloc(&head);
 *
 * ws4d_eprlist_free(&head, epr);
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 * @param epr epr element to free
 *
 * @return WS4D_OK on success or error code otherwise
 */
static INLINE int
ws4d_eprlist_free (struct ws4d_abs_eprlist *head, struct ws4d_epr *epr)
{
  ws4d_assert (head && head->callbacks && head->callbacks->ffree
               && epr, WS4D_ERR);

  return head->callbacks->ffree (head, epr);
}

/**
 * Function to add an epr to an eprlist
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * epr = ws4d_eprlist_alloc(&head);
 *
 * ws4d_eprlist_add(&head, epr);
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 * @param epr Endpoint Reference to add
 *
 * @return WS4D_OK on success WS4D_ERR otherwise
 */
static INLINE int
ws4d_eprlist_add (struct ws4d_abs_eprlist *head, struct ws4d_epr *epr)
{
  ws4d_assert (head && head->callbacks && head->callbacks->fadd
               && epr, WS4D_ERR);

  return head->callbacks->fadd (head, epr);
}

/**
 * Function to remove an Endpoint Reference from an Endpoint Reference List
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 * epr = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_add(&head, epr);
 *
 * ws4d_eprlist_remove(&head, epr);
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 * @param epr Endpoint Reference to remove
 * @return WS4D_OK on success WS4D_ERR otherwise
 */
static INLINE int
ws4d_eprlist_remove (struct ws4d_abs_eprlist *head, struct ws4d_epr *epr)
{
  ws4d_assert (head && head->callbacks && head->callbacks->fremove
               && epr, WS4D_ERR);

  return head->callbacks->fremove (head, epr);
}

/**
 * Function to check if an eprlist is empty
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * if (ws4d_eprlist_isempty(&head))
 * {
 *   //eprlist is empty
 * }
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 */
static INLINE int
ws4d_eprlist_isempty (struct ws4d_abs_eprlist *head)
{
  ws4d_assert (head && head->callbacks
               && head->callbacks->fisempty, WS4D_ERR);

  return head->callbacks->fisempty (head);
}

/**
 * Function to clear an eprlist
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * epr = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_add(&head, epr);
 *
 * if (!ws4d_eprlist_isempty(&head))
 * {
 *   //eprlist is not empty
 * }
 *
 * ws4d_eprlist_clear(&head, epr);
 *
 * if (ws4d_eprlist_isempty(&head))
 * {
 *   //eprlist is empty
 * }
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 *
 * @return WS4D_OK on success, error code otherwise
 */
static INLINE int
ws4d_eprlist_clear (struct ws4d_abs_eprlist *head)
{
  ws4d_assert (head && head->callbacks && head->callbacks->fclear, WS4D_ERR);

  return head->callbacks->fclear (head);
}

/**
 * Function to get first element of an eprlist
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr, *first;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * epr = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_add(&head, epr);
 *
 * first = ws4d_eprlist_get_first(&head);
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 *
 * @return pointer to first element of eprlist, NULL on failure
 */
static INLINE struct ws4d_epr *
ws4d_eprlist_get_first (struct ws4d_abs_eprlist *head)
{
  ws4d_assert (head && head->callbacks && head->callbacks->fget_first, NULL);

  return head->callbacks->fget_first (head);
}

/**
 * Function to get the next element of an eprlist after a specific epr
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr1, *epr2, *first, *next;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * epr1 = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_add(&head, epr1);
 *
 * epr2 = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_add(&head, epr2);
 *
 * first = ws4d_eprlist_get_first(&head);
 * next = ws4d_eprlist_get_next(&head, first);
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 * @param epr element of eprlist
 *
 * @return pointer to epr after the on specified or NULL on failure
 */
static INLINE struct ws4d_epr *
ws4d_eprlist_get_next (struct ws4d_abs_eprlist *head, struct ws4d_epr *epr)
{
  ws4d_assert (head && head->callbacks && head->callbacks->fget_next, NULL);

  return head->callbacks->fget_next (head, epr);
}


/**
 * Function to search for an epr by address
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr, *found;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * epr = ws4d_eprlist_alloc(&head);
 * ws4d_epr_set_Addrs(epr, "urn:uuid:[uuid]");
 * ws4d_eprlist_add(&head, epr);
 *
 * found = ws4d_eprlist_get_byAddr(&head, "urn:uuid:[uuid]");
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 *
 * @param head head of eprlist
 * @param Addr address of epr
 *
 * @return pointer to epr with specified address or NULL on otherwise
 */
static INLINE struct ws4d_epr *
ws4d_eprlist_get_byAddr (struct ws4d_abs_eprlist *head, const char *Addr)
{
  ws4d_assert (head && head->callbacks && head->callbacks->fget_byAddr
               && Addr, NULL);

  return head->callbacks->fget_byAddr (head, Addr);
}

/**
 * Macro to iterate over all eprs in an eprlist
 *
 * @code
 * ws4d_abs_eprlist head;
 * struct ws4d_epr *epr1, *epr2, *epr, *iter;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * epr1 = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_add(&head, epr1);
 *
 * epr2 = ws4d_eprlist_alloc(&head);
 * ws4d_eprlist_add(&head, epr2);
 *
 * ws4d_eprlist_foreach(epr, iter, &head)
 * {
 *    //do something with epr
 * }
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 */
#define ws4d_eprlist_foreach(epr, iter, list) \
for ( epr = ws4d_eprlist_get_first(list), iter = ws4d_eprlist_get_next(list, epr); \
    epr; \
    epr = iter, iter = ws4d_eprlist_get_next(list, iter))

#ifdef WITH_MUTEXES
static INLINE void
ws4d_eprlist_lock (struct ws4d_abs_eprlist *head)
{
  ws4d_assert (head && head->callbacks && head->callbacks->flock,);

  head->callbacks->flock (head);
}

static INLINE void
ws4d_eprlist_unlock (struct ws4d_abs_eprlist *head)
{
  ws4d_assert (head && head->callbacks && head->callbacks->funlock,);

  head->callbacks->funlock (head);
}
#else

/**
 * Macro to lock an eprlist for mutual exclusion
 *
 * @code
 * ws4d_abs_eprlist head;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * ws4d_eprlist_lock(&head);
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 */
#define ws4d_eprlist_lock(head)

/**
 * Macro to unlock an eprlist for mutual exclusion
 *
 * @code
 * ws4d_abs_eprlist head;
 *
 * ws4d_eprlist_init(&head, ws4d_eprllist_init, NULL);
 *
 * ws4d_eprlist_lock(&head);
 * ws4d_eprlist_unlock(&head);
 *
 * ws4d_eprlist_done(&head);
 * @endcode
 */
#define ws4d_eprlist_unlock(head)
#endif

/** @} */

/**
 * Endpoint Reference List Implementations
 *
 * @addtogroup APIEndpointListImplementation Implementations
 * @ingroup APIEndpointLists
 *
 * @{
 */

/** @} */

#endif /*ABSTRACT_EPRLIST_IMPLEMENT */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*_ABSTRACT_EPRLIST_H_*/
