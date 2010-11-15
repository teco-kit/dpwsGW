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

#include "sis.nsmap"
#include "dpws_device.h"
#include "msiop_metadata.h"
#include "iop_services.h"

#define SIMPLESERVICE_URI_LEN 255

int
setup_SimpleService1 (struct dpws_s *device, struct soap *handle,
                      const char *wsdl, int backlog)
{
  soap_set_namespaces (handle, sis_namespaces);

  return msiop_setup_SimpleService (device, handle, wsdl, backlog);
}

static int
msiop_setup_SimpleService2 (struct dpws_s *dev, struct soap *handle,
                            const char *wsdl, int backlog)
{
  char uri[DPWS_URI_MAX_LEN] = "http://host:0/";
  struct ws4d_qname *service_type;
  struct ws4d_epr *service = NULL;

  service =
    dpws_service_init (dev, "http://schemas.example.org/SimpleService2");

  if (dpws_service_bind
      (dev, service, handle, uri, DPWS_URI_MAX_LEN, backlog))
    {
      return WS4D_ERR;
    }

  service_type = NULL;

  service_type = ws4d_qname_alloc (1, &dev->alloc_list);
  service_type->ns =
    ws4d_strdup ("http://schemas.example.org/SimpleService",
                 &dev->alloc_list);
  service_type->name = ws4d_strdup ("SimpleService", &dev->alloc_list);
  dpws_service_add_type (service, service_type);


  if (dpws_service_set_wsdl (service, wsdl))
    return WS4D_ERR;

  if (dpws_add_hosted_service (dev, service, uri, DPWS_URI_MAX_LEN))
    return WS4D_ERR;

  return dpws_update_Metadata (dev);
}

int
setup_SimpleService2 (struct dpws_s *device, struct soap *handle,
                      const char *wsdl, int backlog)
{
  soap_set_namespaces (handle, sis_namespaces);

  return msiop_setup_SimpleService2 (device, handle, wsdl, backlog);
}

int
__sis1__OneWay (struct soap *soap, struct sis1__OneWayType *sis1__OneWay)
{
  if (sis1__OneWay)
    {
      printf
        ("\nSimpleService: OneWay Operation called with Param: %d\n",
         sis1__OneWay->Param);
    }

  return soap_send_empty_response (soap, SOAP_OK);
}


int
__sis1__TwoWay (struct soap *soap,
                struct sis1__TwoWayType *sis1__TwoWayRequest,
                struct sis1__TwoWayResponseType *sis1__TwoWayResponse)
{
  if (sis1__TwoWayRequest)
    {
      sis1__TwoWayResponse->Sum =
        sis1__TwoWayRequest->X + sis1__TwoWayRequest->Y;

      printf
        ("\nSimpleService: TwoWay Operation called with X: %d and Y: %d\n",
         sis1__TwoWayRequest->X, sis1__TwoWayRequest->Y);
    }

  return dpws_header_gen_response (soap, NULL, wsa_header_get_ReplyTo (soap),
                                   "http://schemas.example.org/SimpleService/TwoWayResponse",
                                   wsa_header_get_MessageId (soap),
                                   sizeof (struct SOAP_ENV__Header));
}
