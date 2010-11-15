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

#ifndef WS4D_DISCOVERY_CTX_H_
#define WS4D_DISCOVERY_CTX_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

/**
 * Discovery Context API
 *
 * @addtogroup DISCOVERY_CTX_API Discovery Context API
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

struct dpws_probectx
{
  struct soap *handle;
  const char *MsgId;
  const char *Address;
  ws4d_qnamelist *Types;
  const char *Scope;
  ws4d_time timeout;
  struct ws4d_tc_int *tc;
  ws4d_alloc_list alist;
};

int ws4d_discoveryctx_init (struct dpws_probectx *ctx, const char *MsgId,
                            const char *Host, struct ws4d_tc_int *tc,
                            struct soap *soap,
                            struct wsd_dis_hooks_t *dis_hooks,
                            struct wsd_uni_hooks_t *uni_hooks, int backlog);

int ws4d_discoveryctx_init_directed (struct dpws_probectx *ctx,
                                     const char *MsgId, const char *Host,
                                     struct ws4d_tc_int *tc,
                                     struct soap *soap,
                                     struct wsd_dis_hooks_t *dis_hooks,
                                     struct wsd_uni_hooks_t *uni_hooks,
                                     int backlog);

int ws4d_discoveryctx_done (struct dpws_probectx *ctx);

int ws4d_discoveryctx_set_Address (struct dpws_probectx *ctx,
                                   const char *Address);

int ws4d_discoveryctx_set_Types (struct dpws_probectx *ctx,
                                 ws4d_qnamelist * Types);

int ws4d_discoveryctx_set_Scope (struct dpws_probectx *ctx,
                                 const char *Scope);

int ws4d_discoveryctx_probe (struct dpws_probectx *ctx, ws4d_time timeout);

int ws4d_discoveryctx_dprobe (struct dpws_probectx *ctx, const char *To,
                              ws4d_time timeout);

int ws4d_discoveryctx_resolve (struct dpws_probectx *ctx, ws4d_time timeout);

int ws4d_discoveryctx_process (struct dpws_probectx *ctx);

int ws4d_discoveryctx_process_resolve (struct dpws_probectx *ctx);

int ws4d_discoveryctx_get_ProbeResults (struct dpws_probectx *ctx,
                                        struct ws4d_abs_eprlist *list);

int ws4d_discoveryctx_get_dProbeResult (struct dpws_probectx *ctx,
                                        struct ws4d_epr *epr);

int ws4d_discoveryctx_get_ResolveResult (struct dpws_probectx *ctx,
                                         struct ws4d_epr *epr);

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /*WS4D_DISCOVERY_CTX_H_ */
