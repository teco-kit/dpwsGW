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

#include "stdsoap2.h"
#include "soap_misc.h"

#include "dpwsH.h"

#include "ws4d_abstract_eprlist.h"
#include "ws4d_targetcache.h"

#ifndef WSD_CLIENT
#define WSD_CLIENT
#endif

#include "ws-addressing.h"
#include "ws-discovery.h"
#include "ws4d_discovery_ctx.h"

int
ws4d_discoveryctx_init (struct dpws_probectx *ctx, const char *MsgId,
                        const char *Host, struct ws4d_tc_int *tc,
                        struct soap *soap, struct wsd_dis_hooks_t *dis_hooks,
                        struct wsd_uni_hooks_t *uni_hooks, int backlog)
{
  int err = WS4D_OK;

  /* test parameters */
  ws4d_assert (ctx && MsgId && tc, WS4D_ERR);

  memset (ctx, 0, sizeof (struct dpws_probectx));
  WS4D_ALLOCLIST_INIT (&ctx->alist);

  ctx->MsgId = ws4d_strdup (MsgId, &ctx->alist);

  if (!soap)
    {
      ctx->handle = ws4d_malloc_alist (sizeof (struct soap), &ctx->alist);
      if (!ctx->handle)
        return WS4D_EOM;

      soap_init (ctx->handle);
#ifdef DEBUG
      soap_omode (ctx->handle, SOAP_XML_INDENT);
#endif

    }
  else
    {
      ctx->handle = soap;
    }

  ctx->tc = tc;

  err = wsa_register_handle (ctx->handle);
  if (err != SOAP_OK)
    return WS4D_ERR;

  err = wsd_soap_init_explicit (ctx->handle, ctx->tc, Host, backlog,
                                uni_hooks, dis_hooks);
  if (err != SOAP_OK)
    return WS4D_ERR;

  return WS4D_OK;
}

int
ws4d_discoveryctx_init_directed (struct dpws_probectx *ctx,
                                 const char *MsgId, const char *Host,
                                 struct ws4d_tc_int *tc, struct soap *soap,
                                 struct wsd_dis_hooks_t *dis_hooks,
                                 struct wsd_uni_hooks_t *uni_hooks,
                                 int backlog)
{
  int err = WS4D_OK;

  /* test parameters */
  ws4d_assert (ctx && MsgId && tc, WS4D_ERR);

  memset (ctx, 0, sizeof (struct dpws_probectx));
  WS4D_ALLOCLIST_INIT (&ctx->alist);

  ctx->MsgId = ws4d_strdup (MsgId, &ctx->alist);

  if (!soap)
    {
      ctx->handle = ws4d_malloc_alist (sizeof (struct soap), &ctx->alist);
      if (!ctx->handle)
        return WS4D_EOM;

      soap_init (ctx->handle);
#ifdef DEBUG
      soap_omode (ctx->handle, SOAP_XML_INDENT);
#endif

    }
  else
    {
      ctx->handle = soap;
    }

  ctx->tc = tc;

  err = wsa_register_handle (ctx->handle);
  if (err != SOAP_OK)
    return WS4D_ERR;

  err = wsd_soap_init_directed (ctx->handle, ctx->tc, Host, backlog,
                                uni_hooks, dis_hooks);
  if (err != SOAP_OK)
    return WS4D_ERR;

  return WS4D_OK;
}

int
ws4d_discoveryctx_set_Address (struct dpws_probectx *ctx, const char *Address)
{
  /* test parameters */
  ws4d_assert (ctx && Address, WS4D_ERR);

  ctx->Address = ws4d_strdup (Address, &ctx->alist);
  return WS4D_OK;
}

int
ws4d_discoveryctx_set_Types (struct dpws_probectx *ctx,
                             ws4d_qnamelist * Types)
{
  /* test parameters */
  ws4d_assert (ctx, WS4D_ERR);

  ctx->Types = Types;
  return WS4D_OK;
}

int
ws4d_discoveryctx_set_Scope (struct dpws_probectx *ctx, const char *Scope)
{
  /* test parameters */
  ws4d_assert (ctx, WS4D_ERR);

  ctx->Scope = Scope;
  return WS4D_OK;
}

int
ws4d_discoveryctx_done (struct dpws_probectx *ctx)
{
  /* test parameters */
  ws4d_assert (ctx, WS4D_ERR);

  soap_end (ctx->handle);
  soap_done (ctx->handle);

  ws4d_alloclist_done (&ctx->alist);

  memset (ctx, 0, sizeof (struct dpws_probectx));

  return WS4D_OK;
}

int
ws4d_discoveryctx_probe (struct dpws_probectx *ctx, ws4d_time timeout)
{
  int err;

  /* test parameters */
  ws4d_assert (ctx && ctx->MsgId, WS4D_ERR);

  ctx->timeout = ws4d_systime_ms () + timeout;
  err = wsd_probe_async (ctx->handle, ctx->MsgId, NULL, ctx->Types,
                         ctx->Scope, timeout, &ctx->alist);
  if (err != SOAP_OK)
    return WS4D_ERR;

  return WS4D_OK;
}

int
ws4d_discoveryctx_dprobe (struct dpws_probectx *ctx, const char *To,
                          ws4d_time timeout)
{
  int ret;

  /* test parameters */
  ws4d_assert (ctx && ctx->MsgId && To, WS4D_ERR);

  ctx->timeout = ws4d_systime_ms () + timeout;
  ret = wsd_probe_async (ctx->handle, ctx->MsgId, To, ctx->Types, ctx->Scope,
                         timeout, &ctx->alist);

  if (ret == 202)
    {
      return WS4D_TO;
    }
  else
    {
      if (ret == SOAP_OK)
        {
          return WS4D_OK;
        }
      else
        {
          return WS4D_ERR;
        }
    }
}

int
ws4d_discoveryctx_resolve (struct dpws_probectx *ctx, ws4d_time timeout)
{
  /* test parameters */
  ws4d_assert (ctx && ctx->MsgId, WS4D_ERR);

  ctx->timeout = ws4d_systime_ms () + timeout;

  return wsd_resolve_async (ctx->handle, ctx->MsgId, ctx->Address, timeout,
                            &ctx->alist);
}

int
ws4d_discoveryctx_process (struct dpws_probectx *ctx)
{
  /* test parameters */
  ws4d_assert (ctx && ctx->handle, WS4D_ERR);

  if (ws4d_systime_ms () < ctx->timeout)
    {
      if (wsd_process (ctx->handle, ctx->timeout - ws4d_systime_ms ()))
        {
          if (ctx->handle->error == SOAP_EOF)
            {
              return WS4D_TO;
            }
          else
            {
              return WS4D_ERR;
            }
        }
    }

  return WS4D_TO;
}

int
ws4d_discoveryctx_process_resolve (struct dpws_probectx *ctx)
{
  /* test parameters */
  ws4d_assert (ctx && ctx->handle, WS4D_ERR);

  while (ws4d_systime_ms () < ctx->timeout)
    {
      if (wsd_process_onemessage
          (ctx->handle, ctx->timeout - ws4d_systime_ms ()))
        {
          if (ctx->handle->error == SOAP_EOF)
            {
              return WS4D_TO;
            }
          else
            {
              return WS4D_ERR;
            }
        }
      if (ws4d_tc_get_ResolveResult (ctx->tc, ctx->MsgId))
        {
          return WS4D_OK;
        }
    }

  return WS4D_TO;
}

int
ws4d_discoveryctx_get_ProbeResults (struct dpws_probectx *ctx,
                                    struct ws4d_abs_eprlist *list)
{
  int res;

  /* test parameters */
  ws4d_assert (ctx && ctx->tc && ctx->MsgId && list, WS4D_ERR);

  ws4d_tc_lock (ctx->tc);
  res = ws4d_tc_copy_ProbeResults (ctx->tc, ctx->MsgId, list);
  ws4d_tc_unlock (ctx->tc);
  return res;
}

int
ws4d_discoveryctx_get_dProbeResult (struct dpws_probectx *ctx,
                                    struct ws4d_epr *epr)
{
  int res;

  /* test parameters */
  ws4d_assert (ctx && ctx->tc && ctx->MsgId && epr, WS4D_ERR);

  ws4d_tc_lock (ctx->tc);
  res = ws4d_tc_copy_dProbeResult (ctx->tc, ctx->MsgId, epr);
  ws4d_tc_unlock (ctx->tc);
  return res;
}

int
ws4d_discoveryctx_get_ResolveResult (struct dpws_probectx *ctx,
                                     struct ws4d_epr *epr)
{
  int res;

  /* test parameters */
  ws4d_assert (ctx && ctx->tc && ctx->MsgId && epr, WS4D_ERR);

  ws4d_tc_lock (ctx->tc);
  res = ws4d_tc_copy_ResolveResult (ctx->tc, ctx->MsgId, epr);
  ws4d_tc_unlock (ctx->tc);
  return res;
}
