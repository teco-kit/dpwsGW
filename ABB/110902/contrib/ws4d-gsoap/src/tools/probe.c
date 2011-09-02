/* probe - probe for dpws devices
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

#include "ws4d_eprllist.h"

struct dpws_s dpws;

int
main (int argc, char **argv)
{
  ws4d_alloc_list alist;

  int ret = 0;

  struct ws4d_abs_eprlist results;

  int probeTime = 5500;

  ws4d_qnamelist type_list;

  char *host = NULL;
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

  /* Stop if no host is given */
  if (!host)
    {
      fprintf (stderr, "\nNo host was specified!");
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

  /* initialize probe result list */
  ws4d_eprlist_init (&results, ws4d_eprllist_init, NULL);

  /* start probe */
  ret = dpws_probe (&dpws, &type_list, scope, probeTime, 1000, NULL, NULL,
                    &results);
  if ((ret == WS4D_OK))
    {
      struct ws4d_epr *epr = NULL, *iter = NULL;

      ws4d_eprlist_foreach (epr, iter, &results)
      {
        printf ("%s\n", ws4d_epr_get_Addrs (epr));
        printf (" + XAddrs: %s\n",
                dpws_resolve_addr (&dpws, epr, NULL, 10000));
        printf (" + Types: %s\n", ws4d_targetep_get_Types (epr));
        printf (" + Scopes: %s\n", ws4d_targetep_get_Scopes (epr));
        printf (" + MetadataVersion: %d\n\n",
                ws4d_targetep_get_MetadataVersion (epr));
      }
    }
  else
    {
      if (ret == WS4D_TO)
        {
          fprintf (stderr, "\nNo device matches\n");
          fflush (NULL);
          dpws_done (&dpws);

        }
      else
        {
          fprintf (stderr, "\nCould not probe for devices\n");
          fflush (NULL);
          dpws_done (&dpws);
          exit (1);
        }
    }

  return 0;
}
