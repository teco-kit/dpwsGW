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

#define WITH_MUTEXES

#include "acc1.nsmap"
#include "dpws_peer.h"
#include "acc_metadata.h"
#include "acc_wsdl.h"
#include "config.h"
#include "discovery_helper.h"
#include <signal.h>

struct soap service;
struct dpws_s device;

int
newdevice (struct ws4d_epr *device)
{
  printf ("\nNew Device %s", ws4d_epr_get_Addrs (device));
  fflush (NULL);

  return WS4D_OK;
}

int
invalidatedevice (struct ws4d_epr *device)
{
  printf ("\nRemoved Device %s", ws4d_epr_get_Addrs (device));
  fflush (NULL);

  return WS4D_OK;
}

void
service_exit ()
{
  printf ("\nAirconditioner: shutting down...\n");

  discovery_helper_stop ();

  dpws_deactivate_hosting_service (&device);
  soap_done (&service);
  dpws_done (&device);

  exit (0);
}

int acc1_serve_request (struct soap *soap);

int
main (int argc, char **argv)
{
#ifndef WIN32
  struct sigaction sa;
#endif
  char *host = NULL;
  char *uuid = NULL;

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
              printf ("\nAirconditioner: Set host to \"%s\"\n", host);
              break;
            case 'u':
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
              printf ("\nAirconditioner: Set uuid to \"%s\"\n", uuid);
              break;
            default:
              fprintf (stderr, "\nAirconditioner: Bad option %s\n", argv[1]);
            }
        }
      --argc;
      ++argv;
    }

  if (host == NULL)
    {
      printf ("\nAirconditioner: No host was specified!\n");
      exit (0);
    }

  /* initialize soap handle */
  soap_init (&service);
  soap_omode (&service, SOAP_XML_INDENT);
  soap_set_namespaces (&service, acc1_namespaces);

  /* initialize device and services */
  if (dpws_init (&device, host)
      || acc_setup_HostingService (&device, &service, uuid, 100)
      || acc_setup_AirConditionerControl (&device, &service,
                                          AIRCONDITIONERCONTROL_WSDL, 100))
    {
      printf ("\nAirconditioner: Can't init device and services\n");
      dpws_done (&device);
      exit (0);
    }

  /* Set Metadata */
  acc_set_Metadata (&device);
  acc_set_wsdl (&device);

  /* Update Metadata */
  if (dpws_update_Metadata (&device))
    {
      printf ("\nAirconditioner: Can't init metadata\n");
      dpws_done (&device);
      exit (0);
    }

  if (discovery_helper_start (&device))
    {
      printf ("\nAirconditioner: Can't init discovery helper\n");
      dpws_done (&device);
      exit (0);
    }
  discovery_helper_addDeviceCB (newdevice);
  discovery_helper_invalidateDeviceCB (invalidatedevice);

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
      printf ("\nAirconditioner: Can't activate device\n");
      dpws_done (&device);
      exit (0);
    }

  printf ("\nAirconditioner: ready to serve... (Ctrl-C for shut down)\n");

  for (;;)
    {
      struct soap *handle = NULL, *soap_set[] = SOAP_HANDLE_SET (&service);
      int (*serve_requests[]) (struct soap * soap) =
        SOAP_SERVE_SET (acc1_serve_request);

      printf ("\nAirconditioner: waiting for request\n");

      /* waiting for new messages */
      handle = dpws_maccept (&device, 100000, 1, soap_set);

      if (handle)
        {

          printf ("\nAirconditioner: processing request from %s:%d\n",
                  inet_ntoa (handle->peer.sin_addr),
                  ntohs (handle->peer.sin_port));

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

int
__acc1__GetStatus (struct soap *soap,
                   struct _acc1__EmptyMessage *acs1__GetStatus,
                   struct acc1__ACStateTypes *acc1__ACStates)
{
  return dpws_header_gen_response (soap, NULL, wsa_header_get_ReplyTo (soap),
                                   "http://www.ws4d.org/axis2/tutorial/AirConditionerControl/GetStatusOut",
                                   wsa_header_get_MessageId (soap),
                                   sizeof (struct SOAP_ENV__Header));
}

int
__acc1__SetTargetTemperature (struct soap *soap, int acc1__TargetTemperature,
                              struct __acc1__SetTargetTemperatureResponse
                              *_param_1)
{
  return dpws_header_gen_response (soap, NULL, wsa_header_get_ReplyTo (soap),
                                   "http://www.ws4d.org/axis2/tutorial/AirConditionerControl/SetTargetTemperatureOut",
                                   wsa_header_get_MessageId (soap),
                                   sizeof (struct SOAP_ENV__Header));
}
