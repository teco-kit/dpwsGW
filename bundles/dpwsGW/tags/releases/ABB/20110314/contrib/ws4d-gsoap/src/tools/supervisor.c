/* supervisor - command line tool to find and describe dpws devices
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

#include <sys/types.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#include <pthread.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#include "dpws.nsmap"
#include "dpws_client.h"

#include "ws4d_targetcache.h"
#include "ws4d_servicecache.h"

#include "ws4d_eprllist.h"

struct soap discovery;
struct dpws_s dpws;

struct ws4d_tc_int *target_cache;

char *host = NULL;
int running = 1;

enum cmdloop_cmds
{
  PRTHELP = 0, PROBE, DPROBE, RESOLVE, DESCRIBE, LIST, EXITCMD
};

#define MAX_NUM_ARGS 5

struct cmdloop_commands
{
  const char *str;              /* the string */
  int cmdnum;                   /* the command */
  int numargs;                  /* the number of arguments */
  const char *args;             /* the args */
  const char *help;             /* help for the commands */
} cmdloop_commands;

static struct cmdloop_commands cmdloop_cmdlist[] = {
  {"Help", PRTHELP, 0, "", "Print this help info"},
  {"Probe", PROBE, 0, "", "Probe for devices in the network"},
  {"DProbe", DPROBE, 1, "", "Unicast Probe for a specific device"},
  {"Resolve", RESOLVE, 1, "", "Resolve network-adresse (IP) of device"},
  {"Describe", DESCRIBE, 1, "", "Get Metadata of device"},
  {"List", LIST, 0, "", "List devices known to the supervisor"},
  {"Exit", EXITCMD, 0, "", "Exits the supervisor application"}
};

static void
new_device (struct soap *soap, struct ws4d_epr *device)
{
  WS4D_UNUSED_PARAM (soap);

  if (device && ws4d_epr_get_Addrs (device))
    {
      printf ("\nDiscovered New Device %s\n\n>> ",
              ws4d_epr_get_Addrs (device));

      ws4d_tc_lock (target_cache);
      ws4d_tc_checkadd_epr (target_cache, ws4d_epr_get_Addrs (device), -1);
      ws4d_tc_unlock (target_cache);
    }
}

static void
invalidate_device (struct soap *soap, struct ws4d_epr *device)
{
  WS4D_UNUSED_PARAM (soap);

  if (device && ws4d_epr_get_Addrs (device))
    {
      printf ("\nDevice removed %s\n\n>> ", ws4d_epr_get_Addrs (device));
    }
}

struct dpws_discovery_hooks discovery_hooks =
  { new_device, invalidate_device };

static void
print_help ()
{
  int numofcmds = sizeof (cmdloop_cmdlist) / sizeof (cmdloop_commands);
  int i;

  printf ("\n");
  printf ("***********************************\n");
  printf ("* gSOAP DPWS Supervisor Help Info *\n");
  printf ("***********************************\n");
  printf ("\n");
  printf ("Commands:\n");
  printf ("\n");

  for (i = 0; i < numofcmds; i++)
    {
      printf ("\t%s: (No. of args:%d)\n", cmdloop_cmdlist[i].str,
              cmdloop_cmdlist[i].numargs);
      printf ("\t\t%s\n", cmdloop_cmdlist[i].help);
      printf ("\n");
    }
}

static void
probe (int argc, char **argv)
{
  int ret;
  struct ws4d_abs_eprlist results;
  ws4d_alloc_list alist;
  ws4d_qnamelist type_list;

  ws4d_qnamelist_init (&type_list);
  WS4D_ALLOCLIST_INIT (&alist);

  /* initialize probe result list */
  ws4d_eprlist_init (&results, ws4d_eprllist_init, NULL);

  if (argc == 1)
    {
      ws4d_qnamelist_addstring (argv[0], &type_list, &alist);
    }

  printf ("\nProbing 10 sec for devices of type:\n\t%s", argc == 1 ? argv[0]
          : "any");
  ret =
    dpws_probe (&dpws, &type_list, NULL, 10000, 100, NULL, NULL, &results);

  if (ret == WS4D_OK)
    {
      struct ws4d_epr *epr = NULL, *iter = NULL;

      printf ("\nFound devices:");
      ws4d_eprlist_foreach (epr, iter, &results)
      {
        printf ("\n%s\n", ws4d_epr_get_Addrs (epr));
      }
    }
  else
    {
      if (ret == WS4D_TO)
        {
          printf ("\nNo device matches!");
        }
      else
        {
          printf ("\nError probing for devices!");
        }
    }

  ws4d_eprlist_done (&results);
  ws4d_qnamelist_done (&type_list);
  ws4d_alloclist_done (&alist);
}

static void
dprobe (int argc, char **argv)
{
  struct ws4d_epr *device = NULL;
  ws4d_alloc_list alist;
  ws4d_qnamelist type_list;
  int res = 0;

  ws4d_qnamelist_init (&type_list);
  WS4D_ALLOCLIST_INIT (&alist);

  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, argv[0]);

  if (argc == 2)
    {
      ws4d_qnamelist_addstring (argv[1], &type_list, &alist);
    }

  res = dpws_dprobe (&dpws, device, &type_list, NULL, NULL, 10000);
  if (res == WS4D_OK)
    {
      printf ("\nDevice matches\n");
    }
  else if (res == WS4D_TO)
    {
      printf ("\nDevices doesnt match\n");
    }
  else
    {
      printf ("\nError occured\n");
    }

  ws4d_epr_free (1, device);
  ws4d_alloclist_done (&alist);
}

static void
resolve (int argc, char **argv)
{
  ws4d_alloc_list alist;
  char *XAddrs = NULL;
  struct ws4d_epr *device = NULL;

  if (argc != 1)
    {
      printf ("\nwrong number of logical addresses to resolve\n");
    }

  WS4D_ALLOCLIST_INIT (&alist);
  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, argv[0]);

  XAddrs = (char *) dpws_resolve_addr (&dpws, device, NULL, 10000);
  if (XAddrs)
    {
      printf ("\nDevice %s resolves to %s\n", ws4d_epr_get_Addrs (device),
              XAddrs);
    }
  else
    {
      printf ("\nDevice %s cannot be resolved\n",
              ws4d_epr_get_Addrs (device));
    }

  ws4d_epr_free (1, device);
  ws4d_alloclist_done (&alist);
}

static void
list ()
{
  struct ws4d_abs_eprlist results;
  int ret = 0;

  /* initialize probe result list */
  ws4d_eprlist_init (&results, ws4d_eprllist_init, NULL);

  ret = ws4d_tc_get_targets (target_cache, &results);

  printf ("\nKnown Devices: %d\n", ret);

  if (ret > 0)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      printf ("\nFound devices:");
      ws4d_eprlist_foreach (epr, iter, &results)
      {
        char *XAddrs, *Types, *Scopes;
        int MetadataVersion = 0;

        printf ("\n%s\n", ws4d_epr_get_Addrs (epr));

        XAddrs = (char *) ws4d_targetep_get_XAddrs (epr);
        if (XAddrs)
          printf (" + XAddrs: %s\n", XAddrs);

        MetadataVersion = ws4d_targetep_get_MetadataVersion (epr);
        if (MetadataVersion != -1)
          printf (" + MetadataVersion: %d\n", MetadataVersion);

        Types = (char *) ws4d_targetep_get_Types (epr);
        if (Types)
          printf (" + Types: \"%s\"\n", Types);

        Scopes = (char *) ws4d_targetep_get_Scopes (epr);
        if (Scopes)
          printf (" + Scopes: \"%s\"\n", Scopes);
      }
    }

  ws4d_eprlist_done (&results);
}

static void
describe (int argc, char **argv)
{
  struct _wsm__Metadata metadata;

  struct ws4d_thisDevice ThisDevice;
  struct ws4d_thisModel ThisModel;

  struct ws4d_epr *device = NULL;
  struct ws4d_abs_eprlist services;

  struct soap client;

  ws4d_alloc_list alist;

  WS4D_UNUSED_PARAM (argc);

  WS4D_ALLOCLIST_INIT (&alist);

  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, argv[0]);

  /* initialize client soap handle */
  soap_init (&client);
  soap_omode (&client, SOAP_C_UTFSTRING | SOAP_XML_INDENT);

  dpws_handle_init (&dpws, &client);

  if (!dpws_metadata_get (&dpws, &client, device, &metadata, 10000))
    {
      soap_print_fault (&client, stdout);
    }

  if (!dpws_metadata_getThisDevice (&metadata, &alist, &ThisDevice))
    {
      printf ("\nDevice description:\n");
      if ((ThisDevice.__sizeFriendlyName > 0)
          && ws4d_locstring_get (ThisDevice.FriendlyName,
                                 ThisDevice.__sizeFriendlyName, "de"))
        {
          printf (" + FriendlyName (de): %s\n",
                  ws4d_locstring_get (ThisDevice.FriendlyName,
                                      ThisDevice.__sizeFriendlyName, "de"));
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

  if (!dpws_metadata_getThisModel (&metadata, &alist, &ThisModel))
    {
      printf ("\nModel description:\n");

      if ((ThisModel.__sizeManufacturer > 0)
          && ws4d_locstring_get (ThisModel.Manufacturer,
                                 ThisModel.__sizeManufacturer, "de"))
        {
          printf (" + Manufacturer (de): %s\n",
                  ws4d_locstring_get (ThisModel.Manufacturer,
                                      ThisModel.__sizeManufacturer, "de"));
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

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  if (!dpws_metadata_getservices (&dpws, device, &metadata, &services))
    {
      register struct ws4d_epr *elem, *iter;

      printf ("\nHosted Services:\n");

      ws4d_eprlist_foreach (elem, iter, &services)
      {
        if (elem && ws4d_epr_isvalid (elem))
          {
            char *types = NULL, *addrs = NULL;

            printf ("\n + %s\n", ws4d_serviceep_getid (elem));

            types =
              ws4d_qnamelist_tostring (ws4d_serviceep_gettypelist (elem),
                                       &alist);
            addrs = (char *) ws4d_epr_get_Addrs (elem);

            if (addrs)
              {
                printf ("    + Address: %s\n", addrs);
              }

            if (types)
              {
                printf ("    + Types: %s\n", types);
              }

          }
      }
    }

  ws4d_eprlist_done (&services);
  ws4d_epr_free (1, device);
  soap_end (&client);
  soap_done (&client);
  ws4d_alloclist_done (&alist);
}

static void
supervisor_exit ()
{
  printf ("\nShutting down ... ");
  running = 0;
}

static int
process_command (char *cmdline)
{
  char *cmd = NULL, *save_ptr = NULL, *args[MAX_NUM_ARGS] = { NULL };
  int cmdnum = -1;
  int numofcmds = sizeof (cmdloop_cmdlist) / sizeof (cmdloop_commands);
  int cmdfound = 0, num_args = 0, invalid_args = 0;
  int i;

  memset (args, 0, sizeof (char *) * MAX_NUM_ARGS);

  while (memchr (cmdline, '\n', strlen (cmdline)))
    {
      char *ret = NULL;
      ret = memchr (cmdline, '\n', strlen (cmdline));
      ret[0] = '\0';
    }

  cmd = STRTOK (cmdline, " ", &save_ptr);

  do
    {
      args[num_args] = STRTOK (NULL, " ", &save_ptr);
      if (args[num_args] != NULL)
        {
          num_args++;
        }
      else
        {
          break;
        }
    }
  while (num_args < MAX_NUM_ARGS);

  for (i = 0; i < numofcmds; i++)
    {
      if (cmd && STRCASECMP (cmd, cmdloop_cmdlist[i].str) == 0)
        {
          cmdnum = cmdloop_cmdlist[i].cmdnum;
          cmdfound++;
          if (num_args < cmdloop_cmdlist[i].numargs)
            {
              invalid_args = 1;
            }
          break;
        }
    }

  if (!cmdfound)
    {
      printf ("\n\nCommand not found\n");
      print_help ();
      return 0;
    }

  if (invalid_args)
    {
      printf ("\n\nInvalid arguments\n");
      print_help ();
      return 0;
    }

  switch (cmdnum)
    {
    case PRTHELP:
      print_help ();
      break;
    case PROBE:
      probe (num_args, args);
      break;
    case DPROBE:
      dprobe (num_args, args);
      break;
    case RESOLVE:
      resolve (num_args, args);
      break;
    case DESCRIBE:
      describe (num_args, args);
      break;
    case LIST:
      list ();
      break;
    case EXITCMD:
      supervisor_exit ();
      break;
    default:
      printf ("\n\nInvalid command\n");
      print_help ();
      break;
    }

  return 0;
}

static void *
command_loop (void *ptr)
{
  char cmdline[1024];

  WS4D_UNUSED_PARAM (ptr);

  while (running)
    {
      printf ("\n>> ");
      fgets (cmdline, 1024, stdin);
      process_command (cmdline);
    }

#ifdef WIN32
  ExitThread (0);
#else
  pthread_exit (NULL);
#endif
}

int
main (int argc, char **argv)
{
#ifdef WIN32
  DWORD cmdloop;
  char pData[3];
#else
  pthread_t cmdloop;
  struct sigaction sa;
#endif
  int err = 0;

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
              printf ("\nSet host to \"%s\"", host);
              fflush (NULL);
              break;
            default:
              fprintf (stderr, "\nBad option %s", argv[1]);
            }
        }
      --argc;
      ++argv;
    }

  if (host == NULL)
    {
      printf ("\nNo host was specified!");
      fflush (NULL);
      exit (0);
    }

  /* initialize target cache */
  target_cache = dpws_create_tc (NULL);

  /* initialize client soap handle */
  soap_init (&discovery);
  soap_omode (&discovery, SOAP_C_UTFSTRING | SOAP_XML_INDENT);

  /* initialize stack */
  if (dpws_init2 (&dpws, host, target_cache) != SOAP_OK)
    {
      dpws_done (&dpws);
      exit (0);
    }

  if (dpws_init_implicit_discovery2
      (&dpws, target_cache, &discovery, &discovery_hooks) != SOAP_OK)
    {
      dpws_done (&dpws);
      exit (0);
    }
#ifdef WIN32
  CreateThread (NULL, 0, (DWORD WINAPI) command_loop, &pData, 0, &cmdloop);
#else
  pthread_create (&cmdloop, NULL, command_loop, NULL);
#endif

  /* wait for SIGINT */
#ifdef WIN32
  signal (SIGINT, supervisor_exit);
#else
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = supervisor_exit;
  sigaction (SIGINT, &sa, NULL);
#endif

  while (running)
    {
      err = dpws_discovery_process (&discovery, 1000);
      if ((err != SOAP_EOF) && (err != SOAP_OK) && (err != SOAP_STOP))
        {
          soap_print_fault (&discovery, stderr);
        }
    }

  /* wait for threads */
  SLEEP (1);
  printf ("done\n");

  soap_end (&discovery);
  soap_done (&discovery);
  dpws_done (&dpws);

  return 0;

}
