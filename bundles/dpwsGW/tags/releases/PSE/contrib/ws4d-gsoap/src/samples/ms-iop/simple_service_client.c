/* Steffen Prüter - MSIOP Testcases 2.1.1 & 2.2.1
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

#include "sis.nsmap"
#include "dpws_client.h"

#include "ws4d_eprllist.h"
#include "ws4d_service.h"
#include "simple_service_client.h"

/* MSIOP Testcase 2.1.1 */

int
call_simpleservice_oneway_method (struct dpws_s *dpws, const char *uuid,
                                  const char *serviceid)
{
  struct soap client;
  ws4d_alloc_list alist;
  int test_err = 0;

  struct ws4d_epr *device = NULL, *service = NULL;

  struct ws4d_abs_eprlist services;

  ws4d_qnamelist service_type_list;

  char *XAddrs = NULL;

  struct sis1__OneWayType sis1__OneWay;
  struct __sis1__OneWay _param_1;

  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);


  /* allocate and prepare device to resolve */
  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, uuid);

  XAddrs = (char *) dpws_resolve_addr (dpws, device, NULL, 10000);
  if (XAddrs != NULL)
    {
      fprintf (stderr, "\nDevice %s found at addr %s\n",
               ws4d_epr_get_Addrs (device), XAddrs);
    }
  else
    {
      fprintf (stderr, "\nDevice %s cannot be found\n",
               ws4d_epr_get_Addrs (device));
      fflush (NULL);
      return 0;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (dpws, device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), serviceid))
          {
            service = epr;
            printf ("\nDevice offers SimpleService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
      if (!service)
        {
          printf ("\nSimpleService not found on %s\n",
                  ws4d_epr_get_Addrs (device));
          return 0;
        }
    }
  else
    {
      printf ("\nSimpleService not found on %s\n",
              ws4d_epr_get_Addrs (device));
      return 0;
    }

  soap_set_namespaces (&client, sis_namespaces);
  dpws_header_gen_oneway (&client, NULL,
                          ws4d_epr_get_Addrs (service),
                          "http://schemas.example.org/SimpleService/OneWay",
                          NULL, sizeof (struct SOAP_ENV__Header));

  sis1__OneWay.Param = 1;
  sis1__OneWay.__size = 0;
  sis1__OneWay.__any = NULL;
  sis1__OneWay.__anyAttribute = NULL;

  if (soap_send___sis1__OneWay
      (&client, ws4d_epr_get_Addrs (service), NULL, &sis1__OneWay) == SOAP_OK)
    {
      printf ("\nSuccessfully called OneWay-operation on %s with param %d\n",
              ws4d_epr_get_Addrs (service), sis1__OneWay.Param);
    }
  else
    {
      printf ("\nError: calling OneWay-operation on %s\n",
              ws4d_epr_get_Addrs (service));
      test_err = 1;
    }

  if (soap_recv___sis1__OneWay (&client, &_param_1) != 202)
    {
      printf ("\nError: got not response from %s\n",
              ws4d_epr_get_Addrs (service));
      test_err = 1;
    }
  soap_end (&client);
  soap_done (&client);

  ws4d_eprlist_done (&services);
  ws4d_epr_free (1, device);

  ws4d_alloclist_done (&alist);

  if (test_err)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

/* MSIOP Testcase 2.2.1 */

int
call_simpleservice_twoway_method (struct dpws_s *dpws, const char *uuid,
                                  const char *serviceid)
{
  struct soap client;
  ws4d_alloc_list alist;
  int test_err = 0;

  struct ws4d_epr *device = NULL, *service = NULL;

  struct ws4d_abs_eprlist services;

  ws4d_qnamelist service_type_list;

  char *XAddrs = NULL;

  struct sis1__TwoWayType sis1__TwoWayRequest;
  struct sis1__TwoWayResponseType sis1__TwoWayResponse;

  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);


  /* allocate and prepare device to resolve */
  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, uuid);

  XAddrs = (char *) dpws_resolve_addr (dpws, device, NULL, 10000);
  if (XAddrs != NULL)
    {
      fprintf (stderr, "\nDevice %s found at addr %s\n",
               ws4d_epr_get_Addrs (device), XAddrs);
    }
  else
    {
      fprintf (stderr, "\nDevice %s cannot be found\n",
               ws4d_epr_get_Addrs (device));
      fflush (NULL);
      return 0;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (dpws, device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), serviceid))
          {
            service = epr;
            printf ("\nDevice offers SimpleService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
      if (!service)
        {
          printf ("\nSimpleService not found on %s\n",
                  ws4d_epr_get_Addrs (device));
          return 0;
        }
    }
  else
    {
      printf ("\nSimpleService not found on %s\n",
              ws4d_epr_get_Addrs (device));
      return 0;
    }

  sis1__TwoWayRequest.X = 1;
  sis1__TwoWayRequest.Y = 2;
  sis1__TwoWayRequest.__size = 0;
  sis1__TwoWayRequest.__any = NULL;
  sis1__TwoWayRequest.__anyAttribute = NULL;

  soap_set_namespaces (&client, sis_namespaces);
  dpws_header_gen_request (&client, NULL,
                           ws4d_epr_get_Addrs (service),
                           "http://schemas.example.org/SimpleService/TwoWayRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

  if (soap_call___sis1__TwoWay
      (&client, ws4d_epr_get_Addrs (service), NULL,
       &sis1__TwoWayRequest, &sis1__TwoWayResponse) == SOAP_OK)
    {
      printf
        ("\nSuccessfully called TwoWay-operation on %s with param X=%d, Y=%d,",
         ws4d_epr_get_Addrs (service),
         sis1__TwoWayRequest.X, sis1__TwoWayRequest.Y);
      printf (" Sum is %d\n", sis1__TwoWayResponse.Sum);
    }
  else
    {
      printf ("\nError: calling TwoWay-operation on %s\n",
              ws4d_epr_get_Addrs (service));
      test_err = 1;
    }

  soap_end (&client);
  soap_done (&client);

  ws4d_eprlist_done (&services);
  ws4d_epr_free (1, device);

  ws4d_alloclist_done (&alist);

  if (test_err)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}
