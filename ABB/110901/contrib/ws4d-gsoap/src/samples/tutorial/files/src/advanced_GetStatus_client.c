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

#include "acs1.nsmap"
#include "dpws_client.h"

#include "ws4d_eprllist.h"

#define USAGE printf("\nusage:\n"); \
  printf("\t%s -i client's IP \n", orig_argv_0); \
  printf("Exmaple:\n\t%s -i 192.168.1.1\n\n", orig_argv_0);

struct soap client;
struct dpws_s dpws;

#define ACS_MAX_DEVICES 20

int
main (int argc, char **argv)
{
  ws4d_alloc_list alist;

  struct acs1__ACStateType ACState;

  struct ws4d_epr *device = NULL, *iter = NULL;
  struct ws4d_abs_eprlist devices, services;
  int ret = 0;

  const char *orig_argv_0 = argv[0];
  char *deviceaddr = NULL, *host = NULL, *XAddrs = NULL;

  /* parsing command line options */
  while (argc > 1)
    {
      if (argv[1][0] == '-')
        {
          char *option = &argv[1][1];
          switch (option[0])
            {
            case 'i':
              if (strlen (option) > 2)
                {
                  ++option;
                  host = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  host = argv[1];
                }
              break;
            default:
              fprintf (stderr, "\nsimple_client: Bad option %s\n", argv[1]);
              USAGE exit (1);
            }
        }
      --argc;
      ++argv;
    }

  if (host == NULL)
    {
      printf ("\nNo host was specified!\n");
      fflush (NULL);
      USAGE exit (1);
    }

  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize client soap handle */
  soap_init (&client);
#ifdef DEBUG
  soap_omode (&client, SOAP_XML_INDENT);
#endif

  /* initialize WS4D-gSOAP */
  if (dpws_init (&dpws, host) != SOAP_OK)
    {
      fprintf (stderr, "\n%s: Could not initialize dpws handle\n", argv[0]);
      USAGE dpws_done (&dpws);
      exit (1);
    }

  /* initialize device list */
  ws4d_eprlist_init (&devices, ws4d_eprllist_init, NULL);

  /* probe for all airconditioner devices */
  ws4d_qnamelist type_list;

  /* init type list */
  ws4d_qnamelist_init (&type_list);
  ws4d_qnamelist_addstring
    ("\"http://www.ws4d.org/axis2/tutorial/AirConditioner\":AirConditioner",
     &type_list, &alist);

#ifdef DEBUG
  printf ("\nProbing for acs devices for 10 seconds ... ");
  fflush (NULL);
#endif
  ret = dpws_probe (&dpws, &type_list, NULL, 10000, 100, NULL,
                    NULL, &devices);
  if (ret != WS4D_OK)
    {
      if (ret == WS4D_TO)
        {
          fprintf (stderr, "No devices found!\n");
        }
      else
        {
          fprintf (stderr, "Could not probe for devices!\n");
        }
      USAGE dpws_done (&dpws);
      exit (1);
    }
#ifdef DEBUG
  printf ("done\n");
  fflush (NULL);
#endif

  /* look for airconditioner service at every device found before */
  ws4d_eprlist_foreach (device, iter, &devices)
  {
    if (ws4d_epr_isvalid (device))
      {
        struct ws4d_epr *service = NULL;
        ws4d_qnamelist service_type_list;

        /* resolve address */
        XAddrs = (char *) dpws_resolve_addr (&dpws, device, NULL, 10000);
        if (XAddrs != NULL)
          {
            fprintf (stderr, "\n\nDevice %s found at addr %s\n",
                     ws4d_epr_get_Addrs (device), XAddrs);
          }
        else
          {
            fprintf (stderr, "\nDevice %s cannot be found\n",
                     ws4d_epr_get_Addrs (device));
            fflush (NULL);
            continue;
          }

        /* prepare service type list */
        ws4d_qnamelist_init (&service_type_list);
        ws4d_qnamelist_addstring
          ("\"http://www.ws4d.org/axis2/tutorial/AirConditioner\":AirConditionerInterface",
           &service_type_list, &alist);

        /* look up service with matching service types on device */
        ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
        if (dpws_find_services (&dpws, device, &service_type_list,
                                10000, &services) == SOAP_OK)
          {
            service = ws4d_eprlist_get_first (&services);
            printf ("\nDevice offers AirConditionerService at %s\n",
                    ws4d_epr_get_Addrs (service));
          }
        else
          {
#ifdef DEBUG
            fprintf (stderr,
                     "\nsimple_client: AirConditionerService not found on %s\n",
                     ws4d_epr_get_Addrs (device));
#endif
            continue;
          }

        /* prepare soap handel to use service */
        soap_set_namespaces (&client, acs1_namespaces);
        dpws_header_gen_request (&client, NULL,
                                 ws4d_epr_get_Addrs (service),
                                 "http://www.ws4d.org/axis2/tutorial/AirConditioner/GetStatusIn",
                                 NULL, NULL,
                                 sizeof (struct SOAP_ENV__Header));

        /* call GetStatus operation */
        if (soap_call___acs1__GetStatus (&client,
                                         ws4d_epr_get_Addrs
                                         (service), NULL, NULL,
                                         &ACState) == SOAP_OK)
          {
            printf ("\n%s - %s\n\tCurrentTemp: %d\n\tTargetTemp: %d\n",
                    ws4d_epr_get_Addrs (device), ws4d_epr_get_Addrs (service),
                    ACState.CurrentTemp, ACState.TargetTemp);
          }
        else
          {
            fprintf (stderr,
                     "\nsimple_client: error calling GetStatus on %s\n",
                     ws4d_epr_get_Addrs (service));
          }

        /* clean up */
        ws4d_eprlist_done (&services);
        soap_end (&client);

      }
  }

  /* clean up */
  soap_done (&client);
  dpws_done (&dpws);

  ws4d_eprlist_done (&devices);

  ws4d_alloclist_done (&alist);

  exit (0);
}
