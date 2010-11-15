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

#ifndef WS4D_ALLOCLIST_H_
#define WS4D_ALLOCLIST_H_

#include "ws4d_mutex.h"
#include "ws4d_list.h"


/**
 * @addtogroup DpwsMemoryManagement Memory management
 * @ingroup WS4D_UTILS
 *
 * @{
 */


/**
 * Wraps the platform malloc() call
 *
 * @code
 * void *buf = NULL;
 *
 * buf = WS4D_MALLOC(100);
 * @endcode
 *
 * @param size size of buffer
 *
 * @return newly-allocated buffer or NULL if no memory
 */
#define WS4D_MALLOC(size) malloc(size)


/**
 * Wraps the platform free() call
 *
 * @code
 * void *buf = NULL;
 *
 * buf = WS4D_MALLOC(100);
 * WS4D_FREE(buf);
 * @endcode
 *
 * @param ptr pointer to buffer to free
 */
#define WS4D_FREE(ptr) free(ptr)


/**
 * Structure to manage allocation lists
 *
 */

typedef struct ws4d_alloc_list_s
{
  struct ws4d_list_node alist;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
} ws4d_alloc_list;


/**
 * Macro to initialize an allocation list at run time
 *
 * @code
 * ws4d_alloc_list alloclist;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * @endcode
 *
 * @param head of allocation list
 */
#define WS4D_ALLOCLIST_INIT(head) do { \
    WS4D_INIT_LIST(&(head)->alist); \
    ws4d_mutex_init(&(head)->lock); \
  } while (0)


/**
 * Function allocates and initializes memory with NULL.
 *
 * @code
 * void *buf = NULL;
 *
 * buf = ws4d_malloc(100);
 * @endcode
 *
 * @param length length of the buffer
 *
 * @return pointer to newly-allocated buffer or NULL if no memory
 */
void *ws4d_malloc (size_t length);


/**
 * Function frees memory allocated with ws4d_malloc.
 *
 * @code
 * void *buf = NULL;
 *
 * buf = ws4d_malloc(100);
 * ws4d_free(buf);
 * @endcode
 *
 * @param p pointer to buffer
 */
void ws4d_free (void *p);


/**
 * Function allocates memory and registers allocation in an allocation list.
 *
 * If allocation list is ommited this function behaves like ws4d_malloc()
 *
 * @code
 * ws4d_alloc_list alloclist;
 * void *buf = NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * buf = ws4d_malloc_alist(100, &alloclist);
 * @endcode
 *
 * @param length length of the buffer
 * @param alist allocation list to insert allocation
 *
 * @return pointer to newly-allocated buffer or NULL if no memory
 */
void *ws4d_malloc_alist (size_t length, ws4d_alloc_list * alist);


/**
 * Function frees a allocation that is listed in an allocation list.
 *
 * @code
 * ws4d_alloc_list alloclist;
 * void *buf = NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * buf = ws4d_malloc_alist(100, &alloclist);
 * ws4d_free_alist(buf);
 * @endcode
 *
 * @param p pointer to allocation buffer to free and remove from
 * allocation list
 */
void ws4d_free_alist (void *p);


/**
 * Function frees all memory that is listed in an allocation list.
 *
 * @code
 * ws4d_alloc_list alloclist;
 * void *buf1 = NULL, *buf2 = NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * buf1 = ws4d_malloc_alist(100, &alloclist);
 * buf2 = ws4d_malloc_alist(300, &alloclist);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param alist allocation list to free
 */
void ws4d_alloclist_done (ws4d_alloc_list * alist);


/**
 * Function duplicates buffer and registers allocation in allocation list.
 *
 * @code
 * ws4d_alloc_list alloclist;
 * void *buf1 = NULL, *buf2 = NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * buf1 = ws4d_malloc_alist(100, &alloclist);
 * memset(buf, 13, 100);
 *
 * buf2 = ws4d_memdup(buf1, 100, &alloclist);
 * @endcode
 *
 * @param src pointer to buffer to duplicate
 * @param length length of the buffer
 * @param alist allocation list to insert allocation
 *
 * @return pointer to newly-allocated buffer or NULL if no memory
 */
void *ws4d_memdup (const void *src, size_t length, ws4d_alloc_list * alist);


/**
 * Function duplicates string and registers allocation in allocation list.
 *
 * @code
 * ws4d_alloc_list alloclist;
 * char *string1 = "Hello", *string2 = NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 *
 * string2 = ws4d_strdup(string1, &alloclist);
 * @endcode
 *
 * @param src pointer to string to duplicate
 * @param alist allocation list to insert allocation
 *
 * @return pointer to newly-allocated string or NULL if no memory
 */
char *ws4d_strdup (const char *src, ws4d_alloc_list * alist);

wchar_t *ws4d_widedup (const wchar_t *src, ws4d_alloc_list * alist);

/**
 * Function duplicates string and registers allocation in allocation list.
 *
 * @code
 * ws4d_alloc_list alloclist;
 * char *string1 = "Hello", *string2 = NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 *
 * string2 = ws4d_strndup(string1, &alloclist);
 * @endcode
 *
 * @param src pointer to string to duplicate
 * @param size size of string
 * @param alist allocation list to insert allocation
 *
 * @return pointer to newly-allocated string or NULL if no memory
 */
char *ws4d_strndup (const char *src, size_t size, ws4d_alloc_list * alist);


/** @} */

#endif /*WS4D_ALLOCLIST_H_ */
