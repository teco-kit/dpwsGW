/* <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2007  University of Rostock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#include "evs.nsmap"
#include "dpws_device.h"
#include "msiop_metadata.h"
#include "iop_services.h"

#define EVENTSERVICE_URI_LEN 255

int
setup_EventingService (struct dpws_s *device, struct soap *handle,
                       const char *wsdl, int backlog)
{
  soap_set_namespaces (handle, evs_namespaces);

  return msiop_setup_EventingService (device, handle, wsdl, backlog);
}

int
fire_simple_event (struct dpws_s *dpws)
{
  struct soap soap;
  struct ws4d_subscription *subs, *next;
  struct evs1__SimpleEventType event;

  memset (&event, 0, sizeof (struct evs1__SimpleEventType));

  soap_init (&soap);
  soap_set_namespaces (&soap, evs_namespaces);

  dpws_for_each_subs (subs, next, dpws,
                      "http://schemas.example.org/EventingService/SimpleEvent")
  {
    struct __evs1__SimpleEvent response;
    char *deliverto = dpws_subsm_get_deliveryPush_address (dpws, subs);

    if (!deliverto)
      continue;

    dpws_header_gen_oneway (&soap, NULL, deliverto,
                            "http://schemas.example.org/EventingService/SimpleEvent",
                            NULL, sizeof (struct SOAP_ENV__Header));

    printf ("Sending Event to %s\n", deliverto);

    if (soap_send___evs1__SimpleEvent (&soap, deliverto, NULL, &event))
      {
        soap_print_fault (&soap, stderr);
      }
    else
      {
        soap_recv___evs1__SimpleEvent (&soap, &response);
      }

    soap_end (&soap);
  }

  soap_done (&soap);

  return WS4D_OK;
}

int
fire_integer_event (struct dpws_s *dpws, int integer)
{
  struct soap soap;
  struct ws4d_subscription *subs, *next;
  struct evs1__IntegerEventType event;

  memset (&event, 0, sizeof (struct evs1__IntegerEventType));
  event.Param = integer;

  soap_init (&soap);
  soap_set_namespaces (&soap, evs_namespaces);

  dpws_for_each_subs (subs, next, dpws,
                      "http://schemas.example.org/EventingService/IntegerEvent")
  {
    struct __evs1__IntegerEvent response;
    char *deliverto = dpws_subsm_get_deliveryPush_address (dpws, subs);

    if (!deliverto)
      continue;

    dpws_header_gen_oneway (&soap, NULL, deliverto,
                            "http://schemas.example.org/EventingService/IntegerEvent",
                            NULL, sizeof (struct SOAP_ENV__Header));

    printf ("Sending Event to %s\n", deliverto);

    if (soap_send___evs1__IntegerEvent (&soap, deliverto, NULL, &event))
      {
        soap_print_fault (&soap, stderr);
      }
    else
      {
        soap_recv___evs1__IntegerEvent (&soap, &response);
      }

    soap_end (&soap);
  }

  soap_done (&soap);

  return WS4D_OK;
}
