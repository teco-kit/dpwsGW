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

#ifndef _MISC_H_
#define _MISC_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#ifdef WITH_NOIO
#include "alt_io.h"
#endif

#include "stdsoap2.h"
#include "ws4d_misc.h"

#ifdef DEBUG

#define soap_assert(handle, expr, val) \
  if (expr) {} else { \
    if (handle) \
    { \
      ((struct soap*) handle)->error = SOAP_ERR; \
    } \
    fprintf(stderr, "(%s:%d) assertion failed: (%s)\n", __FILE__, __LINE__, #expr); \
    soap_abort_on_assert();\
    return val; \
  }

#else

#define soap_assert(handle, expr, val) \
  if (expr) {} else { \
    if (handle) \
    { \
      ((struct soap*) handle)->error = SOAP_ERR; \
    } \
    return val; \
  }

#endif

struct wsdp__LocalizedStringType
  *ws4d_locstring_tosoap (struct ws4d_locstring *string,
                          int size, ws4d_alloc_list * alist);

void soap_locstring_free (struct wsdp__LocalizedStringType *string, int size);

struct ws4d_locstring
  *soap_locstring_tows4d (struct wsdp__LocalizedStringType *string,
                          int size, ws4d_alloc_list * alist);

char *soap_locstring_get (struct wsdp__LocalizedStringType *string,
                          int size, const char *lang);

#ifdef SOAP_TYPE_wsdp__ThisModelType
/* TODO: add documentation */
int
soap_thismodel_copy (struct ws4d_thisModel *dst,
                     struct wsdp__ThisModelType *src,
                     ws4d_alloc_list * alist);
#endif

#ifdef SOAP_TYPE_wsdp__ThisDeviceType
/* TODO: add documentation */
int
soap_thisdevice_copy (struct ws4d_thisDevice *dst,
                      struct wsdp__ThisDeviceType *src,
                      ws4d_alloc_list * alist);
#endif

/**
 * @addtogroup DpwsCustomSerializer Functions for custom de-/serlializing
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

/**
 * TODO: To be documented
 */
char *soap_elem_to_str (void *ptr, char *tag, int type);


/**
 * TODO: To be documented
 */
int soap_begin_deser (struct soap *soap, char *str, size_t str_len,
                      struct Namespace *ns);


/**
 * TODO: To be documented
 */
int soap_end_deser (struct soap *soap);

/** @} */


/**
 * @addtogroup DpwsMAccept Functions for serving on multiple soap handles
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

struct Namespace *soap_extend_namespaces (struct Namespace *orig_namespaces,
                                          struct Namespace *extns,
                                          ws4d_alloc_list * alist);


void *soap_getpeer (struct soap *soap);

size_t soap_getpeerlen (struct soap *soap);

/**
 * Macro to create a set of soap handels as parameter for soap_maccept
 *
 * @code
 * struct soap service1, service2;
 * struct soap *soap_set[] = SOAP_HANDLE_SET (&service1, &service2);
 * @endcode
 */
#ifdef _MSC_VER
#if _MSC_VER >= 1400
#define SOAP_HANDLE_SET(...) { __VA_ARGS__, NULL, NULL, NULL }
#endif
#else
#define SOAP_HANDLE_SET(...) { __VA_ARGS__, NULL, NULL, NULL }
#endif


/**
 * Function to listen for new messages on multiple soap handles. This function
 * is typically used in the loop where new messages are received and processed.
 *
 * @code
 * struct soap service1, service2;
 * [...]
 * for (;;)
 *  {
 *     struct soap *handle = NULL;
 *     struct soap *soap_set[] = SOAP_HANDLE_SET (&service1, &service2);
 *     int soap_count = 2;
 *
 *     handle = dpws_maccept (&device, 100000, soap_count, soap_set);
 *     if (handle)
 *      {
 *         if (soap_serve (handle))
 *          {
 *             soap_print_fault (handle, stderr);
 *          }
 *         soap_end (handle);
 *      }
 *  }
 * @endcode
 *
 * @param timeout time how long dpws_maccept should block
 * @param count number of soap handles in soap set
 * @param soap_handles soap handles set to listen on
 */
struct soap *soap_maccept (ws4d_time timeout, int count,
                           struct soap **soap_handles);

/**
 * Macro to create a set of soap serve functions as parameter for soap_mserve
 *
 * @code
 * int sis_serve_request (struct soap *soap);
 * int ats_serve_request (struct soap *soap);
 * struct soap *soap_set[] = SOAP_SERVE_SET (sis_serve_request, ats_serve_request);
 * @endcode
 */
#ifdef _MSC_VER
#if _MSC_VER >= 1400
#define SOAP_SERVE_SET(...) { __VA_ARGS__, NULL, NULL, NULL }
#endif
#else
#define SOAP_SERVE_SET(...) { __VA_ARGS__, NULL, NULL, NULL }
#endif

/**
 * Function to serve multiple serve request funtions generated by soapcpp2 at
 * a soap handle. This function is typically used in the loop where new
 * messages are received and processed.
 *
 * @code
 * int sis_serve_request (struct soap *soap);
 * int ats_serve_request (struct soap *soap);
 *
 * struct soap service1, service2;
 * [...]
 * for (;;)
 *  {
 *     struct soap *handle = NULL;
 *     struct soap *soap_handle_set[] = SOAP_HANDLE_SET (&service1, &service2);
 *     int soap_handle_count = 2;
 *     struct soap *soap_serve_set[] = SOAP_SERVE_SET (sis_serve_request, ats_serve_request);
 *     int soap_serve_count = 2;
 *
 *     handle = soap_maccept (&device, 100000, soap_count, soap_handle_set);
 *     if (handle)
 *      {
 *         if (soap_mserve (handle, soap_serve_count, serve_serve_set))
 *          {
 *             soap_print_fault (handle, stderr);
 *          }
 *         soap_end (handle);
 *      }
 *  }
 * @endcode
 *
 * @param soap handle to serve new request
 * @param count number of serve request functions in serve functions set
 * @param serve_requests set of serve request functions
 */
int
soap_mserve (struct soap *soap, int count,
             int (*serve_requests[])(struct soap * soap));

/** @} */

struct Namespace *soap_qnamelist_namespaces (ws4d_qnamelist * head,
                                             ws4d_alloc_list * alist,
                                             struct Namespace *namespaces);

char *soap_gen_prefix (int *num, ws4d_alloc_list * alist,
                       struct Namespace *namespaces);

int soap_out_transform_qnames (struct soap *soap, char **outstring,
                               char *const *a);

int soap_in_expand_qnames (struct soap *soap, char **instring, char **a);

int soap_header_new (struct soap *soap, size_t size);

/**
 * gSOAP based allocator implementation
 *
 * @addtogroup APIgSOAPAllocatorImplementation gSOAP based allocator implementation
 * @ingroup APIAbsAllocatorImplementations
 *
 * @{
 */

/**
 * function to create a gSOAP based abstract allocator
 *
 * Don't call this function itself! But use it with ws4d_allocator_init() !
 * @code
 * ws4d_abs_allocator alloc;
 * struct soap soap;
 *
 * soap_init(&soap);
 * ws4d_eprlist_init(&alloc, soap_allocator_finit, &soap);
 * ws4d_eprlist_done(&alloc);
 * soap_done(&soap);
 * @endcode
 *
 * @param allocator allocator to initialize
 * @param arg argument for allocator, in this case an initialized gSOAP handle
 */
int soap_allocator_finit (struct ws4d_abs_allocator *allocator, void *arg);

/** @} */

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif
#endif /* _MISC_H_ */
