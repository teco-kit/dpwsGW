/* test_driver - command line tool for the interoperability test
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

#include "discovery_client.h"
#include "description_client.h"
#include "simple_service_client.h"
#include "attachment_service_client.h"
#include "eventing_service_client.h"

struct soap discovery, event_handler;
struct dpws_s dpws;

struct ws4d_tc_int *target_cache;

char *host = NULL;
char *uuid = NULL;
static int running = 1;
int last_command = 1;

#define MAX_NUM_ARGS 5
enum cmdloop_cmds
{
  PRTHELP = 0,
  NEXT_TC,
  TC_1_1,
  TC_1_2,
  TC_1_3,
  TC_1_4,
  TC_1_5,
  TC_1_6,
  TC_2_1,
  TC_2_2,
  TC_2_3,
  TC_3_1,
  TC_3_2,
  TC_4_1,
  TC_4_2,
  TC_4_3,
  TC_4_4,
  EXITCMD,
  LASTCMD
};

typedef void (*cmd_func_t) (int argc, char **argv);


struct cmdloop_commands
{
  const char *str;              /* the string */
  int cmdnum;                   /* the command */
  int numargs;                  /* the number of arguments */
  const char *help;             /* help for the commands */
  cmd_func_t cmd;               /* function implementing the command */
} cmdloop_commands;

static void
new_device (struct soap *soap, struct ws4d_epr *device)
{
  WS4D_UNUSED_PARAM (soap);

  if (device && ws4d_epr_get_Addrs (device))
    {
      ws4d_tc_lock (target_cache);
      ws4d_tc_checkadd_epr (target_cache, ws4d_epr_get_Addrs (device),
                            ws4d_targetep_get_MetadataVersion (device));
      ws4d_tc_unlock (target_cache);
    }
}

static void
invalidate_device (struct soap *soap, struct ws4d_epr *device)
{
  WS4D_UNUSED_PARAM (soap);

  ws4d_tc_invalidate_epr (target_cache, ws4d_epr_get_Addrs (device));
}

struct dpws_discovery_hooks discovery_hooks =
  { new_device, invalidate_device };

static void print_help (int argc, char **argv);

static void next_tc (int argc, char **argv);

static void
tc_1_1 (int argc, char **argv)
{
  struct ws4d_epr device;
  int ret;

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  printf ("\nTest case 1.1.1\n");
  printf ("Waiting 60 seconds for hello of device %s\n", uuid);

  if (wait_for_hello (&dpws, target_cache, uuid, 60, 2))
    {
      printf ("\nPASSED: Test case 1.1.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.1.1\n");
    }
  fflush (NULL);

  printf ("\nTest case 1.1.2\n");
  printf ("Send Resolve to %s\n", uuid);

  if (do_resolve (&dpws, target_cache, uuid))
    {
      printf ("\nPASSED: Test case 1.1.2\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.1.2\n");
    }
  fflush (NULL);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.1.3\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.1.3\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.1.3\n");
    }
  fflush (NULL);

  printf ("\nTest case 1.1.4\n");
  printf ("Waiting 50 seconds for bye of device %s\n", uuid);

  if (wait_for_epr_invalid (&dpws, target_cache, uuid, 50, 1))
    {
      printf ("\nPASSED: Test case 1.1.4\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.1.4\n");
    }
  fflush (NULL);
  ws4d_epr_done (&device);

  printf ("\nTest case 1.1.5\n");
  printf ("Waiting 110 seconds for hello of device %s\n", uuid);

  if (wait_for_hello (&dpws, target_cache, uuid, 50, 2))
    {
      printf ("\nPASSED: Test case 1.1.5\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.1.5\n");
    }
  fflush (NULL);

  printf ("\nTest case 1.1.7\n");
  printf ("Send Resolve to %s\n", uuid);

  if (do_resolve (&dpws, target_cache, uuid))
    {
      printf ("\nPASSED: Test case 1.1.7\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.1.7\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.1.8\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.1.8\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.1.8\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
}

static void
tc_1_2 (int argc, char **argv)
{
  struct ws4d_epr device;
  int ret;

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  printf ("\nTest case 1.2.1\n");
  printf ("Send Resolve to %s\n", uuid);

  if (do_resolve (&dpws, target_cache, uuid))
    {
      printf ("\nPASSED: Test case 1.2.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.2.1\n");
    }
  fflush (NULL);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.2.2\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.2.2\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.2.2\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
}

static void
tc_1_3 (int argc, char **argv)
{
  struct ws4d_epr device;
  int ret;

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  printf ("\nTest case 1.3.1\n");
  printf ("Sending probe\n");

  if (do_probe (&dpws, target_cache, uuid))
    {
      printf ("\nPASSED: Test case 1.3.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.3.1\n");
    }
  fflush (NULL);

  printf ("\nTest case 1.3.2\n");
  printf ("Resolving device %s\n", uuid);

  if (do_resolve (&dpws, target_cache, uuid))
    {
      printf ("\nPASSED: Test case 1.3.2\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.3.2\n");
    }
  fflush (NULL);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.3.3\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.3.3\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.3.3\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
}

static void
tc_1_4 (int argc, char **argv)
{
  struct ws4d_epr device;
  int ret;

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  printf ("\nTest case 1.4.1\n");
  printf ("Sending probe\n");

  if (do_probe (&dpws, target_cache, uuid))
    {
      printf ("\nPASSED: Test case 1.4.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.4.1\n");
    }
  fflush (NULL);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.4.2\n");
  printf ("Sending not matching dprobe\n");

  if (do_notmatching_dprobe (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.4.2\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.4.2\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
}

static void
tc_1_5 (int argc, char **argv)
{
  struct ws4d_epr device;
  int ret;

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.5.1\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_verbose_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.5.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.5.1\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
}

static void
tc_1_6 (int argc, char **argv)
{
  struct ws4d_epr device;
  int ret;

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  printf ("\nTest case 1.6.1\n");
  printf ("Waiting 5 seconds for hello of device %s\n", uuid);

  if (wait_for_hello (&dpws, target_cache, uuid, 5, 2))
    {
      printf ("\nPASSED: Test case 1.6.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.6.1\n");
    }
  fflush (NULL);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.6.2\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_verbose_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.6.2\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.6.2\n");
    }
  fflush (NULL);

  printf ("\nTest case 1.6.4\n");
  printf ("Waiting 50 seconds for new hello of device %s\n", uuid);

  if (wait_for_epr_invalid (&dpws, target_cache, uuid, 50, 1))
    {
      printf ("\nPASSED: Test case 1.6.4\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.6.4\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 1.6.5\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_verbose_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 1.6.5\n");
    }
  else
    {
      printf ("\nFAILED: Test case 1.6.5\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
}

static void
tc_2_1 (int argc, char **argv)
{
  const char *service_id = "http://testdevice.example.org/SimpleService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  printf ("\nTest case 2.1.1\n");

  if (call_simpleservice_oneway_method (&dpws, uuid, service_id))
    {
      printf ("\nPASSED: Test case 2.1.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 2.1.1\n");
    }
  fflush (NULL);
}

static void
tc_2_2 (int argc, char **argv)
{
  const char *service_id = "http://testdevice.example.org/SimpleService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  printf ("\nTest case 2.2.1\n");

  if (call_simpleservice_twoway_method (&dpws, uuid, service_id))
    {
      printf ("\nPASSED: Test case 2.2.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 2.2.1\n");
    }
  fflush (NULL);
}

static void
tc_2_3 (int argc, char **argv)
{
  struct ws4d_epr device;
  int ret;
  const char *service_id = "http://testdevice.example.org/SimpleService2";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  printf ("\nTest case 2.3.2\n");
  printf ("Waiting 50 seconds for new hello of device %s\n", uuid);

  if (wait_for_epr_invalid (&dpws, target_cache, uuid, 50, 1))
    {
      printf ("\nPASSED: Test case 2.3.2\n");
    }
  else
    {
      printf ("\nFAILED: Test case 2.3.2\n");
    }
  fflush (NULL);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);

  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  printf ("\nTest case 2.3.3\n");
  printf ("Getting meta data from %s\n", uuid);

  if (do_verbose_getMetadata (&dpws, &device))
    {
      printf ("\nPASSED: Test case 2.3.3\n");
    }
  else
    {
      printf ("\nFAILED: Test case 2.3.3\n");
    }
  fflush (NULL);

  printf ("\nTest case 2.3.4\n");

  if (call_simpleservice_twoway_method (&dpws, uuid, service_id))
    {
      printf ("\nPASSED: Test case 2.3.4\n");
    }
  else
    {
      printf ("\nFAILED: Test case 2.3.4\n");
    }
  fflush (NULL);

  ws4d_epr_done (&device);
}

static void
tc_3_1 (int argc, char **argv)
{
  const char *service_id = "http://testdevice.example.org/AttachmentService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  printf ("\nTest case 3.1.1\n");

  if (call_attachmentservice_oneway_method (&dpws, uuid, service_id))
    {
      printf ("\nPASSED: Test case 3.1.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 3.1.1\n");
    }
  fflush (NULL);
}

static void
tc_3_2 (int argc, char **argv)
{
  const char *service_id = "http://testdevice.example.org/AttachmentService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  printf ("\nTest case 3.2.1\n");

  if (call_attachmentservice_twoway_method (&dpws, uuid, service_id))
    {
      printf ("\nPASSED: Test case 3.2.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 3.2.1\n");
    }
  fflush (NULL);

  printf ("\nTest case 3.2.2\n");

  /*TODO: Test case 3.2.2 */
}

static void
tc_4_1 (int argc, char **argv)
{
  const char *subsid = NULL;
  struct ws4d_epr device;
  struct ws4d_epr *service;
  struct ws4d_abs_eprlist services;
  ws4d_qnamelist service_type_list;
  int ret;
  const char *service_id = "http://testdevice.example.org/EventingService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);
  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (&dpws, &device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), service_id))
          {
            service = epr;
            printf ("\nDevice offers EventingService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
    }
  else
    {
      printf ("\nEventingService not found on %s\n",
              ws4d_epr_get_Addrs (&device));
      return;
    }

  printf ("\nTest case 4.1.1\n");
  printf ("Subscribing to simple event\n");

  subsid =
    do_subscribe (service,
                  "http://schemas.example.org/EventingService/SimpleEvent",
                  event_handler_geturi (), 0);
  if (subsid)
    {
      printf ("\nPASSED: Test case 4.1.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.1.1\n");
    }
  fflush (NULL);

  printf ("\nTest case 4.1.2\n");
  printf ("Waiting 60 seconds for event\n");

  SLEEP (20);

  printf ("\nTest case 4.1.3\n");
  printf ("Renew subscription %s\n", subsid);

  if (do_renew (service, subsid, 0))
    {
      printf ("\nPASSED: Test case 4.1.3\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.1.3\n");
    }
  fflush (NULL);

  printf ("\nTest case 4.1.4\n");
  printf ("Waiting 60 seconds for event\n");
  SLEEP (20);

  printf ("\nTest case 4.1.5\n");
  printf ("Unsubscribe %s\n", subsid);

  if (do_unsubscribe (service, subsid))
    {
      printf ("\nPASSED: Test case 4.1.5\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.1.5\n");
    }
  fflush (NULL);

  printf ("\nTest case 4.1.6\n");
  printf ("Nothing to do here");

  ws4d_eprlist_done (&services);
  ws4d_epr_done (&device);
}

static void
tc_4_2 (int argc, char **argv)
{
  const char *subsid = NULL;
  struct ws4d_epr device;
  struct ws4d_epr *service;
  struct ws4d_abs_eprlist services;
  ws4d_qnamelist service_type_list;
  int ret;
  const char *service_id = "http://testdevice.example.org/EventingService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);
  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (&dpws, &device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), service_id))
          {
            service = epr;
            printf ("\nDevice offers EventingService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
    }
  else
    {
      printf ("\nEventingService not found on %s\n",
              ws4d_epr_get_Addrs (&device));
      return;
    }

  printf ("\nTest case 4.2.1\n");
  printf ("Subscribing to simple event\n");

  subsid =
    do_subscribe (service,
                  "http://schemas.example.org/EventingService/SimpleEvent",
                  event_handler_geturi (), 3600);
  if (subsid)
    {
      printf ("\nPASSED: Test case 4.2.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.2.1\n");
      printf ("can't proceed without subsription\n");
      return;
    }
  fflush (NULL);

  printf ("\nTest case 4.2.2\n");
  printf ("Waiting 60 seconds for event\n");
  SLEEP (30);

  printf ("\nTest case 4.2.3\n");
  printf ("Renew subscription %s\n", subsid);

  if (do_renew (service, subsid, 3600))
    {
      printf ("\nPASSED: Test case 4.2.3\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.2.3\n");
    }
  fflush (NULL);

  printf ("\nTest case 4.2.4\n");
  printf ("Waiting 60 seconds for event\n");
  SLEEP (30);

  ws4d_eprlist_done (&services);
  ws4d_epr_done (&device);
}

static void
tc_4_3 (int argc, char **argv)
{
  const char *subsid = NULL;
  struct ws4d_epr device;
  struct ws4d_epr *service;
  struct ws4d_abs_eprlist services;
  ws4d_qnamelist service_type_list;
  int ret;
  const char *service_id = "http://testdevice.example.org/EventingService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);
  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (&dpws, &device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), service_id))
          {
            service = epr;
            printf ("\nDevice offers EventingService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
    }
  else
    {
      printf ("\nEventingService not found on %s\n",
              ws4d_epr_get_Addrs (&device));
      return;
    }

  printf ("\nTest case 4.3.1\n");
  printf ("Subscribing to simple event\n");

  subsid =
    do_subscribe (service,
                  "http://schemas.example.org/EventingService/SimpleEvent http://schemas.example.org/EventingService/IntegerEvent",
                  event_handler_geturi (), 40);
  if (subsid)
    {
      printf ("\nPASSED: Test case 4.3.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.3.1\n");
      printf ("can't proceed without subscription\n");
      return;
    }
  fflush (NULL);

  printf ("\nTest case 4.3.2\n");
  printf ("Waiting 60 seconds for event\n");
  SLEEP (30);

  printf ("\nTest case 4.3.3\n");
  printf ("Unsubscribe %s\n", subsid);

  if (do_unsubscribe (service, subsid))
    {
      printf ("\nPASSED: Test case 4.3.3\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.3.3\n");
    }
  fflush (NULL);

  printf ("\nTest case 4.3.4\n");
  printf ("Waiting 60 seconds for event\n");
  SLEEP (30);

  ws4d_eprlist_done (&services);
  ws4d_epr_done (&device);
}

static void
tc_4_4 (int argc, char **argv)
{
  const char *subsid = NULL;
  struct ws4d_epr device;
  struct ws4d_epr *service;
  struct ws4d_abs_eprlist services;
  ws4d_qnamelist service_type_list;
  int ret;
  const char *service_id = "http://testdevice.example.org/EventingService1";

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

  if ((argc == 1) && argv[0])
    {
      service_id = argv[0];
    }
  printf ("Using service id %s\n", service_id);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (target_cache, uuid, &device);
  if (ret != WS4D_OK)
    {
      printf ("can't proceed testing because device %s was not found\n",
              uuid);
      return;
    }

  ws4d_eprlist_init (&services, ws4d_eprllist_init, NULL);
  ws4d_qnamelist_init (&service_type_list);

  if (dpws_find_services (&dpws, &device, &service_type_list,
                          10000, &services) == SOAP_OK)
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &services)
      {
        if (!strcmp (ws4d_serviceep_getid (epr), service_id))
          {
            service = epr;
            printf ("\nDevice offers EventingService at %s\n",
                    ws4d_epr_get_Addrs (service));
            break;
          }
      }
    }
  else
    {
      printf ("\nEventingService not found on %s\n",
              ws4d_epr_get_Addrs (&device));
      return;
    }

  printf ("\nTest case 4.4.1\n");
  printf ("Subscribing to simple event\n");

  subsid =
    do_subscribe (service,
                  "http://schemas.example.org/EventingService/SimpleEvent http://schemas.example.org/EventingService/IntegerEvent",
                  event_handler_geturi (), 10);
  if (subsid)
    {
      printf ("\nPASSED: Test case 4.4.1\n");
    }
  else
    {
      printf ("\nFAILED: Test case 4.4.1\n");
      printf ("can't proceed without subscription\n");
      return;
    }
  fflush (NULL);

  printf ("Waiting 30 seconds for expiration of subscription\n");
  SLEEP (30);

  printf ("\nTest case 4.4.2\n");
  printf ("Renew expired subscription %s\n", subsid);

  if (do_renew (service, subsid, 3600))
    {
      printf ("\nFAILED: Test case 4.4.2\n");
    }
  else
    {
      printf ("\nPASSED: Test case 4.4.2\n");
    }
  fflush (NULL);

  ws4d_eprlist_done (&services);
  ws4d_epr_done (&device);
}

#ifdef WIN32
static void
_test_driver_exit (int d)
#else
static void
_test_driver_exit ()
#endif
{
#ifdef WIN32
  WS4D_UNUSED_PARAM (d);
#endif

  printf ("\nShutting down ... ");
  running = 0;
}

static void
test_driver_exit (int argc, char **argv)
{
  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

#ifdef WIN32
  _test_driver_exit (0);
#else
  _test_driver_exit ();
#endif
}

static struct cmdloop_commands cmdloop_cmdlist[] = {
  {"help", PRTHELP, 0, "Print this help info", print_help},
  {"next", NEXT_TC, 0, "Next test case", next_tc},
  {"tc_1_1", TC_1_1, 0, "Test case 1.1", tc_1_1},
  {"tc_1_2", TC_1_2, 0, "Test case 1.2", tc_1_2},
  {"tc_1_3", TC_1_3, 0, "Test case 1.3", tc_1_3},
  {"tc_1_4", TC_1_4, 0, "Test case 1.4", tc_1_4},
  {"tc_1_5", TC_1_5, 0, "Test case 1.5", tc_1_5},
  {"tc_1_6", TC_1_6, 0, "Test case 1.6", tc_1_6},
  {"tc_2_1", TC_2_1, 0, "Test case 2.1", tc_2_1},
  {"tc_2_2", TC_2_2, 0, "Test case 2.2", tc_2_2},
  {"tc_2_3", TC_2_3, 0, "Test case 2.3", tc_2_3},
  {"tc_3_1", TC_3_1, 0, "Test case 3.1", tc_3_1},
  {"tc_3_2", TC_3_2, 0, "Test case 3.2", tc_3_2},
  {"tc_4_1", TC_4_1, 0, "Test case 4.1", tc_4_1},
  {"tc_4_2", TC_4_2, 0, "Test case 4.2", tc_4_2},
  {"tc_4_3", TC_4_3, 0, "Test case 4.3", tc_4_3},
  {"tc_4_4", TC_4_4, 0, "Test case 4.4", tc_4_4},
  {"exit", EXITCMD, 0, "Exits the test_driver application", test_driver_exit}
};

static void
print_help (int argc, char **argv)
{
  int numofcmds = LASTCMD;
  int i;

  WS4D_UNUSED_PARAM (argc);
  WS4D_UNUSED_PARAM (argv);

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
next_tc (int argc, char **argv)
{
  last_command++;
  cmdloop_cmdlist[last_command].cmd (argc, argv);
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
      if (cmd && *cmd)
        {
          printf ("\n\nCommand not found\n");
          print_help (0, NULL);
        }
      else
        {
          next_tc (0, NULL);
        }
      return 0;
    }

  if (invalid_args)
    {
      printf ("\n\nInvalid arguments\n");
      print_help (0, NULL);
      return 0;
    }

  cmdloop_cmdlist[cmdnum].cmd (num_args, args);
  if (cmdnum != NEXT_TC)
    {
      last_command = cmdnum;
    }

  return 0;
}

#ifdef WIN32
static void *
command_loop (LPVOID ptr)
#else
static void *
command_loop (void *ptr)
#endif
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

int evs_serve_request (struct soap *soap);

int
main (int argc, char **argv)
{
#ifdef WIN32
  DWORD cmdloop;
  char pData[3];
  HANDLE hthread;
#else
  pthread_t cmdloop;
  struct sigaction sa;
#endif

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
              printf ("\nUsing device \"%s\" for testing", uuid);
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
  hthread =
    CreateThread (NULL, 0, (DWORD WINAPI) command_loop, &pData, 0, &cmdloop);
#else
  pthread_create (&cmdloop, NULL, command_loop, NULL);
#endif

  event_handler_init (&dpws, &event_handler);

  /* wait for SIGINT */
#ifdef WIN32
  signal (SIGINT, _test_driver_exit);
#else
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = _test_driver_exit;
  sigaction (SIGINT, &sa, NULL);
#endif

  while (running)
    {
      struct soap *handle = NULL;
      struct soap *soap_set[] = SOAP_HANDLE_SET (&discovery, &event_handler);
      int (*serve_requests[]) (struct soap * soap) =
        SOAP_SERVE_SET (evs_serve_request);

      handle = dpws_maccept (&dpws, 1000, 2, soap_set);

      if (handle)
        {
          if (dpws_mserve (handle, 1, serve_requests))
            {
              soap_print_fault (handle, stderr);
            }

          /* clean up soaps internaly allocated memory */
          soap_end (handle);
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
