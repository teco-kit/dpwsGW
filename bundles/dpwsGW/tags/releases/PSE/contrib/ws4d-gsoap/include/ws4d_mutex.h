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

#ifndef WS4D_MUTEX_H_
#define WS4D_MUTEX_H_

/**
 * Mechanisms for mutual exclusion
 *
 * This is an abstraction layer for platform dependent locking if the
 * the stack is compiled with multithreading support. The logic behind
 * these macros should be implemented for each platform. Up to now only
 * support for the pthreads api is implemented.
 *
 * @addtogroup DpwsMutexes Locking for multithreading
 * @ingroup WS4D_UTILS
 *
 * @{
 */

#ifdef WITH_MUTEXES

#ifdef WIN32
#define WS4D_MUTEX(x) HANDLE x
#else
#include <pthread.h>
#define WS4D_MUTEX(x) pthread_mutex_t x
#endif

void ws4d_mutex_init (WS4D_MUTEX (*mutex));
void ws4d_mutex_destroy (WS4D_MUTEX (*mutex));
void ws4d_mutex_lock (WS4D_MUTEX (*mutex));
void ws4d_mutex_unlock (WS4D_MUTEX (*mutex));

#else

/**
 * Declares a mutex
 *
 * A mutex should bei declared as:
 * @code
 * WS4D_MUTEX(mutex);
 * @endcode
 * or in a structure:
 * @code
 * struct mystruct {
 *   [...]
 *   WS4D_MUTEX(mutex);
 *   [...]
 * };
 * @endcode
 *
 * @param x variable to declare as mutex
 */
#define WS4D_MUTEX(x)

/**
 * Initializes a mutex at runtime
 *
 * A mutex can be initialized at run time with:
 * @code
 * WS4D_MUTEX(mutex);
 *
 * ws4d_mutex_init(&mutex);
 * @endcode
 *
 * @param x pointer to mutex to initialize
 */
#define ws4d_mutex_init(x)

/**
 * Destroys a mutex at runtime
 *
 * A mutex can be destroyed at run time with:
 * @code
 * WS4D_MUTEX(mutex);
 *
 * ws4d_mutex_init(&mutex);
 *
 * ws4d_mutex_destroy(&mutex);
 * @endcode
 *
 * @param x pointer to mutex to destroy
 */
#define ws4d_mutex_destroy(x)

/**
 * Locks a mutex
 *
 * A mutex can be locked at run time with:
 * @code
 * WS4D_MUTEX(mutex);
 *
 * ws4d_mutex_init(&mutex);
 *
 * ws4d_mutex_lock(&mutex);
 * @endcode
 *
 * @param x pointer to mutex to lock
 */
#define ws4d_mutex_lock(x)

/**
 * Unlocks a mutex
 *
 * A mutex can be unlocked at run time with:
 * @code
 * WS4D_MUTEX(mutex);
 *
 * ws4d_mutex_init(&mutex);
 * ws4d_mutex_lock(&mutex);
 *
 * ws4d_mutex_unlock(&mutex);
 * @endcode
 *
 * @param x pointer to mutex to unlock
 */
#define ws4d_mutex_unlock(x)
#endif
/** @} */

#endif /*WS4D_MUTEX_H_ */
