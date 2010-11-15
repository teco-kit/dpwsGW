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
#include "dpws_device.h"
#include "acs_metadata.h"
#include "acs_wsdl.h"
#include "config.h"
#include <signal.h>

#include "ws4d_scopelist.h"

struct soap service;
struct dpws_s device;

#define USAGE printf("\nusage:\n"); \
			  printf("\t%s -i device's IP -u device's UUID\n", orig_argv_0); \
			  printf("\tUUID in form of urn:uuid:[uuid]\n"); \
			  printf("\tYou might want to use the tool uuidgen to obtain a uuid\n"); \
			  printf("Exmaple:\n\t%s -i 192.168.1.1 -u urn:uuid:`uuidgen`\n\n", orig_argv_0);

void
service_exit ()
{
#ifdef DEBUG
  printf ("\nAirconditioner: shutting down...\n");
#endif

  dpws_deactivate_hosting_service (&device);
  soap_done (&service);
  dpws_done (&device);

  exit (0);
}

int
main (int argc, char **argv)
{
#ifndef WIN32
  struct sigaction sa;
#endif
  const char *orig_argv_0 = argv[0];
  const char *interf = NULL;
  const char *uuid = NULL;
  const char *scope = NULL;
  struct ws4d_list_node scopelist;
  ws4d_alloc_list alist;

  WS4D_ALLOCLIST_INIT (&alist);
  ws4d_sl_init (&scopelist);

  /* parsing command line options */
  while (argc > 1)
    {
      if (argv[1][0] == '-')
        {
          char *option = &argv[1][1];

          switch (option[0])
            {
            case 'i':          /* set interf with option -i */
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
              printf ("\nAirconditioner: Set interface to \"%s\"\n", interf);
#endif
              break;
            case 'u':          /* set id with option -u */
              if (strlen (option) > 2)
                {
                  ++option;
                  uuid = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  uuid = argv[1];
                }
#ifdef DEBUG
              printf ("\nAirconditioner: Set uuid to \"%s\"\n", uuid);
#endif
              break;
            case 's':
              if (strlen (option) > 2)
                {
                  ++option;
                  scope = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  scope = argv[1];
                }
#ifdef DEBUG
              printf ("\nAirconditioner: Adding scope \"%s\"\n", scope);
#endif
              ws4d_sl_add (&scopelist, scope, &alist);
              break;
            default:
              fprintf (stderr, "\nAirconditioner: Bad option %s\n", argv[1]);
              USAGE exit (1);
            }
        }
      --argc;
      ++argv;
    }

  if (interf == NULL)
    {
      fprintf (stderr,
               "\nAirconditioner: No interface address was specified!\n");
      fprintf (stderr, "Expecting device's IP address\n");
      USAGE exit (1);
    }

  /* initialize soap handle */
  soap_init (&service);
#ifdef DEBUG
  soap_omode (&service, SOAP_XML_INDENT);
#endif
  soap_set_namespaces (&service, acs1_namespaces);

  /* initialize device and services */
  if (dpws_init (&device, interf))
    {
      fprintf (stderr,
               "Can't init device. Something wrong with interface address\n");
      USAGE dpws_done (&device);
      exit (1);
    }
  if (acs_setup_HostingService (&device, &service, uuid, 100))
    {
      fprintf (stderr,
               "Can't init service. Something wrong with interface uuid\n");
      USAGE dpws_done (&device);
      exit (1);
    }
  if (acs_setup_AirConditioner (&device, &service, AIRCONDITIONER_WSDL, 100))
    {
      fprintf (stderr, "\nAirconditioner: Can't init device and services\n");
      USAGE dpws_done (&device);
      exit (1);
    }

  /* Set Metadata */
  acs_set_Metadata (&device);
  acs_set_wsdl (&device);

  /* Set Scopes */
  if (!ws4d_sl_isempty (&scopelist))
    {
      dpws_add_scope (&device, ws4d_sl_tostr (&scopelist, &alist));
    }

  /* Update Metadata */
  if (dpws_update_Metadata (&device))
    {
      fprintf (stderr, "\nAirconditioner: Can't init metadata\n");
      dpws_done (&device);
      exit (1);
    }

  /* install signal handler for SIGINT or Ctrl-C */
#ifdef WIN32
  signal (SIGINT, service_exit);
#else
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = service_exit;
  sigaction (SIGINT, &sa, NULL);
#endif

  /* Tell hosting service to start advertising its hosted services */
  if (dpws_activate_hosting_service (&device))
    {
      fprintf (stderr, "\nAirconditioner: Can't activate device\n");
      dpws_done (&device);
      exit (1);
    }

  ws4d_sl_done (&scopelist);
  ws4d_alloclist_done (&alist);

#ifdef DEBUG
  printf ("\nAirconditioner: ready to serve... (Ctrl-C for shut down)\n");
#endif

  for (;;)
    {
      struct soap *handle = NULL, *soap_set[] = SOAP_HANDLE_SET (&service);
      int (*serve_requests[]) (struct soap * soap) =
        SOAP_SERVE_SET (acs1_serve_request);

#ifdef DEBUG
      printf ("\nAirconditioner: waiting for request\n");
#endif

      /* waiting for new messages */
      handle = dpws_maccept (&device, 100000, 1, soap_set);

      if (handle)
        {

#ifdef DEBUG
          printf ("\nAirconditioner: processing request from %s:%d\n",
                  inet_ntoa (handle->peer.sin_addr),
                  ntohs (handle->peer.sin_port));
#endif

          /* dispatch messages */
          if (dpws_mserve (handle, 1, serve_requests))
            {
              soap_print_fault (handle, stderr);
            }

          /* clean up soaps internaly allocated memory */
          soap_end (handle);
        }
    }

  return -1;
}

int temp = 23;

int
__acs1__GetStatus (struct soap *soap,
                   struct _acs1__EmptyMessage *acs1__GetStatus,
                   struct acs1__ACStateType *acs1__ACState)
{
  /* fill response message */
  acs1__ACState->CurrentTemp = temp;
  acs1__ACState->TargetTemp = temp;

  /* create response header */
  return dpws_header_gen_response (soap, NULL, wsa_header_get_ReplyTo (soap),
                                   "http://www.ws4d.org/axis2/tutorial/AirConditioner/GetStatusOut",
                                   wsa_header_get_MessageId (soap),
                                   sizeof (struct SOAP_ENV__Header));
}


int
__acs1__SetTargetTemperature (struct soap *soap,
                              int acs1__TargetTemperature,
                              struct acs1__ACStateType *acs1__ACState)
{
  /* process request message */
  temp = acs1__TargetTemperature;

  /* fill response message */
  acs1__ACState->CurrentTemp = temp;
  acs1__ACState->TargetTemp = temp;

  /* create response header */
  return dpws_header_gen_response (soap, NULL, wsa_header_get_ReplyTo (soap),
                                   "http://www.ws4d.org/axis2/tutorial/AirConditioner/SetTargetTemperatureOut",
                                   wsa_header_get_MessageId (soap),
                                   sizeof (struct SOAP_ENV__Header));
}
