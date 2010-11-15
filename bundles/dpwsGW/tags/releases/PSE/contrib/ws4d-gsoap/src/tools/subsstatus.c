/* subsstatus - check the status of a subscription at a dpws device
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

struct dpws_s dpws;
struct soap client;

int
main (int argc, char **argv)
{
  ws4d_alloc_list alist;
  char *host = NULL;
  char *subsman = NULL;
  char *id = NULL;

  ws4d_time duration;

  WS4D_ALLOCLIST_INIT (&alist);

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
              fflush (NULL);
              break;
            case 'm':
              if (strlen (option) > 2)
                {
                  ++option;
                  subsman = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  subsman = argv[1];
                }
              fflush (NULL);
              break;
            case 'n':
              if (strlen (option) > 2)
                {
                  ++option;
                  id = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  id = argv[1];
                }
              fflush (NULL);
              break;

            default:
              fprintf (stderr, "\nBad option %s", argv[1]);
              exit (1);
            }
        }
      --argc;
      ++argv;
    }

  if (host == NULL)
    {
      fprintf (stderr, "\nNo host was specified!");
      fflush (NULL);
      exit (1);
    }

  if (subsman == NULL)
    {
      fprintf (stderr, "\nNo subscription manager was specified!");
      fflush (NULL);
      exit (1);
    }

  if (id == NULL)
    {
      fprintf (stderr, "\nNo subscription id was specified!");
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

  soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);

  dpws_handle_init (&dpws, &client);

  if (dpws_subs_get_status2 (&client, subsman, id, &duration))
    {
      soap_print_fault (&client, stderr);
      exit (1);
    }

  printf ("Subscription status of %s:\n", id);
  printf (" + expires in %ld seconds\n", duration - ws4d_systime_s ());

  fflush (NULL);
  exit (0);
}
