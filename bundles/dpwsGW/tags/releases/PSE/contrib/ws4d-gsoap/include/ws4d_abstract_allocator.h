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

#ifndef _ABSTRACT_ALLOCATOR_H_
#define _ABSTRACT_ALLOCATOR_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#include "ws4d_misc.h"

struct ws4d_abs_allocator;

struct ws4d_abs_allocator_cbs
{
  void (*fdone) (struct ws4d_abs_allocator * allocator);

  int (*ffreeall) (struct ws4d_abs_allocator * allocator);

  void *(*falloc) (struct ws4d_abs_allocator * allocator, size_t size);

  int (*ffree) (struct ws4d_abs_allocator * allocator, void *buf);

  void *(*fmemdup) (struct ws4d_abs_allocator * allocator, const void *buf,
                    size_t size);

  void *(*fmemref) (struct ws4d_abs_allocator * allocator, const void *buf,
                    size_t size);

  int (*funref) (struct ws4d_abs_allocator * allocator, void *buf);
};

struct ws4d_abs_allocator
{
  const char *id;
  void *data;
  struct ws4d_abs_allocator_cbs *callbacks;
};

#ifndef ABSTRACT_ALLOCATOR_IMPLEMENT

/**
 * Abstract Allocator
 *
 * @addtogroup APIAbsAllocator Abstract Allocator
 * @ingroup WS4D_UTILS
 *
 * @{
 */

#define WS4D_ALLOCATOR_DECL(name) struct ws4d_abs_allocator name
#define WS4D_ALLOCATOR_INIT(name, finit, arg) \
    ws4d_allocator_init(name, finit, arg)

/**
 * Function to initialize an abstract allocator
 *
 * @code
 * ws4d_abs_allocator alloc;
 *
 * ws4d_allocator_init(&alloc, ws4d_alist_init, NULL);
 * @endcode
 *
 * @param allocator allocator
 * @param finit function to create allocator
 * @param arg argumets to finit function
 *
 * @return WS4D_OK on success, error code otherwise
 */
int
ws4d_allocator_init (struct ws4d_abs_allocator *allocator,
                     int (*finit) (struct ws4d_abs_allocator *
                                   allocator, void *arg), void *arg);

/**
 * Function to destroy allocator and free allocated memory
 *
 * @code
 * ws4d_abs_allocator alloc;
 *
 * ws4d_eprlist_init(&alloc, ws4d_alist_init, NULL);
 * ws4d_eprlist_done(&alloc);
 * @endcode
 *
 * @param allocator allocator
 */
void ws4d_allocator_done (struct ws4d_abs_allocator *allocator);

/**
 * function to free all memory of an allocator
 *
 * @code
 * ws4d_abs_allocator alloc;
 * void *buf;
 *
 * ws4d_eprlist_init(&alloc, ws4d_alist_init, NULL);
 *
 * ws4d_allocator_freeall(&alloc);
 *
 * ws4d_eprlist_done(&alloc);
 * @endcode
 *
 * @param allocator allocator
 *
 * @return WS4D_OK on success or error code otherwise
 */
int ws4d_allocator_freeall (struct ws4d_abs_allocator *allocator);

/**
 * function to allocate a memory with an allocator
 *
 * @code
 * ws4d_abs_allocator alloc;
 * void *buf;
 *
 * ws4d_eprlist_init(&alloc, ws4d_alist_init, NULL);
 *
 * buf = ws4d_allocator_alloc(&alloc, 20);
 *
 * ws4d_eprlist_done(&alloc);
 * @endcode
 *
 * @param allocator allocator
 * @param length length of buffer to allocate
 *
 * @return pointer to allocated memory or NULL on failure
 */
void *ws4d_allocator_alloc (struct ws4d_abs_allocator *allocator,
                            size_t length);

/**
 * function to free memory of an allocator
 *
 * @code
 * ws4d_abs_allocator alloc;
 * void *buf;
 *
 * ws4d_eprlist_init(&alloc, ws4d_alist_init, NULL);
 *
 * buf = ws4d_allocator_alloc(&alloc, 20);
 * ws4d_allocator_free(&alloc, buf);
 *
 * ws4d_eprlist_done(&alloc);
 * @endcode
 *
 * @param allocator allocator
 * @param buf memory to free
 *
 * @return WS4D_OK on success or error code otherwise
 */
int ws4d_allocator_free (struct ws4d_abs_allocator *allocator, void *buf);

/**
 * function to duplicate memory with an allocator
 *
 * @code
 * ws4d_abs_allocator alloc;
 * void *buf, *buf2;
 *
 * ws4d_allocator_init(&alloc, ws4d_alist_init, NULL);
 *
 * buf = ws4d_allocator_alloc(&alloc, 20);
 *
 * buf2 = ws4d_allocator_dup(&alloc, buf, 20);
 *
 * ws4d_eprlist_done(&alloc);
 * @endcode
 *
 * @param allocator allocator
 * @param src pointer to memory to duplicate
 * @param length lenght of memory buffer to duplicate
 *
 * @return pointer to allocated memory or NULL on failure
 */
void *ws4d_allocator_memdup (struct ws4d_abs_allocator *allocator,
                             const void *src, size_t length);

/**
 * function to reference memory allocated with an allocator
 *
 * @code
 * ws4d_abs_allocator alloc;
 * void *buf, *buf2;
 *
 * ws4d_allocator_init(&alloc, ws4d_alist_init, NULL);
 *
 * buf = ws4d_allocator_alloc(&alloc, 20);
 *
 * buf2 = ws4d_allocator_memref(&alloc, buf);
 *
 * ws4d_eprlist_done(&alloc);
 * @endcode
 *
 * @param allocator allocator
 * @param src pointer to memory to reference
 * @return WS4D_OK on success WS4D_ERR otherwise
 */
void *ws4d_allocator_memref (struct ws4d_abs_allocator *allocator,
                             const void *src, size_t length);

/**
 * function to free a reference allocated with an allocator
 *
 * @code
 * ws4d_abs_allocator alloc;
 * void *buf, *buf2;
 *
 * ws4d_allocator_init(&alloc, ws4d_alist_init, NULL);
 *
 * buf = ws4d_allocator_alloc(&alloc, 20);
 *
 * buf2 = ws4d_allocator_memref(&alloc, buf);
 * ws4d_allocator_unref(&alloc, buf);
 *
 * ws4d_eprlist_done(&alloc);
 * @endcode
 *
 * @param allocator allocator
 * @param src pointer to memory to reference
 * @return WS4D_OK on success WS4D_ERR otherwise
 */
int ws4d_allocator_unref (struct ws4d_abs_allocator *allocator, void *src);
/** @} */

/**
 * Allocator Implementations
 *
 * @addtogroup APIAbsAllocatorImplementations Implementations
 * @ingroup APIAbsAllocator
 *
 * @{
 */

/** @} */

#endif /*ABSTRACT_ALLOCATOR_IMPLEMENT */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*_ABSTRACT_ALLOCATOR_H_*/
