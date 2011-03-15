/* Steffen Prüter - MSIOP Testcase 2.1.1 & 2.2.1
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

#include "ats.nsmap"
#include "dpws_client.h"

#include "ws4d_eprllist.h"
#include "ws4d_service.h"

#include "fileio.h"

#include "attachment_service_client.h"

// MSIOP Testcase 3.1.1 ////////////////////////////////////////////////////////////////////////////

int
call_attachmentservice_oneway_method (struct dpws_s *dpws, const char *uuid,
                                      const char *serviceid)
{
  static unsigned char buffer[1000000];
  char MsgId[DPWS_MSGID_SIZE];

  struct soap client;
  struct soap client2;
  ws4d_alloc_list alist;
  int test_err = 0;

  struct ws4d_epr *device = NULL, *service = NULL;

  struct ws4d_abs_eprlist services;

  ws4d_qnamelist service_type_list;

  char *XAddrs = NULL;

  struct ats1__AttachmentType ats1__OneWayAttachment;
  struct __ats1__OneWayAttachment _param_1;

  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);


  /* allocate and prepare device to resolve */
  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, uuid);

  XAddrs = (char *) dpws_resolve_addr (dpws, device, NULL, 10000);
  if (XAddrs != NULL)
    {
      fprintf (stderr, "\nDevice %s found at addr %s\n",
               ws4d_epr_get_Addrs (device), XAddrs);
    }
  else
    {
      fprintf (stderr, "\nDevice %s cannot be found\n",
               ws4d_epr_get_Addrs (device));
      fflush (NULL);
      return 0;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (dpws, device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), serviceid))
          {
            service = epr;
            printf ("\nDevice offers AttachmentService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
      if (!service)
        {
          printf ("\nAttachmentService not found on %s\n",
                  ws4d_epr_get_Addrs (device));
          return 0;
        }
    }
  else
    {
      printf ("\nAttachmentService not found on %s\n",
              ws4d_epr_get_Addrs (device));
      return 0;
    }


  ///start
  soap_init1 (&client2, SOAP_ENC_MTOM);
  dpws_handle_init (dpws, &client2);

  ats1__OneWayAttachment.Param.__ptr = buffer;
  ats1__OneWayAttachment.Param.__size =
    readfile ("dpws1ref.jpg", ats1__OneWayAttachment.Param.__ptr);

  if (ats1__OneWayAttachment.Param.__size < 0)
    {
      printf ("Cannot read file for attachment\n");
      return 0;
    }

  ats1__OneWayAttachment.Param.id = NULL;
  ats1__OneWayAttachment.Param.type = (char *) "image/jpeg";
  ats1__OneWayAttachment.Param.options = NULL;
  ats1__OneWayAttachment.__anyAttribute = NULL;

  soap_set_namespaces (&client2, ats_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  dpws_header_gen_oneway (&client2, MsgId, ws4d_epr_get_Addrs (service),
                          "http://schemas.example.org/AttachmentService/OneWayAttachment",
                          NULL, sizeof (struct SOAP_ENV__Header));

  if (soap_send___ats1__OneWayAttachment
      (&client2, ws4d_epr_get_Addrs (service), NULL,
       &ats1__OneWayAttachment) == SOAP_OK)
    {
      printf ("Successfully called OneWay-operation on %s\n",
              ws4d_epr_get_Addrs (service));
    }
  else
    {
      printf ("Error: calling OneWay-operation on %s\n",
              ws4d_epr_get_Addrs (service));
      test_err = 1;
    }

  if (soap_recv___ats1__OneWayAttachment (&client2, &_param_1) != 202)
    {
      printf ("Error: got not response from %s\n",
              ws4d_epr_get_Addrs (service));
      test_err = 1;
    }


  soap_end (&client);
  soap_done (&client);
  soap_end (&client2);
  soap_done (&client2);

  ws4d_eprlist_done (&services);
  ws4d_epr_free (1, device);

  ws4d_alloclist_done (&alist);

  if (test_err)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

// MSIOP Testcase 3.2.1 ////////////////////////////////////////////////////////////////////////////

int
call_attachmentservice_twoway_method (struct dpws_s *dpws, const char *uuid,
                                      const char *serviceid)
{
  static unsigned char buffer[1000000];
  char MsgId[DPWS_MSGID_SIZE];

  struct soap client;
  struct soap client2;
  ws4d_alloc_list alist;
  int test_err = 0;

  struct ws4d_epr *device = NULL, *service = NULL;

  struct ws4d_abs_eprlist services;

  ws4d_qnamelist service_type_list;

  char *XAddrs = NULL;

  struct ats1__AttachmentType ats1__TwoWayAttachmentRequest;
  struct ats1__AttachmentType ats1__TwoWayAttachmentResponse;

  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);


  /* allocate and prepare device to resolve */
  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, uuid);

  XAddrs = (char *) dpws_resolve_addr (dpws, device, NULL, 10000);
  if (XAddrs != NULL)
    {
      fprintf (stderr, "\nDevice %s found at addr %s\n",
               ws4d_epr_get_Addrs (device), XAddrs);
    }
  else
    {
      fprintf (stderr, "\nDevice %s cannot be found\n",
               ws4d_epr_get_Addrs (device));
      fflush (NULL);
      return 0;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (dpws, device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), serviceid))
          {
            service = epr;
            printf ("\nDevice offers AttachmentService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
      if (!service)
        {
          printf ("\nAttachmentService not found on %s\n",
                  ws4d_epr_get_Addrs (device));
          return 0;
        }
    }
  else
    {
      printf ("\nAttachmentService not found on %s\n",
              ws4d_epr_get_Addrs (device));
      return 0;
    }

  ///start
  soap_init1 (&client2, SOAP_ENC_MTOM);
  dpws_handle_init (dpws, &client2);

  ats1__TwoWayAttachmentRequest.Param.__ptr = buffer;
  ats1__TwoWayAttachmentRequest.Param.__size =
    readfile ("dpws1ref.jpg", ats1__TwoWayAttachmentRequest.Param.__ptr);

  if (ats1__TwoWayAttachmentRequest.Param.__size < 0)
    {
      printf ("Cannot read file for attachment\n");
      return SOAP_OK;
    }

  ats1__TwoWayAttachmentRequest.Param.id = NULL;
  ats1__TwoWayAttachmentRequest.Param.type = (char *) "image/jpeg";
  ats1__TwoWayAttachmentRequest.Param.options = NULL;
  ats1__TwoWayAttachmentRequest.__anyAttribute = NULL;

  soap_set_namespaces (&client2, ats_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  dpws_header_gen_request (&client2, MsgId, ws4d_epr_get_Addrs (service),
                           "http://schemas.example.org/AttachmentService/TwoWayAttachmentRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

  if (soap_call___ats1__TwoWayAttachment
      (&client2, ws4d_epr_get_Addrs (service), NULL,
       &ats1__TwoWayAttachmentRequest,
       &ats1__TwoWayAttachmentResponse) == SOAP_OK)

    {
      int res = 0;

      printf ("Received Attachment size(%d) type(%s)\n",
              ats1__TwoWayAttachmentResponse.Param.__size,
              ats1__TwoWayAttachmentResponse.Param.type);

      if (writefile ("dpws2recv.jpg",
                     ats1__TwoWayAttachmentResponse.Param.__ptr,
                     ats1__TwoWayAttachmentResponse.Param.__size) < 0)
        {
          printf ("\nCannot save attachment\n");
        }

      if ((res = comparefiles ("dpws2recv.jpg", "dpws2.jpg")) == 0)
        {
          test_err = 1;
          printf ("Attachment OK\n");
        }
      else
        {
          printf ("Attachment NOTOK: difference at byte %d\n", res);
        }

    }
  soap_end (&client);
  soap_done (&client);
  soap_end (&client2);
  soap_done (&client2);

  ws4d_eprlist_done (&services);
  ws4d_epr_free (1, device);

  ws4d_alloclist_done (&alist);


  return test_err;
}
