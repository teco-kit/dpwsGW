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

#define USAGE printf("\nusage:\n"); \
  printf("\t%s -i client's IP -d device's UUID -t temperature\n", orig_argv_0); \
  printf("\tUUID in form of urn:uuid:[uuid]\n"); \
  printf("\tYou might want to use the tool uuidgen to obtain a uuid\n"); \
  printf("Exmaple:\n\t%s -i 192.168.1.1 -d urn:uuid:`uuidgen` -t 27\n\n", orig_argv_0);

#include "acs1.nsmap"
#include "dpws_client.h"

#include "ws4d_eprllist.h"

struct soap client;
struct dpws_s dpws;

#define ACS_NO_TEMPERATURE -1

int
main (int argc, char **argv)
{
  ws4d_alloc_list alist;

  struct acs1__ACStateType ACState;

  struct ws4d_epr device, *service = NULL;
  struct ws4d_abs_eprlist services;
  ws4d_qnamelist service_type_list;

  const char *orig_argv_0 = argv[0];
  char *deviceaddr = NULL;
  char *interf = NULL;
  char *XAddrs = NULL;
  int TargetTemperature = ACS_NO_TEMPERATURE;

  /* parsing command line options */
  while (argc > 1)
    {
      if (argv[1][0] == '-')
        {
          char *option = &argv[1][1];
          switch (option[0])
            {
            case 'i':          /* set interface with option -i */
              if (strlen (option) > 2)
                {
                  ++option;
                  interf = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  interf = argv[1];
                }
#ifdef DEBUG
              printf ("\nsimple_client: Set interface to \"%s\"\n", interf);
#endif
              break;
            case 'd':          /* set device address with option -d */
              if (strlen (option) > 2)
                {
                  ++option;
                  deviceaddr = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  deviceaddr = argv[1];
                }
#ifdef DEBUG
              printf ("\nsimple_client: Set device address to \"%s\"\n",
                      deviceaddr);
#endif
              break;
            case 't':
              if (strlen (option) > 2)
                {
                  ++option;
                  TargetTemperature = atoi (option);
                }
              else
                {
                  --argc;
                  ++argv;
                  TargetTemperature = atoi (argv[1]);
                }
#ifdef DEBUG
              printf ("\nsimple_client: Set target temperature to \"%d\"\n",
                      TargetTemperature);
#endif
              break;
            default:
              fprintf (stderr, "\nsimple_client: Bad option %s\n", argv[1]);
              USAGE exit (1);
            }
        }
      --argc;
      ++argv;
    }

  if (TargetTemperature == ACS_NO_TEMPERATURE)
    {
      fprintf (stderr,
               "\nsimple_client: No target temperature was specified!\n");
      USAGE exit (1);
    }

  if (interf == NULL)
    {
      fprintf (stderr,
               "\nsimple_client: No interface address was specified!\n");
      USAGE exit (1);
    }

  if (deviceaddr == NULL)
    {
      fprintf (stderr, "\nsimple_client: No device address was specified!\n");
      USAGE exit (1);
    }

  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize soap handle */
  soap_init (&client);
#ifdef DEBUG
  soap_omode (&client, SOAP_XML_INDENT);
#endif

  /* initialize WS4D-gSOAP */
  if (dpws_init (&dpws, interf) != SOAP_OK)
    {
      fprintf (stderr, "\nsimple_client: could not initialize dpws handle\n");
      fprintf (stderr,
               "looks like something is wrong with interface address\n");
      USAGE dpws_done (&dpws);
      exit (1);
    }

  /* prepare device epr to resolve */
  ws4d_epr_init (&device);
  ws4d_epr_set_Addrs (&device, deviceaddr);

  /* resolve address */
  XAddrs = (char *) dpws_resolve_addr (&dpws, &device, NULL, 10000);
  if (XAddrs != NULL)
    {
#ifdef DEBUG
      printf ("\nsimple_client: device %s found at addr %s\n",
              ws4d_epr_get_Addrs (&device), XAddrs);
#endif
    }
  else
    {
      fprintf (stderr, "\nsimple_client: device %s cannot be found\n",
               ws4d_epr_get_Addrs (&device));
      fprintf (stderr, "maybe wrong uuid?\n");
      USAGE exit (1);
    }

  /* prepare service type list */
  ws4d_qnamelist_init (&service_type_list);
  ws4d_qnamelist_addstring
    ("\"http://www.ws4d.org/axis2/tutorial/AirConditioner\":AirConditionerInterface",
     &service_type_list, &alist);

  /* look up service with matching service types on device */
  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  if (dpws_find_services
      (&dpws, &device, &service_type_list, 10000, &services) == SOAP_OK)
    {
      /* use first service */
      service = ws4d_eprlist_get_first (&services);
#ifdef DEBUG
      printf ("\nsimple_client: device offers AirConditionerService at %s\n",
              ws4d_epr_get_Addrs (service));
#endif
    }
  else
    {
      fprintf (stderr,
               "\nsimple_client: AirConditionerService not found on %s\n",
               ws4d_epr_get_Addrs (&device));
      USAGE exit (1);
    }

  /* prepare soap handel to use service */
  soap_set_namespaces (&client, acs1_namespaces);
  dpws_header_gen_request (&client, NULL, ws4d_epr_get_Addrs (service),
                           "http://www.ws4d.org/axis2/tutorial/AirConditioner/SetTargetTemperatureIn",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

  /* call SetTargetTemperature operation */
  if (soap_call___acs1__SetTargetTemperature
      (&client, ws4d_epr_get_Addrs (service), NULL, TargetTemperature,
       &ACState) == SOAP_OK)
    {
#ifdef DEBUG
      printf ("\nsimple_client: Successfully set TargetTemperature on %s\n",
              ws4d_epr_get_Addrs (service));
#endif

      printf ("\nCurrentTemp: %d\nTargetTemp: %d\n", ACState.CurrentTemp,
              ACState.TargetTemp);
    }
  else
    {
      fprintf (stderr,
               "\nsimple_client: Error calling SetTargetTemperature on %s\n",
               ws4d_epr_get_Addrs (service));
    }

  /* clean up */
  soap_end (&client);
  soap_done (&client);
  dpws_done (&dpws);

  ws4d_eprlist_done (&services);
  ws4d_epr_done (&device);

  ws4d_alloclist_done (&alist);

  exit (0);
}
