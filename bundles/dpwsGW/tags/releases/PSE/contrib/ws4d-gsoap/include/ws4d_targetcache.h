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

#ifndef WS4D_TARGETCACHE_H_
#define WS4D_TARGETCACHE_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#include "ws4d_target.h"

struct ws4d_tc_int
{
  const char *id;
  void *data;
};

/**
 * Target Cache API
 *
 * @addtogroup APITargetCache Target Cache
 * @ingroup WS4D_UTILS
 *
 * @{
 */

/**
 * Function to initialize a targetcache
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * ws4d_tc_init(&tc, NULL);
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache to init
 * @param arg arguments to target cache implementation
 *
 * @return WS4D_OK on success or failure otherwise
 */
int ws4d_tc_init (struct ws4d_tc_int *tc, void *arg);

/**
 * Function to destroy a targetcache
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * ws4d_tc_init(&tc, NULL);
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache to destroy
 */
void ws4d_tc_done (struct ws4d_tc_int *tc);

/**
 * Check if a target with Address addr is already known in target cache and
 * add it if it is not known.
 *
 * @code
 * int ret;
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ret = ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 * if (ret == WS4D_TARGETCACHE_VALID)
 * {
 *   //target is already known
 * }
 * else if (ret == WS4D_TARGETCACHE_NEW)
 * {
 *   //target is new
 * }
 * else
 * {
 *   //error occured
 * }
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache to check
 * @param addr address of target to check
 * @param metadataversion metadata version of target to check
 *
 * @return WS4D_TARGETCACHE_VALID if hosting_target is known and valid,
 * WS4D_TARGETCACHE_NEW if hosting_target is new or WS4D_TARGETCACHE_ERR on failure.
 */
int
ws4d_tc_checkadd_epr (struct ws4d_tc_int *tc,
                      const char *addr, int metadataversion);

/**
 * Check if a target with Address addr is already known in target cache.
 *
 * @code
 * int ret;
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ret = ws4d_tc_check_epr(&tc, "urn:uuid:[device-id]", 1234567);
 * if (ret == WS4D_TARGETCACHE_VALID)
 * {
 *   //target is already known
 * }
 * else if (ret == WS4D_TARGETCACHE_NEW)
 * {
 *   //target is new
 * }
 * else
 * {
 *   //error occured
 * }
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache to check
 * @param addr address of target to check
 * @param metadataversion metadata version of target to check
 *
 * @return WS4D_TARGETCACHE_VALID if hosting_target is known and valid,
 * WS4D_TARGETCACHE_NEW if hosting_target is new or WS4D_TARGETCACHE_ERR on failure.
 */
int
ws4d_tc_check_epr (struct ws4d_tc_int *tc,
                   const char *addr, int metadataversion);

/**
 * Function invalidates a target in the target cache by its address
 *
 * @code
 * int ret;
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * ws4d_tc_invalidate_epr(&tc, "urn:uuid:[device-id]");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param addr Address of target to invalidate
 *
 * @return SOAP_OK on success of SOAP_ERR otherwise
 */
int ws4d_tc_invalidate_epr (struct ws4d_tc_int *tc, const char *addr);

/**
 * Function to get target by address
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_tc_int tc;
 * struct ws4d_epr *target;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * target = ws4d_epr_alloc(1, &alloclist);

 * ws4d_tc_get_byAddr(&tc, "urn:uuid:[device-id]", target);
 *
 * ws4d_epr_free(1, target);
 * ws4d_tc_done(&tc);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param tc targetcache
 * @param addr address for search
 * @param result result is copied to the epr where result is pointing to
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_get_byAddr (struct ws4d_tc_int *tc,
                    const char *addr, struct ws4d_epr *result);

/**
 * Function to update the types of a target
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * ws4d_tc_update_Types(&tc, "urn:uuid:[device-id]", "\"namespace\":type");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param addr address of target to update
 * @param Types qname list of types
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_update_Types (struct ws4d_tc_int *tc,
                      const char *addr, const char *Types);

/**
 * Function to update the scope of a target
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * ws4d_tc_update_Scopes(&tc, "urn:uuid:[device-id]", "http://www.ws4d.org/scopes/testscope");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param addr address of target to update
 * @param Scopes scope
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_update_Scopes (struct ws4d_tc_int *tc,
                       const char *addr, const char *Scopes);

/**
 * Function to update the XAddrs of a target
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * ws4d_tc_update_XAddrs(&tc, "urn:uuid:[device-id]", "http://192.168.1.23:3487/");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param addr address of target to update
 * @param XAddrs address
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_update_XAddrs (struct ws4d_tc_int *tc,
                       const char *addr, const char *XAddrs);

/**
 * Function to set the last Probe Message Id
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * ws4d_tc_set_ProbeMsgId(&tc, "urn:uuid:[device-id]", "urn:uuid:[message-id]");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param addr address of target to update
 * @param ProbeMsgId Probe Message Id
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_set_ProbeMsgId (struct ws4d_tc_int *tc,
                        const char *addr, const char *ProbeMsgId);

/**
 * Function to set the last Resolve Message Id
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * ws4d_tc_set_ResolveMsgId(&tc, "urn:uuid:[device-id]", "urn:uuid:[uuid]");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param addr address of target to update
 * @param ResolveMsgId Resolve Message Id
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_set_ResolveMsgId (struct ws4d_tc_int *tc,
                          const char *addr, const char *ResolveMsgId);

/**
 * Function to get the XAddrs (network address) of a target
 *
 * @code
 * struct ws4d_tc_int tc;
 * char *XAddrs
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 * ws4d_tc_update_XAddrs(&tc, "urn:uuid:[device-id]", "http://192.168.1.23:3487/");
 *
 * XAddrs = ws4d_tc_get_XAddrs(&tc, "urn:uuid:[device-id]");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param addr Address of target
 *
 * @return XAddrs as character string or NULL
 */

const char *ws4d_tc_get_XAddrs (struct ws4d_tc_int *tc, const char *addr);

/**
 * Function to get all targets of a targetcache
 *
 * @code
 * struct ws4d_tc_int tc;
 * ws4d_abs_eprlist targets;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 *
 * ws4d_eprlist_init(&targets, ws4d_eprllist_init, NULL);
 *
 * ws4d_tc_get_targets(&tc, &targets);
 *
 * ws4d_eprlist_done(&targets);
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param list abstract eprlist to copy targets
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_get_targets (struct ws4d_tc_int *tc, struct ws4d_abs_eprlist *list);

/**
 * Function to get the number of results matching a specific probe message id
 *
 * @code
 * struct ws4d_tc_int tc;
 * int result_count;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 * ws4d_tc_set_ProbeMsgId(&tc, "urn:uuid:[device-id]", "urn:uuid:[message-id]");
 *
 * result_count = ws4d_tc_get_ProbeResults(&tc, "urn:uuid:[message-id]");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param ProbeMsgId Probe Message Id
 *
 * @return WS4D_OK on success or error code otherwise
 */
int ws4d_tc_get_ProbeResults (struct ws4d_tc_int *tc, const char *ProbeMsgId);

/**
 * Function to copy all eprs matching a specific probe message id to a eprlist
 *
 * @code
 * struct ws4d_tc_int tc;
 * ws4d_abs_eprlist results;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 * ws4d_tc_set_ProbeMsgId(&tc, "urn:uuid:[device-id]", "urn:uuid:[message-id]");
 *
 * ws4d_eprlist_init(&results, ws4d_eprllist_init, NULL);
 *
 * ws4d_tc_copy_ProbeResults(&tc, "urn:uuid:[message-id]", &results);
 *
 * ws4d_eprlist_done(&results);
 * ws4d_tc_done(tc);
 * @endcode
 *
 * @param tc targetcache
 * @param ProbeMsgId Probe Message Id
 * @param list eprlist to copy matching eprs
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_copy_ProbeResults (struct ws4d_tc_int *tc,
                           const char *ProbeMsgId,
                           struct ws4d_abs_eprlist *list);

/**
 * Function to copy the epr matching a specific dprobe message id
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_tc_int tc;
 * struct ws4d_epr *target;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[uuid]", 1234567);
 * ws4d_tc_set_ProbeMsgId(&tc, "urn:uuid:[device-id]", "urn:uuid:[message-id]");
 *
 * target = ws4d_epr_alloc(1, &alloclist);

 * ws4d_tc_copy_dProbeResult(&tc, "urn:uuid:[message-id]", target);
 *
 * ws4d_epr_free(1, target);
 * ws4d_tc_done(&tc);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param tc targetcache
 * @param ProbeMsgId probe message id
 * @param result result is copied to the epr where result is pointing to
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_copy_dProbeResult (struct ws4d_tc_int *tc,
                           const char *ProbeMsgId, struct ws4d_epr *result);

/**
 * Function to search the target cache for the epr matching a specific resolve message id
 *
 * @code
 * struct ws4d_tc_int tc;
 * int result_count;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[device-id]", 1234567);
 * ws4d_tc_set_ResolveMsgId(&tc, "urn:uuid:[device-id]", "urn:uuid:[message-id]");
 *
 * result_count = ws4d_tc_get_ProbeResults(&tc, "urn:uuid:[message-id]");
 *
 * ws4d_tc_done(&tc);
 * @endcode
 *
 * @param tc targetcache
 * @param ResolveMsgId Resolve Message Id
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_get_ResolveResult (struct ws4d_tc_int *tc, const char *ResolveMsgId);

/**
 * Function to copy the epr matching a specific resolve message id
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_tc_int tc;
 * struct ws4d_epr *target;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ws4d_tc_checkadd_epr(&tc, "urn:uuid:[uuid]", 1234567);
 * ws4d_tc_set_ResolveMsgId(&tc, "urn:uuid:[device-id]", "urn:uuid:[message-id]");
 *
 * target = ws4d_epr_alloc(1, &alloclist);

 * ws4d_tc_copy_ResolveResult(&tc, "urn:uuid:[message-id]", target);
 *
 * ws4d_epr_free(1, target);
 * ws4d_tc_done(&tc);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param tc targetcache
 * @param ResolveMsgId resolve message id
 * @param result result is copied to the epr where result is pointing to
 *
 * @return WS4D_OK on success or error code otherwise
 */
int
ws4d_tc_copy_ResolveResult (struct ws4d_tc_int *tc,
                            const char *ResolveMsgId,
                            struct ws4d_epr *result);

#ifdef WITH_MUTEXES
void ws4d_tc_lock (struct ws4d_tc_int *tc);

void ws4d_tc_unlock (struct ws4d_tc_int *tc);
#else /*WITH_MUTEXES */

/**
 * Macro to lock a targetcache for mutual exclusion
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ws4d_tc_lock(&tc);
 *
 * ws4d_tc_done(&tc);
 * @endcode
 */

#define ws4d_tc_lock(tc)

/**
 * Macro to unlock a targetcache for mutual exclusion
 *
 * @code
 * struct ws4d_tc_int tc;
 *
 * tc = ws4d_tc_init(&tc, NULL);
 *
 * ws4d_tc_lock(&tc);
 * ws4d_tc_unlock(&tc);
 *
 * ws4d_tc_done(&tc);
 * @endcode
 */
#define ws4d_tc_unlock(tc)
#endif /*WITH_MUTEXES */

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*WS4D_TARGETCACHE_H_ */
