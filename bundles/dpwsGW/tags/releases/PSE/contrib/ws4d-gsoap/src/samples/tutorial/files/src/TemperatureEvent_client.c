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

#include "acs_inv1.nsmap"
#include "dpws_client.h"
#include <signal.h>

#include "ws4d_eprllist.h"

#define USAGE printf("\nusage:\n"); \
  printf("\t%s -i client's IP -d device's UUID\n", orig_argv_0); \
  printf("\tUUID in form of urn:uuid:[uuid]\n"); \
  printf("\tYou might want to use the tool uuidgen to obtain a uuid\n"); \
  printf("Exmaple:\n\t%s -i 192.168.1.1 -d urn:uuid:`uuidgen`\n\n", orig_argv_0);

struct soap client, handler;
struct dpws_s dpws;
int shutdownFlag = 0;

void
client_exit ()
{
  printf
    ("\nTemperatureEvent_client: waiting for mainloop to shut down...\n");
  fflush (NULL);
  shutdownFlag = 1;
}


int
main (int argc, char **argv)
{
#ifndef WIN32
  struct sigaction sa;
#endif

  ws4d_alloc_list alist;

  struct ws4d_epr device, *service = NULL;
  struct ws4d_abs_eprlist services;
  ws4d_qnamelist service_type_list;

  const char *orig_argv_0 = argv[0];
  char *deviceaddr = NULL, *host = NULL, *XAddrs = NULL;
  const char *id = NULL;

  char handler_uri[DPWS_URI_MAX_LEN + 1] = "http://host:0/";

  struct ws4d_delivery_type *delivery = NULL;
  struct ws4d_filter_type *filter = NULL;

  ws4d_time duration = 3600;


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
                  host = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  host = argv[1];
                }
#ifdef DEBUG
              printf ("\nTemperatureEvent_client: Set interface to \"%s\"\n",
                      host);
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
              printf
                ("\nTemperatureEvent_client: Set device address to \"%s\"\n",
                 deviceaddr);
#endif
              break;
            default:
              fprintf (stderr, "\nTemperatureEvent_client: Bad option %s",
                       argv[1]);
              USAGE exit (1);
            }
        }
      --argc;
      ++argv;
    }

  if (deviceaddr == NULL)
    {
      fprintf (stderr,
               "\nTemperatureEvent_client: No device id or address was specified!\n");
      USAGE exit (1);
    }

  if (host == NULL)
    {
      fprintf (stderr,
               "\nTemperatureEvent_client: No interface address was specified!\n");
      USAGE exit (1);
    }

  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize client soap handle */
  soap_init (&client);
#ifdef DEBUG
  soap_omode (&client, SOAP_XML_INDENT);
#endif
  soap_set_namespaces (&client, acs_inv1_namespaces);

  soap_init (&handler);
#ifdef DEBUG
  soap_omode (&handler, SOAP_XML_INDENT);
#endif
  soap_set_namespaces (&handler, acs_inv1_namespaces);

  /* initialize WS4D-gSOAP */
  if (dpws_init (&dpws, host) != SOAP_OK)
    {
      fprintf (stderr,
               "\nTemperatureEventListener: Could not initialize dpws handle\n");
      fprintf (stderr, "something seems to be wrong with ip address\n");
      USAGE dpws_done (&dpws);
      exit (1);
    }

  /* bind listener handle to an address */
  dpws_handle_init (&dpws, &handler);
  if (dpws_handle_bind (&dpws, &handler, handler_uri, DPWS_URI_MAX_LEN, 100)
      == SOAP_INVALID_SOCKET)
    {
      fprintf (stderr,
               "\nTemperatureEventListener: error calling dpws_handle_bind");
      USAGE exit (1);
    }

  /* allocate and prepare device to resolve */
  ws4d_epr_init (&device);
  ws4d_epr_set_Addrs (&device, deviceaddr);

  /* resolve address */
  XAddrs = (char *) dpws_resolve_addr (&dpws, &device, NULL, 10000);
  if (XAddrs != NULL)
    {
#ifdef DEBUG
      printf ("\nTemperatureEventListener: Device %s found at addr %s\n",
              ws4d_epr_get_Addrs (&device), XAddrs);
#endif
    }
  else
    {
      fprintf (stderr,
               "\nTemperatureEventListener: Device %s cannot be found\n",
               ws4d_epr_get_Addrs (&device));
      USAGE exit (1);
    }

  /* prepare service type list */
  ws4d_qnamelist_init (&service_type_list);
  ws4d_qnamelist_addstring
    ("\"http://www.ws4d.org/axis2/tutorial/AirConditioner\":AirConditionerInterface",
     &service_type_list, &alist);

  /* look up service with matching service types on device */
  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  if (dpws_find_services (&dpws, &device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      /* use first service */
      service = ws4d_eprlist_get_first (&services);
#ifdef DEBUG
      printf
        ("\nTemperatureEventListener: Device offers AirConditionerService at %s\n",
         ws4d_epr_get_Addrs (service));
#endif
    }
  else
    {
      fprintf (stderr,
               "\nTemperatureEventListener: AirConditionerService not found on %s\n",
               ws4d_epr_get_Addrs (&device));
      exit (1);
    }

  /* prepare filter and delivery mode */
  delivery =
    dpws_gen_delivery_push (&client, dpws_handle_get_paddr (&handler));

  filter =
    dpws_gen_filter_action (&client,
                            "http://www.ws4d.org/axis2/tutorial/AirConditioner/TemperatureEventOut");


  /* Subscribing to Airconditioner Service */
#ifdef DEBUG
  printf
    ("\nTemperatureEventListener: Subscribe to Airconditioner Service ... ");
#endif
  id = dpws_subscribe (&client, service, NULL, &duration, delivery, filter);
  if (!id)
    {
      soap_print_fault (&client, stderr);
      exit (1);
    }

#ifdef DEBUG
  printf ("OK (%s %s)\n", ws4d_subsproxy_getsubsman (service, id), id);
#endif

  /* install signal handler for SIGINT or Ctrl-C */
#ifdef WIN32
  signal (SIGINT, client_exit);
#else
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = client_exit;
  sigaction (SIGINT, &sa, NULL);
#endif

  /* set accept timeout to one second */
  handler.accept_timeout = 1;

#ifdef DEBUG
  printf
    ("\nTemperatureEventListener: ready to receive events ... (Ctrl-C for shut down)\n");
#endif

  while (!shutdownFlag)
    {

#ifdef DEBUG
      printf ("\nTemperatureEventListener: waiting for events\n");
#endif

      if (soap_accept (&handler) != SOAP_INVALID_SOCKET)
        {

#ifdef DEBUG
          printf ("\nTemperatureEventListener: processing event from %s:%d\n",
                  inet_ntoa (handler.peer.sin_addr),
                  ntohs (handler.peer.sin_port));
#endif

          if (acs_inv1_serve (&handler))
            {
              soap_print_fault (&handler, stderr);
            }

          /* clean up soaps internaly allocated memory */
          soap_end (&handler);
        }
    }

  printf ("\nTemperatureEventListener: shutting down...");

  if (dpws_subs_unsubscribe (&client, service, id) != SOAP_OK)
    {
      soap_print_fault (&client, stderr);
      exit (1);
    }

  soap_end (&client);
  soap_done (&client);
  soap_end (&handler);
  soap_done (&handler);
  dpws_done (&dpws);

  ws4d_eprlist_done (&services);
  ws4d_epr_done (&device);

  ws4d_alloclist_done (&alist);

  printf ("done\n");

  exit (0);
}


int
__acsinv1__TemperatureEvent (struct soap *soap,
                             struct acsinv1__ACStateType *acsinv1__ACState)
{
  printf ("\nGot TemperatureEvent: ");
  if (acsinv1__ACState)
    {
      printf ("\n\tCurrentTemp: %d\n\tTargetTemp: %d\n",
              acsinv1__ACState->CurrentTemp, acsinv1__ACState->TargetTemp);
    }
  else
    {
      printf ("no ACState transmitted!\n");
    }

  return SOAP_OK;
}
