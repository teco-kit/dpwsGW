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
#include "dpws_device.h"
#ifndef WIN32
#include <pthread.h>
#endif
struct dpws_s *_device = NULL;

enum ac_states
{ OFF, IDLE, COOLING };         /* Supported states of the air conditioner. */
int currentState = OFF;         /* Initial state is turned off. */
int currentTemp = 24;           /* Initial temperature in degrees celsius. */
int targetTemp = 24;            /* Initial target temperature in degrees celsius. */
int shutdownFlag = 0;           /* Flag indicates systemshutdown. */

#ifdef WIN32                    /* The working thread. */
DWORD workerid;
HANDLE worker;
#else
pthread_t worker;
#endif


int
event_worker_get_current ()
{
  return currentTemp;
}


int
event_worker_set_target (int target)
{
  targetTemp = target;
  if (currentState == OFF)
    {
      currentState = IDLE;
    }
}


int
event_worker_get_target ()
{
  return targetTemp;
}


void
deliver_event (int CurrentTemp, int TargetTemp)
{
  struct soap soap;
  struct ws4d_subscription *subs, *next;
  struct acsinv1__ACStateType event;

  event.CurrentTemp = CurrentTemp;
  event.TargetTemp = TargetTemp;

  soap_init (&soap);
  soap_set_namespaces (&soap, acs_inv1_namespaces);

  dpws_for_each_subs (subs, next, _device,
                      "http://www.ws4d.org/axis2/tutorial/AirConditioner/TemperatureEventOut")
  {
    struct __acsinv1__TemperatureEvent response;
    char *deliverto = dpws_subsm_get_deliveryPush_address (_device, subs);

    if (!deliverto)
      continue;

    dpws_header_gen_oneway (&soap, NULL, deliverto,
                            "http://www.ws4d.org/axis2/tutorial/AirConditioner/TemperatureEventOut",
                            NULL, sizeof (struct SOAP_ENV__Header));

    dpws_header_gen_wseIdentifier (&soap, _device, subs);

    printf ("Sending Event to %s\n", deliverto);

    if (soap_send___acsinv1__TemperatureEvent
        (&soap, deliverto, NULL, &event))
      {
        soap_print_fault (&soap, stderr);
      }
    else
      {
        soap_recv___acsinv1__TemperatureEvent (&soap, &response);
      }

    soap_end (&soap);
  }

  soap_done (&soap);
}


void *
event_worker_loop ()
{
  // Wait for initial start.
  while (OFF == currentState && !shutdownFlag)
    SLEEP (1);

  printf ("Airconditioner: Entering working loop...\n");
  while (!shutdownFlag)
    {
      if (targetTemp < currentTemp)
        {
          printf
            ("Airconditioner: Current Temperature %d; Switching to state COOLING.\n",
             currentTemp);
          currentState = COOLING;

          while (targetTemp <= currentTemp && !shutdownFlag)
            {
              currentTemp--;
              SLEEP (5);
            }
        }
      else
        {
          printf
            ("Airconditioner: Reached target temperature: %d; Switching to state IDLE.\n",
             targetTemp);
          currentState = IDLE;

          deliver_event (currentTemp, targetTemp);

          SLEEP (5);
          currentTemp += 2;
        }
    }
  printf ("Airconditioner: Leaving working loop...\n");
}


int
event_worker_init (struct dpws_s *device)
{
  if (_device)
    {
      return SOAP_ERR;
    }
  else
    {
      if (!device)
        {
          return SOAP_ERR;
        }
      _device = device;
    }

#ifdef WIN32
  worker =
    CreateThread (NULL, 0, (DWORD WINAPI) event_worker_loop, NULL, 0,
                  &workerid);
  if (worker != NULL)
#else
  if (pthread_create (&worker, NULL, event_worker_loop, NULL) == 0)
#endif
    {
      return SOAP_OK;
    }
  else
    {
      return SOAP_ERR;
    }
}


int
event_worker_shutdown ()
{
  shutdownFlag = 1;
#ifdef WIN32
  WaitForSingleObject (worker, INFINITE);
#else
  pthread_join (worker, NULL);
#endif
}
