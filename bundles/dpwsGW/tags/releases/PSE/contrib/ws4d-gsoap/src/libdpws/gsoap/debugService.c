/* WS4D-gSOAP - Implementation of the Devices Profile for Web Services
 * (DPWS) on top of gSOAP
 * Copyright (C) 2007 University of Rostock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "soapH.h"
#include "stddpws.h"

struct soap *debugSoap = NULL;

struct soap *
dpwsDebug_init (struct soap *soap)
{
  if (!dpws_bind_service (soap, "dpwsDebugService", NULL, "http://", 0, 100))
    {
      printf ("Can't init dpwsDebugService\n");
      return debugSoap;
    }

  debugSoap = soap;

  return debugSoap;
}

int
dpwsDebug__StopDevice (struct soap *soap)
{
  dpws_send_empty_response (soap);

  dpws_device_done ();
  exit (0);

  return SOAP_OK;
}
