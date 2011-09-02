/* hosting service - out of process service hosting
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
 *
 * author: Guido Moritz, Elmar Zeeb and Christian Fabian
 */

#include "dpws.nsmap"
#include "dpws_device.h"

#include "config.h"

#ifdef WIN32
#include <sys\stat.h>
#include <sys/stat.h>
#include <assert.h>
#include <winbase.h>
#include <process.h>
#include "windows.h"
#include "direct.h"
#include <signal.h>
#else
#include <signal.h>
#include <sys/stat.h>
#include "dirent.h"
#endif

#define XML_SIZE    10240
#define MAX_CHAR_SIZE 512

struct soap service;
struct dpws_s device;
struct ws4d_list_node xml_files_register;

ws4d_alloc_list alist;

struct xml_file
{
  struct ws4d_list_node list;
  char *file;
  int found;
  char **services;
  char *devicetypes;
};

static void
service_exit ()
{
  printf ("\n shutting down...\n");
  dpws_deactivate_hosting_service (&device);
  dpws_done (&device);
  soap_done (&service);
  ws4d_alloclist_done (&alist);
  exit (0);
}

static void
read_file (const char *file, char *xml)
{
  FILE *fd;
  int i = 0;

  fd = fopen (file, "r");
  if (fd == NULL)
    {
      printf ("File: %s not found\n", file);
      exit (0);
    }
  fseek (fd, 0L, SEEK_SET);

  while (!feof (fd))
    xml[i++] = fgetc (fd);

  xml[i] = '\0';
  fclose (fd);
}

static int
setup_hostingservice (struct soap *handle, const char *xml_file,
                      const char *uuid)
{
  char *xml;
  int i;
  struct soap parser;
  struct _wsm__Metadata Metadata;
  struct _wsm__MetadataSection *MetadataSection = NULL;
  struct wsdp__ThisDeviceType *ThisDevice = NULL;
  struct wsdp__ThisModelType *ThisModel = NULL;
  struct _wsdp__Relationship *Relation = NULL;

  xml = (char *) ws4d_malloc_alist (XML_SIZE * sizeof (char), &alist);
  if (xml == NULL)
    {
      printf ("Malloc failed");
      exit (1);
    }

  read_file (xml_file, xml);

  soap_init (&parser);
  soap_set_namespaces (&parser, dpws_namespaces);

  if (soap_begin_deser (&parser, xml, strlen (xml), dpws_namespaces) !=
      SOAP_OK)
    {
      printf ("\nERROR in: soap_begin_deser\n");
      exit (0);
    }

  soap_default__wsm__Metadata (&parser, &Metadata);
  if (soap_get__wsm__Metadata (&parser, &Metadata, "wsm:Metadata", "") ==
      NULL)
    {
      printf ("\nERROR, Metadata not valid\n");
      exit (0);
    }

  for (i = 0; i < Metadata.__sizeMetadataSection; i++)
    {
      MetadataSection = &Metadata.wsm__MetadataSection[i];

      if (MetadataSection && MetadataSection->Dialect
          && !strcmp (MetadataSection->Dialect, DPWS_MEX_RELATIONSHIP))
        {
          printf ("\nRelation found:\n");
          Relation =
            MetadataSection->union_MetadataSection.wsdp__Relationship;

          if (dpws_add_hosting_service
              (&device, handle, 0, NULL, uuid, 100, 0))
            {
              printf ("\nCan't add hosting service\n");
              dpws_done (&device);
              exit (0);
            }
          dpws_add_typestring (&device,
                               MetadataSection->union_MetadataSection.
                               wsdp__Relationship->wsdp__Host->wsdp__Types);
        }

      if (MetadataSection && MetadataSection->Dialect
          && !strcmp (MetadataSection->Dialect, DPWS_MEX_MODEL))
        {
          struct ws4d_thisModel *model;
          printf ("\nModel found:\n");

          if (MetadataSection->__union_MetadataSection !=
              SOAP_UNION__wsm__union_MetadataSection_wsdp__ThisModel)
            {
              return SOAP_ERR;
            }

          ThisModel = MetadataSection->union_MetadataSection.wsdp__ThisModel;

          model = dpws_change_thismodel (&device);
          soap_thismodel_copy (model, ThisModel, dpws_get_alist (&device));

          if ((ThisModel->__sizeManufacturer > 0)
              && soap_locstring_get (ThisModel->Manufacturer,
                                     ThisModel->__sizeManufacturer, "de"))
            {
              printf ("\n + Manufacturer (de): %s",
                      soap_locstring_get (ThisModel->Manufacturer,
                                          ThisModel->
                                          __sizeManufacturer, "de"));
            }

          if (ThisModel->ManufacturerUrl)
            {
              printf ("\n + ManufacturerUrl: %s", ThisModel->ManufacturerUrl);
            }

          if ((ThisModel->__sizeModelName > 0)
              && soap_locstring_get (ThisModel->ModelName,
                                     ThisModel->__sizeModelName, "de"))
            {
              printf ("\n + ModelName (de): %s",
                      soap_locstring_get (ThisModel->ModelName,
                                          ThisModel->__sizeModelName, "de"));
            }

          if (ThisModel->ModelNumber)
            {
              printf ("\n + ModelNumber: %s", ThisModel->ModelNumber);
            }

          if (ThisModel->ModelUrl)
            {
              printf ("\n + ModelUrl: %s", ThisModel->ModelUrl);
            }

        }

      if (MetadataSection && MetadataSection->Dialect
          && !strcmp (MetadataSection->Dialect, DPWS_MEX_DEVICE))
        {
          struct ws4d_thisDevice *_device;

          printf ("\nDevice found:\n");
          if (MetadataSection->__union_MetadataSection !=
              SOAP_UNION__wsm__union_MetadataSection_wsdp__ThisDevice)
            {
              return SOAP_ERR;
            }

          ThisDevice =
            MetadataSection->union_MetadataSection.wsdp__ThisDevice;

          _device = dpws_change_thisdevice (&device);
          soap_thisdevice_copy (_device, ThisDevice,
                                dpws_get_alist (&device));

          if ((ThisDevice->__sizeFriendlyName > 0)
              && soap_locstring_get (ThisDevice->FriendlyName,
                                     ThisDevice->__sizeFriendlyName, "de"))
            {
              printf ("\n + FriendlyName (de): %s",
                      soap_locstring_get (ThisDevice->FriendlyName,
                                          ThisDevice->
                                          __sizeFriendlyName, "de"));
            }
          if ((ThisDevice->__sizeFriendlyName > 0)
              && soap_locstring_get (ThisDevice->FriendlyName,
                                     ThisDevice->__sizeFriendlyName, "en"))
            {
              printf ("\n + FriendlyName (en): %s",
                      soap_locstring_get (ThisDevice->FriendlyName,
                                          ThisDevice->
                                          __sizeFriendlyName, "en"));
            }

          if (ThisDevice->FirmwareVersion)
            {
              printf ("\n + FirmwareVersion: %s",
                      ThisDevice->FirmwareVersion);
            }

          if (ThisDevice->SerialNumber)
            {
              printf ("\n + SerialNumber: %s", ThisDevice->SerialNumber);
            }
        }
    }
  soap_end_deser (&parser);
  soap_end (&parser);
  soap_done (&parser);

  return SOAP_OK;
}

static int
setup_hostedservice (struct xml_file *xml_file)
{
  char *xml;
  int i, j;
  struct soap parser;
#if defined(DEVPROF_2009_01)
  struct wsdp__HostedServiceType *Hosted;
#else
  struct wsdp__HostServiceType *Hosted;
#endif
  struct ws4d_epr *service_s = NULL;
  struct _wsm__Metadata Metadata;
  struct _wsm__MetadataSection *MetadataSection;
  struct _wsdp__Relationship *Relation;

  ws4d_assert (xml_file, SOAP_ERR);

  xml = (char *) ws4d_malloc_alist (XML_SIZE * sizeof (char), &alist);
  if (xml == NULL)
    {
      printf ("Malloc failed");
      exit (1);
    }

  read_file (xml_file->file, xml);

  soap_init (&parser);
  soap_set_namespaces (&parser, dpws_namespaces);
  soap_begin_deser (&parser, xml, strlen (xml), dpws_namespaces);
  soap_default__wsm__Metadata (&parser, &Metadata);

  if (soap_get__wsm__Metadata (&parser, &Metadata, "wsm:Metadata", "") ==
      NULL)
    {
      printf ("\nERROR, Metadata not valid, check %s\n", xml_file->file);
      exit (0);
    }

  for (i = 0; i < Metadata.__sizeMetadataSection; i++)
    {
      MetadataSection = &Metadata.wsm__MetadataSection[i];

      if (MetadataSection && MetadataSection->Dialect
          && !strcmp (MetadataSection->Dialect, DPWS_MEX_RELATIONSHIP))
        {
          printf ("\nRelation found:\n");

          if (MetadataSection->__union_MetadataSection !=
              SOAP_UNION__wsm__union_MetadataSection_wsdp__Relationship)
            {
              return SOAP_ERR;
            }
          Relation =
            MetadataSection->union_MetadataSection.wsdp__Relationship;
          xml_file->services =
            ws4d_malloc_alist ((Relation->__sizeHosted + 1) * sizeof (char *),
                               &alist);

          if (Relation->wsdp__Host)
            {
              if (Relation->wsdp__Host->wsdp__Types)
                {
                  dpws_add_typestring (&device,
                                       Relation->wsdp__Host->wsdp__Types);
                }
            }

          for (j = 0; j < Relation->__sizeHosted; j++)
            {
              char *service_id = NULL;
              char uri[DPWS_URI_MAX_LEN + 1];

              Hosted = &Relation->wsdp__Hosted[j];

              printf ("\tServiceID: %s\n", Hosted->wsdp__ServiceId);
              printf ("\tTypes    : %s\n", Hosted->wsdp__Types);
              printf ("\tAdress   : %s\n",
                      Hosted->wsa__EndpointReference.Address);

              /*
               * 1. init
               * 2. add_type
               * 3. dpws_add_hosted_service
               * 4. dpws_updatemetadata_RS
               */
              service_s =
                dpws_service_init (&device, Hosted->wsdp__ServiceId);
              if (service_s == NULL)
                {
                  printf ("\nCan't init services\n");
                  dpws_done (&device);
                  exit (0);
                }
              if (dpws_service_add_typestring (service_s, Hosted->wsdp__Types)
                  != SOAP_OK)
                {
                  printf ("\nCan't add type\n");
                  dpws_done (&device);
                  exit (0);
                }

              strncpy (uri, Hosted->wsa__EndpointReference.Address,
                       DPWS_URI_MAX_LEN);
              if (dpws_add_hosted_service
                  (&device, service_s, uri, DPWS_URI_MAX_LEN) != SOAP_OK)
                {
                  printf ("\nCan't add service\n");
                  dpws_done (&device);
                  exit (0);
                }

              service_id = ws4d_strdup (Hosted->wsdp__ServiceId, &alist);
              xml_file->services[j] = service_id;
            }
        }
    }
  soap_end_deser (&parser);
  soap_end (&parser);
  soap_done (&parser);

  return SOAP_OK;
}

static int
shutdown_hostedservice (struct xml_file *xml_file)
{
  char **service_id = xml_file->services;

  while (*service_id)
    {
      struct ws4d_epr *service_s = NULL;

      service_s =
        dpws_get_hosted_service (&device, (const char *) *service_id);
      if (service_s)
        {
          dpws_del_hosted_service (&device, service_s);
          dpws_service_done (&device, service_s);
        }

      ws4d_free_alist (*service_id);

      service_id++;
    }

  ws4d_free_alist (xml_file->services);

  return SOAP_OK;
}

static int
dir_monitor (const char *location)
{
  char string_xmlFile[MAX_CHAR_SIZE];
  int found = 0;
  int change = 0;
  register struct xml_file *elem, *next;

#ifdef WIN32
  WIN32_FIND_DATA w32fd;
  HANDLE hFind = NULL;
#else
  struct stat attribut;
  struct dirent *dir_entry_p;
  DIR *dir_p = opendir (location);
#endif

  if (!location)
    {
      return SOAP_ERR;
    }

  /* set all entries to zero */
  ws4d_list_foreach (elem, next, &xml_files_register, struct xml_file, list)
  {
    if (elem)
      elem->found = 0;
  }

  printf ("\nCheck for XML-files in %s", location);

#ifdef WIN32
  sprintf (string_xmlFile, "%s\\*.xml", location);
  hFind = FindFirstFile (string_xmlFile, &w32fd);
  if (hFind == INVALID_HANDLE_VALUE)
    {
      return SOAP_ERR;
    }
  do
    {
      if (!strcmp (w32fd.cFileName, ".") || !strcmp (w32fd.cFileName, ".."))
        {
          continue;
        }

      SNPRINTF (string_xmlFile, MAX_CHAR_SIZE, "%s\\%s", location,
                w32fd.cFileName);

#else
  while (NULL != (dir_entry_p = readdir (dir_p)))
    {
      if (!stat (dir_entry_p->d_name, &attribut)
          || !strstr (dir_entry_p->d_name, ".xml"))
        {
          continue;
        }

      SNPRINTF (string_xmlFile, MAX_CHAR_SIZE, "%s/%s", location,
                dir_entry_p->d_name);

#endif
      found = 0;
      /* a file was found
       * //search all known entries */
      ws4d_list_foreach (elem, next,
                         &xml_files_register, struct xml_file, list)
      {
        if (elem && elem->file && !strcmp (elem->file, string_xmlFile))
          {
            /* file found in list */
            found = 1;
            printf ("\nfound: %s", string_xmlFile);
            elem->found = 1;
            break;
          }
      }

      if (!found)
        {
          struct xml_file *file = NULL;

          printf ("\n-->add: %s", string_xmlFile);

          /* file not found in the register
           * add to register */
          file = ws4d_malloc_alist (sizeof (struct xml_file), &alist);
          if (!file)
            {
              printf ("Malloc failed (dir_monitor: file)");
              return SOAP_ERR;
            }
          file->file = ws4d_strdup (string_xmlFile, &alist);
          file->found = 1;
          ws4d_list_add (&file->list, &xml_files_register);

          /* parse file */
          setup_hostedservice (file);

          change = 1;
        }
    }
#ifdef WIN32
  while (FindNextFile (hFind, &w32fd));
  FindClose (hFind);
#else
  closedir (dir_p);
#endif

  printf ("\n");
  /* search for services, which are removed */
  ws4d_list_foreach (elem, next, &xml_files_register, struct xml_file, list)
  {

    if (elem && elem->found == 0)
      {
        printf ("\n--> Elem found, that not exist any more: %s", elem->file);

        shutdown_hostedservice (elem);
        /* service removed -> delete */
        ws4d_list_del (&elem->list);
        ws4d_free_alist (elem->file);

        ws4d_free_alist (elem);

        change = 1;
      }
  }

  /* Update Metadata */
  if (change && dpws_update_Metadata_hosting (&device))
    {
      printf ("\nCan't init metadata\n");
      dpws_done (&device);
      exit (0);
    }

  return SOAP_OK;
}


int
main (int argc, char **argv)
{
  char *host = NULL;
  char *uuid = NULL;
  const char *metadata = HOSTING_SERVICE_CONFIG_FILE;
  const char *location = HOSTING_SERVICE_CONFIG_DIR;
#ifndef WIN32
  struct sigaction sa;
#endif

  /* init register for xml-files */
  WS4D_INIT_LIST (&xml_files_register);
  WS4D_ALLOCLIST_INIT (&alist);

  /* parsing command line options */
  while ((argc > 1) && (argv[1][0] == '-'))
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
          printf ("hosting_service: Set host to \"%s\"\n", host);
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
          printf ("hosting_service: Set uuid to \"%s\"\n", uuid);
          break;
        case 'l':
          if (strlen (option) > 2)
            {
              ++option;
              location = option;
            }
          else
            {
              --argc;
              ++argv;
              location = argv[1];
            }
          printf ("hosting_service: Set location to \"%s\"\n", location);
          break;
        case 'm':
          if (strlen (option) > 2)
            {
              ++option;
              metadata = option;
            }
          else
            {
              --argc;
              ++argv;
              metadata = argv[1];
            }
          printf ("hosting_service: Set metadata to \"%s\"\n", metadata);
          break;
        default:
          printf ("hosting_service: Bad option %s\n", argv[1]);
          printf ("\t-i : IP-Adress\n");
          printf ("\t-u : UUID\n");
          printf
            ("\t-m : location and name of the hosting_service-metadata file\n");
          printf ("\t-l : location of the XML-Metadata-Folder\n");
          exit (0);
        }
      --argc;
      ++argv;
    }

  if (host == NULL || uuid == NULL)
    {
      printf
        ("\nhosting service: host (-h), uuid (-u), metadata (-m) and location (-l) have to be specified!\n");
      exit (0);
    }

  /* init stack */
  soap_init (&service);
  soap_omode (&service, SOAP_XML_INDENT);
  soap_set_namespaces (&service, dpws_namespaces);

  if (dpws_init (&device, host))
    {
      printf ("\nCan't init device\n");
      dpws_done (&device);
      exit (0);
    }

  /* setup hosting service */
  setup_hostingservice (&service, metadata, uuid);

  /* Update Metadata */
  if (dpws_update_Metadata_hosting (&device))
    {
      printf ("\nCan't init metadata\n");
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

  printf ("\nHosting ready to serve... (Ctrl-C for shut down)\n");

  dpws_activate_hosting_service (&device);

  dir_monitor (location);

  for (;;)
    {
      struct soap *handle = NULL;
      struct soap *soap_set[] = SOAP_HANDLE_SET (&service);
      int (*serve_requests[]) (struct soap * soap) = SOAP_SERVE_SET (NULL);

      printf ("\nHostingService: waiting for request\n");

      handle = dpws_maccept (&device, 5000, 1, soap_set);

      if (handle)
        {
          int port;
#if DEBUG
#ifdef WITH_IPV6
          struct sockaddr_in6 *in6 = (struct sockaddr_in6 *) soap_getpeer(handle);
          char straddr[INET6_ADDRSTRLEN];

          ws4d_inet_ntop (AF_INET6, &in6->sin6_addr, straddr,
                          sizeof (straddr));
          port = ntohs (in6->sin6_port);
#else
          struct sockaddr_in *in = (struct sockaddr_in *) soap_getpeer(handle);
          char straddr[INET_ADDRSTRLEN];

          ws4d_inet_ntop (AF_INET, &in->sin_addr, straddr, sizeof (straddr));
          port = ntohs (in->sin_port);
#endif

          printf ("\nHostingService: processing request from %s:%d\n",
                  straddr, port);
#endif

          if (dpws_mserve (handle, 0, serve_requests))
            {
              soap_print_fault (handle, stderr);
            }

          /* clean up soaps internaly allocated memory */
          soap_end (handle);
        }
      dir_monitor (location);
    }

  soap_end (&service);
  soap_done (&service);

  return -1;
}
