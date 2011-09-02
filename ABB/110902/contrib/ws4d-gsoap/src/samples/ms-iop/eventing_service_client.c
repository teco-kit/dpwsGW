/* Elmar Zeeb - MSIOP Testcase 2.1.1 & 2.2.1
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
#include "dpws_client.h"

#include "ws4d_eprllist.h"
#include "ws4d_service.h"
#include "eventing_service_client.h"

const char *
do_subscribe (struct ws4d_epr *service, const char *actionfilter,
              const char *deliverto, ws4d_time duration)
{
  struct soap client;
  struct ws4d_delivery_type *delivery = NULL;
  struct ws4d_filter_type *filter = NULL;
  const char *subsid = NULL;

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);

  delivery = dpws_gen_delivery_push (&client, deliverto);
  filter = dpws_gen_filter_action (&client, actionfilter);

  subsid =
    dpws_subscribe (&client, service, NULL, &duration, delivery, filter);
  if (subsid)
    {
      printf ("Got subscription ID: %s\n", subsid);
    }
  else
    {
      soap_print_fault (&client, stderr);
    }

  soap_end (&client);
  soap_done (&client);

  return subsid;
}

int
do_renew (struct ws4d_epr *service, const char *id, ws4d_time duration)
{
  struct soap client;
  int ret = 0;

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);

  if (dpws_subs_renew (&client, service, id, &duration) == WS4D_OK)
    {
      ret = 1;
    }
  else
    {
      soap_print_fault (&client, stderr);
      ret = 0;
    }

  soap_end (&client);
  soap_done (&client);

  return ret;
}

int
do_unsubscribe (struct ws4d_epr *service, const char *id)
{
  struct soap client;
  int ret = 0;

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);

  if (dpws_subs_unsubscribe (&client, service, id) == WS4D_OK)
    {
      ret = 1;
    }
  else
    {
      soap_print_fault (&client, stderr);
      ret = 0;
    }

  soap_end (&client);
  soap_done (&client);

  return ret;
}

static char handler_uri[DPWS_URI_MAX_LEN + 1] = "http://host:0/";

int
event_handler_init (struct dpws_s *dpws, struct soap *soap)
{
  soap_init (soap);
  soap_omode (soap, SOAP_XML_INDENT);
  soap_set_namespaces (soap, evs_namespaces);

  /* bind listener handle to an address */
  dpws_handle_init (dpws, soap);
  if (dpws_handle_bind (dpws, soap, handler_uri, DPWS_URI_MAX_LEN, 100)
      == SOAP_INVALID_SOCKET)
    {
      fprintf (stderr,
               "\nTemperatureEventListener: error calling dpws_handle_bind");
      fflush (NULL);
      exit (1);
    }

  return WS4D_OK;
}

const char *
event_handler_geturi (void)
{
  return handler_uri;
}

int
__evs1__SimpleEvent (struct soap *soap,
                     struct evs1__SimpleEventType *evs1__SimpleEvent)
{
  WS4D_UNUSED_PARAM (soap);
  WS4D_UNUSED_PARAM (evs1__SimpleEvent);

  printf ("\n\nGot SimpleEvent\n");

  return SOAP_OK;
}

int
__evs1__IntegerEvent (struct soap *soap,
                      struct evs1__IntegerEventType *evs1__IntegerEvent)
{
  WS4D_UNUSED_PARAM (soap);
  WS4D_UNUSED_PARAM (evs1__IntegerEvent);

  printf ("\n\nGot IntegerEvent\n");

  return SOAP_OK;
}
