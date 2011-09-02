/* Steffen Pr�ter - MSIOP Testcase 2.1.1 & 2.2.1
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

#include "dpwsH.h"
#include "dpws_client.h"

#include "ws4d_eprllist.h"

#include "ws4d_targetcache.h"
#include "ws4d_service.h"

#include "description_client.h"

int
do_getMetadata (struct dpws_s *dpws, struct ws4d_epr *device)
{
  struct _wsm__Metadata metadata;
  struct soap client;
  int got_metadata = 1;

  soap_init (&client);
  soap_omode (&client, SOAP_C_UTFSTRING | SOAP_XML_INDENT);

  dpws_handle_init (dpws, &client);

  if (dpws_metadata_get (dpws, &client, device, &metadata, 10000) != WS4D_OK)
    {
      soap_print_fault (&client, stdout);
      got_metadata = 0;
    }

  soap_end (&client);
  soap_done (&client);

  return got_metadata;
}

int
do_verbose_getMetadata (struct dpws_s *dpws, struct ws4d_epr *device)
{
  ws4d_alloc_list alist;
  struct _wsm__Metadata metadata;
  struct ws4d_abs_eprlist services;
  struct ws4d_thisDevice ThisDevice;
  struct ws4d_thisModel ThisModel;
  struct soap client;
  int got_metadata = 1;

  WS4D_ALLOCLIST_INIT (&alist);

  soap_init (&client);
  soap_omode (&client, SOAP_C_UTFSTRING | SOAP_XML_INDENT);

  dpws_handle_init (dpws, &client);

  if (dpws_metadata_get (dpws, &client, device, &metadata, 10000) != WS4D_OK)
    {
      soap_print_fault (&client, stdout);
      got_metadata = 0;
    }

  if (got_metadata)
    {                           /* get ThisDevice */
      if (!dpws_metadata_getThisDevice (&metadata, &alist, &ThisDevice))
        {
          printf ("Device description:\n");
          if ((ThisDevice.__sizeFriendlyName > 0)
              && ws4d_locstring_get (ThisDevice.FriendlyName,
                                     ThisDevice.__sizeFriendlyName, "de"))
            {
              printf (" + FriendlyName (de): %s\n",
                      ws4d_locstring_get (ThisDevice.FriendlyName,
                                          ThisDevice.__sizeFriendlyName,
                                          "de"));
            }

          if (ThisDevice.FirmwareVersion)
            {
              printf (" + FirmwareVersion: %s\n", ThisDevice.FirmwareVersion);
            }

          if (ThisDevice.SerialNumber)
            {
              printf (" + SerialNumber: %s\n", ThisDevice.SerialNumber);
            }
        }

      /* get ThisModel */

      if (!dpws_metadata_getThisModel (&metadata, &alist, &ThisModel))
        {
          printf ("Model description:\n");

          if ((ThisModel.__sizeManufacturer > 0)
              && ws4d_locstring_get (ThisModel.Manufacturer,
                                     ThisModel.__sizeManufacturer, "de"))
            {
              printf (" + Manufacturer (de): %s\n",
                      ws4d_locstring_get (ThisModel.Manufacturer,
                                          ThisModel.__sizeManufacturer,
                                          "de"));
            }

          if (ThisModel.ManufacturerUrl)
            {
              printf (" + ManufacturerUrl: %s\n", ThisModel.ManufacturerUrl);
            }

          if ((ThisModel.__sizeModelName > 0)
              && ws4d_locstring_get (ThisModel.ModelName,
                                     ThisModel.__sizeModelName, "de"))
            {
              printf (" + ModelName (de): %s\n",
                      ws4d_locstring_get (ThisModel.ModelName,
                                          ThisModel.__sizeModelName, "de"));
            }

          if (ThisModel.ModelNumber)
            {
              printf (" + ModelNumber: %s\n", ThisModel.ModelNumber);
            }

          if (ThisModel.ModelUrl)
            {
              printf (" + ModelUrl: %s\n", ThisModel.ModelUrl);
            }

        }
      /* get Relationship */

      ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
      if (!dpws_metadata_getservices (dpws, device, &metadata, &services))
        {
          register struct ws4d_epr *elem, *iter;

          printf ("Services:\n");

          ws4d_eprlist_foreach (elem, iter, &services)
          {
            if (ws4d_epr_isvalid (elem))
              {
                register struct ws4d_qname *cur_qn, *qname_iter;

                printf (" + %s\n", ws4d_serviceep_getid (elem));
                printf ("    + Address: %s\n", ws4d_epr_get_Addrs (elem));
                printf ("    + Types:\n");
                ws4d_qnamelist_foreach (cur_qn, qname_iter,
                                        ws4d_serviceep_gettypelist (elem))
                {
                  printf (" (%s):%s\n", ws4d_qname_getNS (cur_qn),
                          ws4d_qname_getName (cur_qn));
                }
              }
          }
        }
      ws4d_eprlist_done (&services);
    }

  soap_end (&client);
  soap_done (&client);
  ws4d_alloclist_done (&alist);

  return got_metadata;
}
