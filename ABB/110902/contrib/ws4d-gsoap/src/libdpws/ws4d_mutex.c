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

#ifdef WITH_MUTEXES
void
ws4d_mutex_init (WS4D_MUTEX (*mutex))
{
#ifdef WIN32
  if (mutex)
    *mutex = CreateMutex (NULL, FALSE, NULL);
#else
  pthread_mutex_init (mutex, NULL);
#endif
}

void
ws4d_mutex_destroy (WS4D_MUTEX (*mutex))
{
#ifdef WIN32
  if (mutex)
    CloseHandle (*mutex);
#else
  pthread_mutex_destroy (mutex);
#endif
}

void
ws4d_mutex_lock (WS4D_MUTEX (*mutex))
{
#ifdef WIN32
  if (mutex)
    WaitForSingleObject (*mutex, INFINITE);
#else
  pthread_mutex_lock (mutex);
#endif
}

void
ws4d_mutex_unlock (WS4D_MUTEX (*mutex))
{
#ifdef WIN32
  if (mutex)
    ReleaseMutex (*mutex);
#else
  pthread_mutex_unlock (mutex);
#endif
}
#endif
