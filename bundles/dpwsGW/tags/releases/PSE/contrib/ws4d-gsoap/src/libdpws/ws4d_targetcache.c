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

#include "ws4d_misc.h"
#include "ws4d_abstract_eprlist.h"

#include "ws4d_target.h"
#include "ws4d_targetcache.h"

#include "ws4d_eprllist.h"

/**
 * Target Cache
 */
#define WS4D_TARGETCACHE_ID "WS4D-Target-Cache-0.1"

const char *ws4d_targetcache_id = WS4D_TARGETCACHE_ID;

struct ws4d_targetcache_data
{
  struct ws4d_abs_eprlist cache;
};

#define targetcache_checkid(tc) \
  ((tc) && (tc)->id && (((tc)->id == ws4d_targetcache_id)||(!strcmp((tc)->id, ws4d_targetcache_id))))

int
ws4d_tc_check_epr (struct ws4d_tc_int *tc, const char *Address,
                   int MetadataVersion)
{
  register struct ws4d_epr *epr = NULL;
  struct ws4d_targetcache_data *data = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  /* search for hosting_target in discovery cache */
  epr = ws4d_eprlist_get_byAddr (&data->cache, Address);

  /* if hosting_target is not known */
  if (ws4d_epr_isvalid (epr))
    {
      if (MetadataVersion == -1)
        {
          return WS4D_TARGETCACHE_VALID;
        }
      else
        {
          /* check if MetadataVersion is set */
          int tmp = ws4d_targetep_get_MetadataVersion (epr);
          if (tmp == -1)
            {
              ws4d_targetep_set_MetadataVersion (epr, MetadataVersion);
              return WS4D_TARGETCACHE_VALID;
            }
          else
            {
              /* check if MetadataVersion has changed */
              if (MetadataVersion > tmp)
                {
                  ws4d_epr_invalidate (epr);
                  ws4d_eprlist_remove (&data->cache, epr);
                  ws4d_eprlist_free (&data->cache, epr);
                  return WS4D_TARGETCACHE_INVALID;
                }
              else
                {
                  return WS4D_TARGETCACHE_VALID;
                }
            }
        }
    }
  else
    {
      return WS4D_TARGETCACHE_INVALID;
    }
}

int
ws4d_tc_checkadd_epr (struct ws4d_tc_int *tc, const char *Address,
                      int MetadataVersion)
{
  register struct ws4d_epr *epr = NULL;
  struct ws4d_targetcache_data *data = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  if (ws4d_tc_check_epr (tc, Address, MetadataVersion) ==
      WS4D_TARGETCACHE_VALID)
    {
      return WS4D_TARGETCACHE_VALID;
    }
  else
    {
      /* add hosting_target to discovery cache */
      epr = ws4d_eprlist_alloc (&data->cache);

      /* error */
      if (!epr)
        return WS4D_TARGETCACHE_ERR;

      ws4d_register_targetep (epr);

      /* set address */
      ws4d_epr_set_Addrs (epr, Address);

      /* set MetadataVersion */
      if (MetadataVersion != -1)
        {
          ws4d_targetep_set_MetadataVersion (epr, MetadataVersion);
        }

      /* set entry valid */
      ws4d_epr_validate (epr);

      ws4d_eprlist_add (&data->cache, epr);

      return WS4D_TARGETCACHE_NEW;
    }
}

int
ws4d_tc_invalidate_epr (struct ws4d_tc_int *tc, const char *Address)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *target = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  target = ws4d_eprlist_get_byAddr (&data->cache, Address);

  if (ws4d_epr_isvalid (target))
    {
      ws4d_epr_invalidate (target);
      ws4d_eprlist_remove (&data->cache, target);
      ws4d_eprlist_free (&data->cache, target);

      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_tc_get_byAddr (struct ws4d_tc_int *tc, const char *Address,
                    struct ws4d_epr *result)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *source = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address && result, WS4D_ERR);
  data = tc->data;

  source = ws4d_eprlist_get_byAddr (&data->cache, Address);
  if (source == NULL)
    return WS4D_ERR;

  return ws4d_epr_copy (result, source);
}

/* TODO: implement update behavior */
int
ws4d_tc_update_Types (struct ws4d_tc_int *tc, const char *Address,
                      const char *Types)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *target = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address
               && Types, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  target = ws4d_eprlist_get_byAddr (&data->cache, Address);
  if (!target)
    {
      return WS4D_TARGETCACHE_ERR;
    }

  if (ws4d_targetep_set_Types (target, Types) != WS4D_OK)
    {
      return WS4D_TARGETCACHE_ERR;
    }
  else
    {
      return WS4D_TARGETCACHE_OK;
    }
}

/* TODO: implement update behavior */
int
ws4d_tc_update_Scopes (struct ws4d_tc_int *tc, const char *Address,
                       const char *Scopes)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *target = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address
               && Scopes, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  target = ws4d_eprlist_get_byAddr (&data->cache, Address);
  if (!target)
    {
      return WS4D_TARGETCACHE_ERR;
    }

  if (ws4d_targetep_set_Scopes (target, Scopes) != WS4D_OK)
    {
      return WS4D_TARGETCACHE_ERR;
    }
  else
    {
      return WS4D_TARGETCACHE_OK;
    }
}

/* TODO: implement update behavior */
int
ws4d_tc_update_XAddrs (struct ws4d_tc_int *tc, const char *Address,
                       const char *XAddrs)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *target = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address
               && XAddrs, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  target = ws4d_eprlist_get_byAddr (&data->cache, Address);
  if (!target)
    {
      return WS4D_TARGETCACHE_ERR;
    }

  if (ws4d_targetep_set_XAddrs (target, XAddrs) != WS4D_OK)
    {
      return WS4D_TARGETCACHE_ERR;
    }
  else
    {
      return WS4D_TARGETCACHE_OK;
    }
}

const char *
ws4d_tc_get_XAddrs (struct ws4d_tc_int *tc, const char *Address)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *target = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address, NULL);
  data = tc->data;

  target = ws4d_eprlist_get_byAddr (&data->cache, Address);
  if (target)
    {
      return (const char *) ws4d_targetep_get_XAddrs (target);
    }
  else
    {
      return NULL;
    }
}

int
ws4d_tc_get_targets (struct ws4d_tc_int *tc, struct ws4d_abs_eprlist *list)
{
  struct ws4d_targetcache_data *data = NULL;
  register struct ws4d_epr *target, *iter;
  int matches = 0;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && list, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  ws4d_eprlist_foreach (target, iter, &data->cache)
  {
    if (ws4d_epr_isvalid (target))
      {
        struct ws4d_epr *copy = ws4d_eprlist_alloc (list);

        if (ws4d_epr_copy (copy, target) != WS4D_OK)
          {
            ws4d_eprlist_free (list, copy);
            break;
          }

        if (ws4d_eprlist_add (list, copy) != WS4D_OK)
          {
            ws4d_eprlist_free (list, copy);
            break;
          }

        matches++;
      }
  }

  return matches;
}

int
ws4d_tc_set_ProbeMsgId (struct ws4d_tc_int *tc, const char *Address,
                        const char *ProbeMsgId)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *target = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address
               && ProbeMsgId, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  target = ws4d_eprlist_get_byAddr (&data->cache, Address);
  if (!target)
    {
      return WS4D_TARGETCACHE_ERR;
    }

  if (ws4d_targetep_set_ProbeMsgId (target, ProbeMsgId) != WS4D_OK)
    {
      return WS4D_TARGETCACHE_ERR;
    }
  else
    {
      return WS4D_TARGETCACHE_OK;
    }
}

int
ws4d_tc_set_ResolveMsgId (struct ws4d_tc_int *tc, const char *Address,
                          const char *ResolveMsgId)
{
  struct ws4d_targetcache_data *data = NULL;
  struct ws4d_epr *target = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && Address
               && ResolveMsgId, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  /* TODO: should check if there is an duplicate ID */

  target = ws4d_eprlist_get_byAddr (&data->cache, Address);
  if (!target)
    {
      return WS4D_TARGETCACHE_ERR;
    }

  if (ws4d_targetep_set_ResolveMsgId (target, ResolveMsgId) != WS4D_OK)
    {
      return WS4D_TARGETCACHE_ERR;
    }
  else
    {
      return WS4D_TARGETCACHE_OK;
    }
}

int
ws4d_tc_get_ProbeResults (struct ws4d_tc_int *tc, const char *ProbeMsgId)
{
  struct ws4d_targetcache_data *data = NULL;
  register struct ws4d_epr *target, *iter;
  register int matches = 0;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && ProbeMsgId, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  ws4d_eprlist_foreach (target, iter, &data->cache)
  {
    char *MsgId = (char *) ws4d_targetep_get_ProbeMsgId (target);

    if (MsgId && !strcmp (MsgId, ProbeMsgId))
      {
        matches++;
      }
  }

  return matches;
}

int
ws4d_tc_copy_ProbeResults (struct ws4d_tc_int *tc,
                           const char *ProbeMsgId,
                           struct ws4d_abs_eprlist *list)
{
  struct ws4d_targetcache_data *data = NULL;
  register struct ws4d_epr *target, *iter;
  int matches = 0;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && ProbeMsgId
               && list, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  ws4d_eprlist_foreach (target, iter, &data->cache)
  {
    char *MsgId = (char *) ws4d_targetep_get_ProbeMsgId (target);

    if (MsgId && !strcmp (MsgId, ProbeMsgId))
      {
        struct ws4d_epr *copy = ws4d_eprlist_alloc (list);

        if (ws4d_epr_copy (copy, target) != WS4D_OK)
          {
            ws4d_eprlist_free (list, copy);
            break;
          }

        if (ws4d_eprlist_add (list, copy) != WS4D_OK)
          {
            ws4d_eprlist_free (list, copy);
            break;
          }

        matches++;
      }
  }

  return matches;
}

int
ws4d_tc_copy_dProbeResult (struct ws4d_tc_int *tc,
                           const char *ProbeMsgId, struct ws4d_epr *epr)
{
  struct ws4d_targetcache_data *data = NULL;
  register struct ws4d_epr *target, *iter;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && ProbeMsgId
               && epr, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  ws4d_eprlist_foreach (target, iter, &data->cache)
  {
    char *MsgId = (char *) ws4d_targetep_get_ProbeMsgId (target);

    if (MsgId && !strcmp (MsgId, ProbeMsgId))
      {
        return ws4d_epr_copy (epr, target);
      }
  }

  return WS4D_TARGETCACHE_ERR;
}

int
ws4d_tc_get_ResolveResult (struct ws4d_tc_int *tc, const char *ResolveMsgId)
{
  struct ws4d_targetcache_data *data = NULL;
  register struct ws4d_epr *target, *iter;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc)
               && ResolveMsgId, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  ws4d_eprlist_foreach (target, iter, &data->cache)
  {
    char *MsgId = (char *) ws4d_targetep_get_ResolveMsgId (target);

    if (MsgId && !strcmp (MsgId, ResolveMsgId))
      {
        return 1;
      }
  }

  return 0;
}

int
ws4d_tc_copy_ResolveResult (struct ws4d_tc_int *tc,
                            const char *ResolveMsgId, struct ws4d_epr *epr)
{
  struct ws4d_targetcache_data *data = NULL;
  register struct ws4d_epr *target, *iter;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc) && ResolveMsgId
               && epr, WS4D_TARGETCACHE_ERR);
  data = tc->data;

  ws4d_eprlist_foreach (target, iter, &data->cache)
  {
    char *MsgId = (char *) ws4d_targetep_get_ResolveMsgId (target);

    if (MsgId && !strcmp (MsgId, ResolveMsgId))
      {
        return ws4d_epr_copy (epr, target);
      }
  }

  return WS4D_TARGETCACHE_ERR;
}

#ifdef WITH_MUTEXES
void
ws4d_tc_lock (struct ws4d_tc_int *tc)
{
  struct ws4d_targetcache_data *data = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc),);
  data = tc->data;

  ws4d_eprlist_lock (&data->cache);
}

void
ws4d_tc_unlock (struct ws4d_tc_int *tc)
{
  struct ws4d_targetcache_data *data = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc),);
  data = tc->data;

  ws4d_eprlist_unlock (&data->cache);
}
#endif

void
ws4d_tc_done (struct ws4d_tc_int *tc)
{
  struct ws4d_targetcache_data *data = NULL;

  /* test parameters */
  ws4d_assert (targetcache_checkid (tc),);
  data = tc->data;

  ws4d_eprlist_done (&data->cache);

  ws4d_free (data);
}

int
ws4d_tc_init (struct ws4d_tc_int *tc, void *arg)
{
  struct ws4d_targetcache_data *data = NULL;
  int err = 0;

  WS4D_UNUSED_PARAM (arg);

  tc->id = ws4d_targetcache_id;

  data = ws4d_malloc (sizeof (struct ws4d_targetcache_data));
  ws4d_assert (data, WS4D_EOM);

  err = ws4d_eprlist_init (&data->cache, ws4d_eprllist_init, NULL);
  ws4d_assert (!err, err);

  tc->data = data;

  return WS4D_OK;
}

/** @} */
