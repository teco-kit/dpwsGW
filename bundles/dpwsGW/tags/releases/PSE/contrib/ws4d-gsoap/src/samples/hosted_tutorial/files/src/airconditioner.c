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
#include "acs1.nsmap"
#include "dpws_hosted.h"
#include "acs_metadata.h"
#include "acs_wsdl.h"
#include "config.h"
#include "event_worker.h"
#include <signal.h>
struct soap service;
struct dpws_s device;


void
service_exit ()
{
  printf ("\nAirconditioner: shutting down...\n");

  event_worker_shutdown ();

  dpws_done (&device);
  soap_end (&service);
  soap_done (&service);

  exit (0);
}

int acs1_serve_request (struct soap *soap);

int
main (int argc, char **argv)
{
#ifndef WIN32
  struct sigaction sa;
#endif
  char *host = NULL;

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
  soap_set_namespaces (&service, acs1_namespaces);

  /* initialize device and services */
  if (dpws_init (&device, host)
      || acs_setup_AirConditioner (&device, &service, AIRCONDITIONER_WSDL,
                                   100))
    {
      printf ("\nAirconditioner: Can't init service\n");
      dpws_done (&device);
      exit (0);
    }

  /* Set Metadata */
  acs_set_wsdl (&device);

  /* Update Metadata */
  if (dpws_update_Metadata (&device))
    {
      printf ("\nAirconditioner: Can't init metadata\n");
      dpws_done (&device);
      exit (0);
    }

  /* activate eventing. */
  if (dpws_activate_eventsource (&device, &service))
    {
      printf ("\nAirconditioner: Can't activate eventing\n");
      dpws_done (&device);
      exit (0);
    }

  /* install signal handler for SIGINT or Ctrl-C */
#ifdef WIN32
  signal (SIGINT, service_exit);
#else
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = service_exit;
  sigaction (SIGINT, &sa, NULL);
#endif

  if (event_worker_init (&device))
    {
      printf ("\nAirconditioner: Can't init event worker\n");
      dpws_done (&device);
      exit (0);
    }

  printf ("\nAirconditioner: ready to serve... (Ctrl-C for shut down)\n");

  for (;;)
    {
      struct soap *handle = NULL, *soap_set[] = SOAP_HANDLE_SET (&service);
      int (*serve_requests[]) (struct soap * soap) =
        SOAP_SERVE_SET (acs1_serve_request);

      printf ("\nAirconditioner: waiting for request\n");

      handle = dpws_maccept (&device, 100000, 1, soap_set);

      if (handle)
        {

          printf ("\nAirconditioner: processing request from %s:%d\n",
                  inet_ntoa (handle->peer.sin_addr),
                  ntohs (handle->peer.sin_port));

          if (dpws_mserve (handle, 1, serve_requests))

            {

              soap_print_fault (handle, stderr);

            }

          /* clean up soaps internaly allocated memory */
          soap_end (handle);
        }

      dpws_check_subscriptions (&device);
    }

  return -1;
}

int
__acs1__GetStatus (struct soap *soap,
                   struct _acs1__EmptyMessage *acs1__GetStatus,
                   struct acs1__ACStateType *acs1__ACState)
{
  acs1__ACState->CurrentTemp = event_worker_get_current ();
  acs1__ACState->TargetTemp = event_worker_get_target ();

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
  event_worker_set_target (acs1__TargetTemperature);

  acs1__ACState->CurrentTemp = event_worker_get_current ();
  acs1__ACState->TargetTemp = event_worker_get_target ();

  return dpws_header_gen_response (soap, NULL, wsa_header_get_ReplyTo (soap),
                                   "http://www.ws4d.org/axis2/tutorial/AirConditioner/SetTargetTemperatureOut",
                                   wsa_header_get_MessageId (soap),
                                   sizeof (struct SOAP_ENV__Header));
}
