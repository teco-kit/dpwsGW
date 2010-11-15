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

#ifndef WS4D_TARGETSERVICE_H_
#define WS4D_TARGETSERVICE_H_

/**
 * Target Service
 *
 * @addtogroup APITargetService Target Service
 * @ingroup WS4D_UTILS
 *
 * @{
 */

struct ws4d_appsequence
{
  unsigned int InstanceId;
  const char *SequenceId;
  unsigned int MessageNumber;
};

struct ws4d_targetservice
{
  struct ws4d_abs_eprlist targets;
  struct ws4d_appsequence as;
  struct ws4d_list_node MatchBy;
  ws4d_alloc_list alist;
};

typedef int (*scope_match_func_t) (const char *s1, const char *s2);

/**
 * TODO: add documentation
 */
int ws4d_targetservice_init (struct ws4d_targetservice *ts,
                             const char *defMatchBy,
                             scope_match_func_t defscope_match_func);

/**
 * TODO: add documentation
 */
int ws4d_targetservice_done (struct ws4d_targetservice *ts);

/**
 * TODO: add documentation
 */
int
ws4d_targetservice_addMatchBy (struct ws4d_targetservice *ts,
                               const char *MatchBy,
                               scope_match_func_t scope_match_func);

/**
 * TODO: add documentation
 */
int
ws4d_targetservice_delMatchBy (struct ws4d_targetservice *ts,
                               const char *MatchBy);

/**
 * TODO: add documentation
 */
struct ws4d_epr *ws4d_targetservice_inittarget (struct ws4d_targetservice *ts,
                                                const char *Addrs,
                                                const char *XAddrs);

/**
 * TODO: add documentation
 */
int ws4d_targetservice_deltarget (struct ws4d_targetservice *ts,
                                  struct ws4d_epr *target);

/**
 * TODO: add documentation
 */
struct ws4d_epr *ws4d_targetservice_gettarget_byAddr (struct
                                                      ws4d_targetservice *ts,
                                                      const char *addr);

/**
 * TODO: add documentation
 */
int ws4d_targetservice_getmatches (struct ws4d_targetservice *ts,
                                   const char *Scopes,
                                   const char *MatchBy,
                                   const char *Types,
                                   struct ws4d_abs_eprlist *matches);

/**
 * TODO: add documentation
 */
int ws4d_targetservice_inc_MessageNumber (struct ws4d_targetservice *ts);

/**
 * TODO: add documentation
 */
struct ws4d_appsequence
  *ws4d_targetservice_get_appsequence (struct ws4d_targetservice *ts);

/** @} */

#endif /*WS4D_TARGETSERVICE_H_ */
