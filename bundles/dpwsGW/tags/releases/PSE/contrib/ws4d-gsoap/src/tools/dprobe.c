/* dprobe - send directed probe request to a dpws device
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
#include "dpws_client.h"
#include "ws4d_targetcache.h"

struct dpws_s dpws;

int
main (int argc, char **argv)
{
  ws4d_alloc_list alist;

  int probeTime = 5500;
  int res = 0;
  int exitstatus = 0;

  struct ws4d_epr *device = NULL;

  ws4d_qnamelist type_list;

  char *host = NULL;
  char *uuid = NULL;
  char *timeout = NULL;
  char *scope = NULL;
  char *type = NULL;
  char *matching = NULL;

  /* init allocation list */
  WS4D_ALLOCLIST_INIT (&alist);

  /* init type list */
  ws4d_qnamelist_init (&type_list);

  /* parsing command line options */
  while (argc > 1)
    {
      if (argv[1][0] == '-')
        {
          char *option = &argv[1][1];
          switch (option[0])
            {
            case 'i':          /* set host */
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
              break;

            case 'd':          /* set device id */
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
              break;

            case 's':          /* set scope */
              if (strlen (option) > 2)
                {
                  ++option;
                  scope = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  scope = argv[1];
                }
              break;

            case 't':          /* set type */
              type = NULL;
              if (strlen (option) > 2)
                {
                  ++option;
                  type = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  type = argv[1];
                }
              if (type)
                {
                  ws4d_qnamelist_addstring (type, &type_list, &alist);
                }
              break;

            case 'p':          /* set timeout period */
              if (strlen (option) > 2)
                {
                  ++option;
                  timeout = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  timeout = argv[1];
                }
              probeTime = atoi (timeout);
              break;

            case 'm':          /* set matching rule */
              if (strlen (option) > 2)
                {
                  ++option;
                  matching = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  matching = argv[1];
                }
              break;

            default:
              fprintf (stderr, "\nBad option %s", argv[1]);
              fflush (NULL);
              exit (1);
            }
        }
      --argc;
      ++argv;
    }

  /* stop if no host is given */
  if (!host)
    {
      fprintf (stderr, "\nNo host was specified!");
      fflush (NULL);
      exit (1);
    }

  /* stop if no device id is given */
  if (!uuid)
    {
      fprintf (stderr, "\nNo device ID was specified!");
      fflush (NULL);
      exit (1);
    }

  /* initialize stack */
  if (dpws_init (&dpws, host) != SOAP_OK)
    {
      fprintf (stderr, "\nCould not initialize dpws handle");
      fflush (NULL);
      dpws_done (&dpws);
      exit (1);
    }

  /* allocate and initialize device */
  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, uuid);

  /* start probe */
  res = dpws_dprobe (&dpws, device, &type_list, scope, NULL, probeTime);
  printf ("\n%s\n", ws4d_epr_get_Addrs (device));
  printf (" + XAddrs: %s\n", dpws_resolve_addr (&dpws, device, NULL, 10000));
  printf (" + Types: %s\n", ws4d_targetep_get_Types (device));
  printf (" + Scopes: %s\n", ws4d_targetep_get_Scopes (device));
  printf (" + MetadataVersion: %d\n",
          ws4d_targetep_get_MetadataVersion (device));

  if (res != WS4D_OK)
    {
      if (res == WS4D_TO)
        {
          printf ("doesn't match\n");
        }
      else
        {
          printf ("doesn't respond\n");
        }
      exitstatus = 1;
    }
  else
    {
      printf ("matches\n");
    }

  dpws_done (&dpws);

  return exitstatus;
}
