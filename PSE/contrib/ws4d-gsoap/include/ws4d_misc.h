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

#ifndef WS4D_MISC_H_
#define WS4D_MISC_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#ifdef _WIN32
# ifndef WIN32
#  define WIN32
# endif
#include <windows.h>
#include <process.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WITH_NOIO
#include "alt_io.h"
#endif

/**
 * gSoap gSOAP runtime extensions API
 *
 * @addtogroup SOAP_MISCAPI Misc
 * @ingroup WS4D_UTILS
 *
 * @{
 */

#include "ws4d_error.h"

#ifdef WIN32
# define INLINE
# define SOCKLEN_T int
# define SNPRINTF _snprintf
# pragma warning(disable : 4996)        /* Remove snprintf Secure Warnings */
# define STRCASECMP strcmp
# define STRTOK strtok_s
# define SLEEP(seconds) Sleep(seconds*1000)
#else
#ifndef WITH_NOIO
#include <sys/socket.h>
#endif
#define INLINE inline
#define SOCKLEN_T socklen_t
#define SNPRINTF snprintf
#define STRCASECMP strcasecmp
#define STRTOK strtok_r
#define SLEEP(seconds) sleep(seconds)
#endif

#ifndef INET_ADDRSTRLEN
# define INET_ADDRSTRLEN 16
#endif

#ifndef INET6_ADDRSTRLEN
# define INET6_ADDRSTRLEN 46
#endif


#include "ws4d_config.h"

#define DPWS_SYNC 0

#ifdef ABORT_ON_ASSERT
#include <stdlib.h>
#define soap_abort_on_assert() abort()
#else
#define soap_abort_on_assert()
#endif

#ifdef DEBUG
#define ws4d_assert(expr, val) \
  if (expr) {} else { \
    fprintf(stderr, "(%s:%d) assertion failed: (%s)\n", __FILE__, __LINE__, #expr); \
    soap_abort_on_assert();\
    return val; \
  }

#define ws4d_fail(expr, err) \
  if (expr) { \
    fprintf(stderr, "(%s:%d) expression failed: (%s)\n", __FILE__, __LINE__, #expr); \
    return err; \
  }

#else

#define ws4d_assert(expr, val) \
  if (expr) {} else { \
    soap_abort_on_assert();\
    return val; \
  }

#define ws4d_fail(expr, err) \
  if (expr) { \
    return err; \
  }
#endif

#define WS4D_UNUSED_PARAM(param) (param = param)

#include "ws4d_mutex.h"

#include "ws4d_list.h"

#include "ws4d_abstract_allocator.h"

#include "ws4d_alloclist.h"

#include "ws4d_stringarray.h"

#include "ws4d_time.h"

#include "ws4d_uuid.h"

#include "ws4d_uri.h"

#include "ws4d_qname.h"

#include "ws4d_localizedstring.h"

#include "ws4d_device_description.h"

#ifdef HAVE_INET_PTON_H
#define ws4d_inet_pton inet_pton
#else
int ws4d_inet_pton (int af, const char *src, void *dst);
#endif

#ifdef HAVE_INET_NTOP_H
#define ws4d_inet_ntop inet_ntop
#else
const char *ws4d_inet_ntop (int af, const void *src, char *dst, size_t size);
#endif

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*WS4D_MISC_H_ */
