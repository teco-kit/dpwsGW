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
#include "ws4d_epr.h"
#include "ws4d_subscription.h"
#include "ws4d_subsmanager.h"

int
ws4d_subsm_init (struct ws4d_subsmanager *subsm, const char *addr)
{
  ws4d_assert (subsm, WS4D_ERR);

  WS4D_INIT_LIST (&subsm->subs_list);
  WS4D_INIT_LIST (&subsm->supp_delivery_list);
  WS4D_INIT_LIST (&subsm->supp_filter_list);
  WS4D_ALLOCLIST_INIT (&subsm->alist);
  subsm->default_delivery = NULL;
  subsm->default_filter = NULL;
  subsm->address = ws4d_strdup ((char *) addr, &subsm->alist);

#ifdef WITH_MUTEXES
  ws4d_mutex_init (&subsm->lock);
#endif

  return WS4D_OK;
}

int
ws4d_subsm_done (struct ws4d_subsmanager *subsm)
{
  register struct ws4d_subscription *cur = NULL, *next = NULL;

  ws4d_assert (subsm, WS4D_ERR);

  ws4d_list_foreach (cur, next, &subsm->subs_list, struct ws4d_subscription,
                     list)
  {
    ws4d_subsm_clean_delivery (subsm, cur);
    ws4d_subsm_clean_filter (subsm, cur);
  }
  ws4d_subslist_clear (&subsm->subs_list);

  ws4d_subsm_unregister_alldelivery (subsm);
  ws4d_subsm_unregister_allfilter (subsm);

  ws4d_alloclist_done (&subsm->alist);

  return WS4D_OK;
}

#ifdef WITH_MUTEXES
void
ws4d_subsm_lock (struct ws4d_subsmanager *subsm)
{
  /* test parameters */
  ws4d_assert (subsm,);

  ws4d_mutex_lock (&subsm->lock);
}

void
ws4d_subsm_unlock (struct ws4d_subsmanager *subsm)
{
  /* test parameters */
  ws4d_assert (subsm,);

  ws4d_mutex_unlock (&subsm->lock);
}
#endif

struct ws4d_subscription *
ws4d_subsm_getsubs (struct ws4d_subsmanager *subsm, const char *id)
{
  return ws4d_subslist_getsubs (&subsm->subs_list, id);
}

char *
ws4d_subsm_getaddr (struct ws4d_subsmanager *subsm)
{
  ws4d_assert (subsm, NULL);

  return subsm->address;
}

struct ws4d_subscription *
ws4d_subsm_allocsubs (struct ws4d_subsmanager *subsm)
{
  int err;
  struct ws4d_subscription *subs;
  char uuidstr[WS4D_UUID_SIZE];
  char id_buf[WS4D_UUID_SCHEMA_SIZE];

  ws4d_assert (subsm, NULL);

  subs =
    ws4d_malloc_alist (sizeof (struct ws4d_subscription),
                       ws4d_subsm_get_alist (subsm));
  if (!subs)
    return NULL;

  ws4d_uuid_generate_random (uuidstr);
  ws4d_uuid_generate_schema (id_buf, uuidstr);

  err = ws4d_subs_init (subs, id_buf);
  if (err)
    return NULL;

  return subs;
}

int
ws4d_subsm_freesubs (struct ws4d_subsmanager *subsm,
                     struct ws4d_subscription *subs)
{
  ws4d_assert (subsm && subs, WS4D_ERR);

  ws4d_subsm_clean_delivery (subsm, subs);
  ws4d_subsm_clean_filter (subsm, subs);

  ws4d_subs_done (subs);

  return WS4D_OK;
}

int
ws4d_subsm_regsubs (struct ws4d_subsmanager *subsm,
                    struct ws4d_subscription *subs)
{
  /* test parameter */
  ws4d_assert (subsm && subs && subs->delivery_mode
               && subs->filter_mode, WS4D_ERR);

  if (ws4d_list_empty (&subsm->subs_list))
    {
      ws4d_list_add (&subs->list, &subsm->subs_list);
    }
  else
    {
      register struct ws4d_subscription *cur = NULL, *next;
      ws4d_list_foreach (cur, next, &subsm->subs_list,
                         struct ws4d_subscription, list)
      {
        if (cur->expiration > subs->expiration)
          {
            ws4d_list_add (&subs->list, cur->list.prev);
            break;
          }

      }
      if (cur->expiration <= subs->expiration)
        {
          ws4d_list_add_tail (&subs->list, &subsm->subs_list);
        }
    }

  return WS4D_OK;
}

int
ws4d_subsm_expiresubs (struct ws4d_subsmanager *subsm,
                       struct ws4d_subscription *subs)
{
  ws4d_assert (subsm && subs, WS4D_ERR);

  ws4d_list_del (&subs->list);

  ws4d_subsm_freesubs (subsm, subs);

  return WS4D_OK;
}


/* event delivery register */

static struct ws4d_event_delivery *
_ws4d_subsm_geteventdelivery (struct ws4d_subsmanager *subsm,
                              const char *Mode)
{
  register struct ws4d_event_delivery *delivery, *next;

  ws4d_assert (subsm && Mode, NULL);

  ws4d_list_foreach (delivery, next, &subsm->supp_delivery_list,
                     struct ws4d_event_delivery, list)
  {
    if (delivery->Mode == Mode)
      {
        return delivery;
      }
  }

  ws4d_list_foreach (delivery, next, &subsm->supp_delivery_list,
                     struct ws4d_event_delivery, list)
  {
    if (!strcmp (delivery->Mode, Mode))
      {
        return delivery;
      }
  }

  return NULL;
}


int
ws4d_subsm_register_eventdelivery (struct ws4d_subsmanager *subsm,
                                   const char *Mode,
                                   ws4d_processDelivery_cb fprocessDelivery,
                                   ws4d_cleanDelivery_cb fcleanDelivery)
{
  struct ws4d_event_delivery *deliveryType = NULL;

  ws4d_assert (subsm && Mode && fprocessDelivery && fcleanDelivery, WS4D_ERR);

  /* TODO: check for duplicates first */

  deliveryType =
    ws4d_malloc_alist (sizeof (struct ws4d_event_delivery),
                       ws4d_subsm_get_alist (subsm));
  if (!deliveryType)
    return WS4D_EOM;

  deliveryType->Mode = Mode;
  deliveryType->fprocessDelivery = fprocessDelivery;
  deliveryType->fcleanDelivery = fcleanDelivery;

  ws4d_list_add (&deliveryType->list, &subsm->supp_delivery_list);

  return WS4D_OK;
}

int
ws4d_subsm_register_defeventdelivery (struct ws4d_subsmanager *subsm,
                                      const char *Mode,
                                      ws4d_processDelivery_cb
                                      fprocessDelivery,
                                      ws4d_cleanDelivery_cb fcleanDelivery)
{
  int ret;

  ws4d_assert (subsm, WS4D_ERR);

  ret = ws4d_subsm_register_eventdelivery (subsm, Mode,
                                           fprocessDelivery, fcleanDelivery);
  if (ret == WS4D_OK)
    {
      struct ws4d_event_delivery *deliveryType = NULL;

      deliveryType = _ws4d_subsm_geteventdelivery (subsm, Mode);
      if (deliveryType)
        {
          subsm->default_delivery = deliveryType;
        }
      else
        {
          return WS4D_ERR;
        }
    }

  return ret;
}

static int
_ws4d_subsm_unregister_eventdelivery (struct ws4d_event_delivery *delivery)
{
  if (delivery)
    ws4d_free_alist (delivery);

  return WS4D_OK;
}

int
ws4d_subsm_unregister_eventdelivery (struct ws4d_subsmanager *subsm,
                                     const char *Mode)
{
  struct ws4d_event_delivery *delivery;

  delivery = _ws4d_subsm_geteventdelivery (subsm, Mode);
  if (delivery)
    {
      return _ws4d_subsm_unregister_eventdelivery (delivery);
    }

  return WS4D_ERR;
}

int
ws4d_subsm_unregister_alldelivery (struct ws4d_subsmanager *subsm)
{
  register struct ws4d_event_delivery *delivery, *next;

  ws4d_assert (subsm, WS4D_ERR);

  ws4d_list_foreach (delivery, next, &subsm->supp_delivery_list,
                     struct ws4d_event_delivery, list)
  {
    _ws4d_subsm_unregister_eventdelivery (delivery);
  }

  return WS4D_OK;
}

/* event filter register */

static struct ws4d_event_filter *
_ws4d_subsm_geteventfilter (struct ws4d_subsmanager *subsm, const char *Type)
{
  register struct ws4d_event_filter *filter, *next;

  ws4d_assert (subsm && Type, NULL);

  ws4d_list_foreach (filter, next, &subsm->supp_filter_list,
                     struct ws4d_event_filter, list)
  {
    if (filter->Type == Type)
      {
        return filter;
      }
  }

  ws4d_list_foreach (filter, next, &subsm->supp_filter_list,
                     struct ws4d_event_filter, list)
  {
    if (!strcmp (filter->Type, Type))
      {
        return filter;
      }
  }

  return NULL;
}

int
ws4d_subsm_register_eventfilter (struct ws4d_subsmanager *subsm,
                                 const char *Type,
                                 ws4d_processFilter_cb fprocessFilter,
                                 ws4d_cleanFilter_cb fcleanFilter)
{
  struct ws4d_event_filter *filterType = NULL;

  ws4d_assert (subsm && Type && fprocessFilter && fcleanFilter, WS4D_ERR);

  filterType =
    ws4d_malloc_alist (sizeof (struct ws4d_event_filter),
                       ws4d_subsm_get_alist (subsm));
  if (!filterType)
    return WS4D_EOM;

  filterType->Type = Type;
  filterType->fprocessFilterType = fprocessFilter;
  filterType->fcleanFilterType = fcleanFilter;

  ws4d_list_add (&filterType->list, &subsm->supp_filter_list);

  return WS4D_OK;
}

int
ws4d_subsm_register_defeventfilter (struct ws4d_subsmanager *subsm,
                                    const char *Type,
                                    ws4d_processFilter_cb fprocessFilter,
                                    ws4d_cleanFilter_cb fcleanFilter)
{
  int ret;

  ws4d_assert (subsm, WS4D_ERR);

  ret = ws4d_subsm_register_eventfilter (subsm, Type,
                                         fprocessFilter, fcleanFilter);

  if (ret == WS4D_OK)
    {
      struct ws4d_event_filter *filterType = NULL;

      filterType = _ws4d_subsm_geteventfilter (subsm, Type);
      if (filterType)
        {
          subsm->default_filter = filterType;
        }
      else
        {
          return WS4D_ERR;
        }
    }

  return ret;
}

static int
_ws4d_subsm_unregister_eventfilter (struct ws4d_event_filter *filter)
{
  if (filter)
    ws4d_free_alist (filter);

  return WS4D_OK;
}

int
ws4d_subsm_unregister_eventfilter (struct ws4d_subsmanager *subsm,
                                   const char *Type)
{
  struct ws4d_event_filter *filter;

  filter = _ws4d_subsm_geteventfilter (subsm, Type);
  if (filter)
    {
      return _ws4d_subsm_unregister_eventfilter (filter);
    }

  return WS4D_ERR;
}

int
ws4d_subsm_unregister_allfilter (struct ws4d_subsmanager *subsm)
{
  register struct ws4d_event_filter *filter, *next;

  ws4d_assert (subsm, WS4D_ERR);

  ws4d_list_foreach (filter, next, &subsm->supp_filter_list,
                     struct ws4d_event_filter, list)
  {
    _ws4d_subsm_unregister_eventfilter (filter);
  }

  return WS4D_OK;
}

int
ws4d_subsm_is_delivery (struct ws4d_subscription *subs, const char *Mode)
{
  if (!Mode || !subs || !subs->delivery_mode)
    return 0;

  if ((Mode == subs->delivery_mode->DeliveryMode)
      || (!strcmp (Mode, subs->delivery_mode->DeliveryMode)))
    return 1;
  else
    return 0;
}

int
ws4d_subsm_is_filter (struct ws4d_subscription *subs, const char *Type)
{
  if (!Type || !subs || !subs->filter_mode)
    return 0;

  if ((Type == subs->filter_mode->FilterType)
      || (!strcmp (Type, subs->filter_mode->FilterType)))
    return 1;
  else
    return 0;
}

int
ws4d_subsm_clean_delivery (struct ws4d_subsmanager *subsm,
                           struct ws4d_subscription *subs)
{
  register struct ws4d_event_delivery *deliveryType, *next;

  ws4d_assert (subsm && subs, WS4D_ERR);

  ws4d_list_foreach (deliveryType, next, &subsm->supp_delivery_list,
                     struct ws4d_event_delivery, list)
  {
    if (ws4d_subsm_is_delivery (subs, deliveryType->Mode))
      {
        if (deliveryType->fcleanDelivery)
          {
            return deliveryType->fcleanDelivery (subs);
          }
        else
          {
            return WS4D_OK;
          }
      }
  }

  return WS4D_ERR;
}

int
ws4d_subsm_clean_filter (struct ws4d_subsmanager *subsm,
                         struct ws4d_subscription *subs)
{
  register struct ws4d_event_filter *filterType, *next;

  ws4d_assert (subsm && subs, WS4D_ERR);

  ws4d_list_foreach (filterType, next, &subsm->supp_filter_list,
                     struct ws4d_event_filter, list)
  {
    if (ws4d_subsm_is_filter (subs, filterType->Type))
      {
        if (filterType->fcleanFilterType)
          {
            return filterType->fcleanFilterType (subs);
          }
        else
          {
            return WS4D_OK;
          }
      }
  }

  return WS4D_ERR;
}

int
ws4d_subsm_check_subs (struct ws4d_subsmanager *subsm,
                       struct ws4d_subscription *subs)
{
  int res = 0;
  if (subs)
    {
      res = (ws4d_subs_get_expires (subs) > ws4d_systime_s ());
      if (!res)
        ws4d_subsm_expiresubs (subsm, subs);
    }
  return res;
}

int
ws4d_subsm_check_allsubs (struct ws4d_subsmanager *subsm)
{
  register struct ws4d_subscription *subs, *next;

  ws4d_assert (subsm, WS4D_ERR);

  ws4d_list_foreach (subs, next, &subsm->subs_list, struct ws4d_subscription,
                     list)
  {
    ws4d_subsm_check_subs (subsm, subs);
  }

  return WS4D_OK;
}
