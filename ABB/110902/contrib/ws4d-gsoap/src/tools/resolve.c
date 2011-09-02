/* resolve - resolves the logical address of dpws devices
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

  struct ws4d_epr *device = NULL;

  int ResolveTime = 5500;

  char *XAddrs = NULL;

  char *host = NULL;
  char *uuid = NULL;
  char *timeout = NULL;

  /* init allocation list */
  WS4D_ALLOCLIST_INIT (&alist);

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
              ResolveTime = atoi (timeout);
              break;

            default:
              fprintf (stderr, "\nBad option %s\n", argv[1]);
              fflush (NULL);
              exit (1);
            }
        }
      --argc;
      ++argv;
    }

  if (!host)
    {
      fprintf (stderr, "\nNo host was specified!");
      fflush (NULL);
      exit (1);
    }

  if (uuid == NULL)
    {
      fprintf (stderr, "\nNo UUID was specified!");
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

  /* allocate and prepare device to resolve */
  device = ws4d_epr_alloc (1, &alist);
  ws4d_epr_set_Addrs (device, uuid);

  /* start resolve */
  XAddrs = (char *) dpws_resolve_addr (&dpws, device, NULL, ResolveTime);

  if (!XAddrs)
    {
      fprintf (stderr, "\nDevice %s cannot be resolved\n",
               ws4d_epr_get_Addrs (device));
      fflush (NULL);
      exit (1);
    }

  printf ("%s\n", XAddrs);
  fflush (NULL);

  exit (0);
}
