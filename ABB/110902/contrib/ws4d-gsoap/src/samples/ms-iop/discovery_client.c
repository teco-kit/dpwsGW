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

#include "discovery_client.h"

int
wait_for_hello (struct dpws_s *dpws, struct ws4d_tc_int *tc, const char *uuid,
                int tries, int wait)
{
  struct ws4d_abs_eprlist results;
  int found_device = 0;
  int ret;

  WS4D_UNUSED_PARAM (dpws);

  /* initialize probe result list */
  ws4d_eprlist_init (&results, ws4d_eprllist_init, NULL);

  while ((tries > 0) && !found_device)
    {
      printf ("Try %d\n", tries);

      ret = ws4d_tc_get_targets (tc, &results);

      if (ret > 0)
        {
          register struct ws4d_epr *epr = NULL, *iter = NULL;

          ws4d_eprlist_foreach (epr, iter, &results)
          {
            if (!strcmp (uuid, ws4d_epr_get_Addrs (epr)))
              {
                char *Types, *Scopes, *XAddrs;
                int MetadataVersion = 0;

                found_device = 1;

                printf ("\nFound device %s\n", ws4d_epr_get_Addrs (epr));

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

                break;
              }
          }

        }
      tries--;
      SLEEP (wait);
    }

  ws4d_eprlist_done (&results);
  return found_device;
}

int
wait_for_epr_invalid (struct dpws_s *dpws, struct ws4d_tc_int *tc,
                      const char *uuid, int tries, int wait)
{
  int ret, got_bye = 0;
  struct ws4d_epr device;

  WS4D_UNUSED_PARAM (dpws);

  ws4d_epr_init (&device);
  ret = ws4d_tc_get_byAddr (tc, uuid, &device);

  while ((tries > 0) && !got_bye)
    {
      printf ("Try %d\n", tries);
      if (device.valid != 1)
        {
          got_bye = 1;
        }
      tries--;
      SLEEP (wait);
    }

  ws4d_epr_done (&device);
  return got_bye;
}

int
do_resolve (struct dpws_s *dpws, struct ws4d_tc_int *tc, const char *uuid)
{
  char *XAddrs = NULL;
  struct ws4d_epr device;
  int resolved = 0;

  WS4D_UNUSED_PARAM (tc);

  ws4d_epr_init (&device);
  ws4d_epr_set_Addrs (&device, uuid);

  XAddrs = (char *) dpws_resolve_addr (dpws, &device, NULL, 10000);
  if (XAddrs)
    {
      printf ("\nDevice %s resolves to %s\n", ws4d_epr_get_Addrs (&device),
              XAddrs);
      resolved = 1;
    }
  else
    {
      printf ("\nDevice %s cannot be resolved\n",
              ws4d_epr_get_Addrs (&device));
    }

  ws4d_epr_done (&device);

  return resolved;
}

int
do_probe (struct dpws_s *dpws, struct ws4d_tc_int *tc, const char *uuid)
{
  int found_device = 0;
  struct ws4d_abs_eprlist results;
  ws4d_qnamelist type_list;
  int ret;

  ws4d_qnamelist_init (&type_list);
  ws4d_eprlist_init (&results, ws4d_eprllist_init, NULL);

  ret = dpws_probe (dpws, &type_list, "", 2500, 1000, NULL, tc, &results);
  if ((ret == WS4D_OK))
    {
      register struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &results)
      {
        if (!strcmp (uuid, ws4d_epr_get_Addrs (epr)))
          {
            char *Types, *Scopes, *XAddrs;
            int MetadataVersion = 0;

            found_device = 1;

            printf ("\nFound device %s\n", ws4d_epr_get_Addrs (epr));

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

            break;
          }
      }
    }
  else
    {
      if (ret == WS4D_TO)
        {
          printf ("\nNo device matches\n");
          fflush (NULL);
        }
      else
        {
          printf ("\nCould not probe for devices\n");
          fflush (NULL);
        }
    }

  return found_device;
}

int
do_notmatching_dprobe (struct dpws_s *dpws, struct ws4d_epr *device)
{
  int not_matching = 0;
  ws4d_qnamelist type_list;
  int ret;

  ws4d_qnamelist_init (&type_list);

  ret =
    dpws_dprobe (dpws, device, &type_list, "http://this.wontmatch.org", NULL,
                 2000);
  if (ret != WS4D_OK)
    {
      if (ret == WS4D_TO)
        {
          printf ("\nDevice %s doesn't match\n", ws4d_epr_get_Addrs (device));
          not_matching = 1;
        }
      else
        {
          printf ("\nDevice %s doesn't respond\n",
                  ws4d_epr_get_Addrs (device));
        }
    }
  else
    {
      printf ("\nDevice %s matches\n", ws4d_epr_get_Addrs (device));
    }

  return not_matching;
}
