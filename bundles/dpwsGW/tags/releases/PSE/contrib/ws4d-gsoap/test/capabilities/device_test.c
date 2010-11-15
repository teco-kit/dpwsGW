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

#include "dpws.nsmap"
#include "dpws_device.h"
#include <signal.h>
#include <time.h>
#include "msiop_metadata.h"
#include "msiop_wsdl.h"
#include "iop_services.h"

#include "test_support.h"

struct soap hosting, eventsource, simpleservice, attachmentservice;
struct dpws_s device;

#ifdef SOAP_HANDLE_SET
struct soap *soap_set[] =
SOAP_HANDLE_SET (&hosting, &simpleservice, &attachmentservice,
                 &eventsource);
#endif

const char *host = "127.0.0.1";
const char *uuid = "urn:uuid:3876121b-433e-4360-846e-cdd13041857e";

int sis_serve_request (struct soap *soap);
int ats_serve_request (struct soap *soap);

START_TEST (device_create_destroy)
{
  /* initialize hosting handle */
  soap_init (&hosting);
  soap_set_namespaces (&hosting, dpws_namespaces);

  /* initialize eventsource handle */
  soap_init (&eventsource);

  /* initialize simpleservice1 handle */
  soap_init (&simpleservice);

  /* initialize attachmentservice handle */
  soap_init1 (&attachmentservice, SOAP_ENC_MTOM);

  /* initialize device and services */
  fail_if (dpws_init (&device, host), "Can't initialize device");

  fail_if (msiop_setup_HostingService (&device, &hosting, uuid, 100),
           "Can't initialize hosting service");

  fail_if (setup_SimpleService1 (&device, &simpleservice,
                                 "SimpleService.wsdl", 100),
           "Can't initialize simple service");

  fail_if (setup_AttachmentService (&device, &attachmentservice,
                                    "AttachmentService.wsdl", 100),
           "Can't initialize attachment service");

  fail_if (setup_EventingService (&device, &eventsource,
                                  "EventingService.wsdl", 100),
           "Can't initialize eventing service");

  /* Set Metadata */
  msiop_set_Metadata (&device);
  msiop_set_wsdl (&device);

  /* Update Metadata */
  fail_if (dpws_update_Metadata (&device), "Can't initialize meta data");

  /* activate eventing. */
  fail_if (dpws_activate_eventsource (&device, &eventsource),
           "Can't activate event source");

  fail_if (dpws_activate_hosting_service (&device),
           "Can't activate hosting service");

  fail_if (dpws_deactivate_hosting_service (&device),
           "Can't deactivate hosting service");

  soap_done (&hosting);
  soap_done (&eventsource);
  soap_done (&simpleservice);
  soap_done (&attachmentservice);

  dpws_done (&device);
}
END_TEST

START_TEST (device_create_destroy_two_times)
{
  /* initialize hosting handle */
  soap_init (&hosting);
  soap_set_namespaces (&hosting, dpws_namespaces);

  /* initialize eventsource handle */
  soap_init (&eventsource);

  /* initialize simpleservice1 handle */
  soap_init (&simpleservice);

  /* initialize attachmentservice handle */
  soap_init1 (&attachmentservice, SOAP_ENC_MTOM);

  /* initialize device and services */
  fail_if (dpws_init (&device, host), "Can't initialize device");

  fail_if (msiop_setup_HostingService (&device, &hosting, uuid, 100),
           "Can't initialize hosting service");

  fail_if (setup_SimpleService1 (&device, &simpleservice,
                                 "SimpleService.wsdl", 100),
           "Can't initialize simple service");

  fail_if (setup_AttachmentService (&device, &attachmentservice,
                                    "AttachmentService.wsdl", 100),
           "Can't initialize attachment service");

  fail_if (setup_EventingService (&device, &eventsource,
                                  "EventingService.wsdl", 100),
           "Can't initialize eventing service");

  /* Set Metadata */
  msiop_set_Metadata (&device);
  msiop_set_wsdl (&device);

  /* Update Metadata */
  fail_if (dpws_update_Metadata (&device), "Can't initialize meta data");

  /* activate eventing. */
  fail_if (dpws_activate_eventsource (&device, &eventsource),
           "Can't activate event source");

  fail_if (dpws_activate_hosting_service (&device),
           "Can't activate hosting service");

  fail_if (dpws_deactivate_hosting_service (&device),
           "Can't deactivate hosting service");

  soap_done (&hosting);
  soap_done (&eventsource);
  soap_done (&simpleservice);
  soap_done (&attachmentservice);

  dpws_done (&device);

  /* do everything again */

  /* initialize hosting handle */
  soap_init (&hosting);
  soap_set_namespaces (&hosting, dpws_namespaces);

  /* initialize eventsource handle */
  soap_init (&eventsource);

  /* initialize simpleservice1 handle */
  soap_init (&simpleservice);

  /* initialize attachmentservice handle */
  soap_init1 (&attachmentservice, SOAP_ENC_MTOM);

  /* initialize device and services */
  fail_if (dpws_init (&device, host), "Can't initialize device second time");

  fail_if (msiop_setup_HostingService (&device, &hosting, uuid, 100),
           "Can't initialize hosting service second time");

  fail_if (setup_SimpleService1 (&device, &simpleservice,
                                 "SimpleService.wsdl", 100),
           "Can't initialize simple service second time");

  fail_if (setup_AttachmentService (&device, &attachmentservice,
                                    "AttachmentService.wsdl", 100),
           "Can't initialize attachment service second time");

  fail_if (setup_EventingService (&device, &eventsource,
                                  "EventingService.wsdl", 100),
           "Can't initialize eventing service second time");

  /* Set Metadata */
  msiop_set_Metadata (&device);
  msiop_set_wsdl (&device);

  /* Update Metadata */
  fail_if (dpws_update_Metadata (&device),
           "Can't initialize meta data second time");

  /* activate eventing. */
  fail_if (dpws_activate_eventsource (&device, &eventsource),
           "Can't activate event source second time");

  fail_if (dpws_activate_hosting_service (&device),
           "Can't activate hosting service second time");

  fail_if (dpws_deactivate_hosting_service (&device),
           "Can't deactivate hosting service second time");

  soap_done (&hosting);
  soap_done (&eventsource);
  soap_done (&simpleservice);
  soap_done (&attachmentservice);

  dpws_done (&device);
}
END_TEST

START_TEST (device_without_wsdl)
{
  /* initialize hosting handle */
  soap_init (&hosting);
  soap_set_namespaces (&hosting, dpws_namespaces);

  /* initialize eventsource handle */
  soap_init (&eventsource);

  /* initialize simpleservice1 handle */
  soap_init (&simpleservice);

  /* initialize attachmentservice handle */
  soap_init1 (&attachmentservice, SOAP_ENC_MTOM);

  /* initialize device and services */
  fail_if (dpws_init (&device, host), "Can't initialize device");

  fail_if (msiop_setup_HostingService (&device, &hosting, uuid, 100),
           "Can't initialize hosting service");

  fail_if (setup_SimpleService1 (&device, &simpleservice,
                                 NULL, 100),
           "Can't initialize simple service");

  fail_if (setup_AttachmentService (&device, &attachmentservice,
                                    "", 100),
           "Can't initialize attachment service");

  fail_if (setup_EventingService (&device, &eventsource,
                                  NULL, 100),
           "Can't initialize eventing service");

  /* Set Metadata */
  msiop_set_Metadata (&device);

  /* Update Metadata */
  fail_if (dpws_update_Metadata (&device), "Can't initialize meta data");

  /* activate eventing. */
  fail_if (dpws_activate_eventsource (&device, &eventsource),
           "Can't activate event source");

  fail_if (dpws_activate_hosting_service (&device),
           "Can't activate hosting service");

  fail_if (dpws_deactivate_hosting_service (&device),
           "Can't deactivate hosting service");

  soap_done (&hosting);
  soap_done (&eventsource);
  soap_done (&simpleservice);
  soap_done (&attachmentservice);

  dpws_done (&device);
}
END_TEST

WS4D_TESTCASE_START ("device")
WS4D_TESTCASE_ADD (device_create_destroy)
WS4D_TESTCASE_ADD (device_create_destroy_two_times)
WS4D_TESTCASE_ADD (device_without_wsdl)
WS4D_TESTCASE_END
