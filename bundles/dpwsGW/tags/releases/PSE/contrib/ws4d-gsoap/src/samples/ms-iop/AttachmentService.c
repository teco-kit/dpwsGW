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

#include "ats.nsmap"
#include "dpws_device.h"
#include "fileio.h"
#include "msiop_metadata.h"
#include "iop_services.h"

#define ATTACHMENTSERVICE_URI_LEN 255

static unsigned char buffer[1000000];

int
setup_AttachmentService (struct dpws_s *device, struct soap *handle,
                         const char *wsdl, int backlog)
{
  soap_set_namespaces (handle, ats_namespaces);

  return msiop_setup_AttachmentService (device, handle, wsdl, backlog);
}

int
__ats1__OneWayAttachment (struct soap *soap,
                          struct ats1__AttachmentType *ats1__OneWayAttachment)
{
  int res = 0;
  printf ("\nAttachmentService: got oneway Attachment size(%d) type(%s)\n",
          ats1__OneWayAttachment->Param.__size,
          ats1__OneWayAttachment->Param.type);
  if (writefile
      ("dpws1recv.jpg", ats1__OneWayAttachment->Param.__ptr,
       ats1__OneWayAttachment->Param.__size) < 0)
    {
      printf ("\nAttachmentService: Cannot save attachment\n");
      return SOAP_OK;
    }
  if ((res = comparefiles ("dpws1recv.jpg", "dpws1ref.jpg")) == 0)
    printf ("\nAttachmentService: Attachment OK\n");
  else
    printf ("\nAttachmentService: Attachment NOK: difference at byte %d\n",
            res);

  return soap_send_empty_response (soap, SOAP_OK);
}

int
__ats1__TwoWayAttachment (struct soap *soap,
                          struct ats1__AttachmentType
                          *ats1__TwoWayAttachmentRequest,
                          struct ats1__AttachmentType
                          *ats1__TwoWayAttachmentResponse)
{
  int res = 0;
  printf ("\nAttachmentService: got oneway Attachment size(%d) type(%s)\n",
          ats1__TwoWayAttachmentRequest->Param.__size,
          ats1__TwoWayAttachmentRequest->Param.type);
  if (writefile
      ("dpws1recv.jpg", ats1__TwoWayAttachmentRequest->Param.__ptr,
       ats1__TwoWayAttachmentRequest->Param.__size) < 0)
    {
      printf ("\nAttachmentService: Cannot save attachment\n");
      return SOAP_OK;
    }
  if ((res = comparefiles ("dpws1recv.jpg", "dpws1ref.jpg")) == 0)
    printf ("\nAttachmentService: Attachment OK\n");
  else
    printf ("\nAttachmentService: Attachment NOK: difference at byte %d\n",
            res);
  ats1__TwoWayAttachmentResponse->Param.__ptr = buffer;
  ats1__TwoWayAttachmentResponse->Param.__size =
    readfile ("dpws2.jpg", ats1__TwoWayAttachmentResponse->Param.__ptr);
  if (ats1__TwoWayAttachmentResponse->Param.__size < 0)
    {
      printf ("\nAttachmentService: Cannot read file for attachment\n");
      return SOAP_OK;
    }
  ats1__TwoWayAttachmentResponse->Param.id = NULL;
  ats1__TwoWayAttachmentResponse->Param.type = (char *) "image/jpeg";

  return dpws_header_gen_response (soap, NULL, wsa_header_get_ReplyTo (soap),
                                   "http://schemas.example.org/AttachmentService/TwoWayAttachmentResponse",
                                   wsa_header_get_MessageId (soap),
                                   sizeof (struct SOAP_ENV__Header));
}
