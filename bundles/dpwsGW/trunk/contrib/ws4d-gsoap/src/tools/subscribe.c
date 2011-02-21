/* subscribe - sends a subscription for an event to a dpws device
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
  struct ws4d_epr service;
  char *action = NULL;
  char *pushTo = NULL;
  char *expires = NULL;
  const char *subsman = NULL;
  const char *id = NULL;

  struct ws4d_delivery_type *delivery = NULL;
  struct ws4d_filter_type *filter = NULL;

  ws4d_time duration;

  WS4D_ALLOCLIST_INIT (&alist);
  ws4d_epr_init (&service);

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

            case 's':
              if (strlen (option) > 2)
                {
                  ++option;
                  ws4d_epr_set_Addrs (&service, option);
                }
              else
                {
                  --argc;
                  ++argv;
                  ws4d_epr_set_Addrs (&service, argv[1]);
                }
              fflush (NULL);
              break;

            case 'a':
              if (strlen (option) > 2)
                {
                  ++option;
                  action = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  action = argv[1];
                }
              fflush (NULL);
              break;

            case 'p':
              if (strlen (option) > 2)
                {
                  ++option;
                  pushTo = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  pushTo = argv[1];
                }
              fflush (NULL);
              break;

            case 'e':
              if (strlen (option) > 2)
                {
                  ++option;
                  expires = option;
                }
              else
                {
                  --argc;
                  ++argv;
                  expires = argv[1];
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

  if (expires == NULL)
    {
      duration = 3600;
    }
  else
    {
      /* TODO: set duration */
      /* duration = ws4d_s_to_dur (atol (expires), duration); */
    }

  if (id && *id)
    {
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

      if (dpws_subs_renew2 (&client, subsman, id, &duration) != SOAP_OK)
        {
          soap_print_fault (&client, stderr);
          exit (1);
        }
    }
  else
    {
      if (ws4d_epr_get_Addrs (&service) == NULL)
        {
          fprintf (stderr, "\nNo event source was specified!");
          fflush (NULL);
          exit (1);
        }

      if (action == NULL)
        {
          fprintf (stderr, "\nNo action was specified!");
          fflush (NULL);
          exit (1);
        }

      if (pushTo == NULL)
        {
          fprintf (stderr, "\nNo event sink was specified!");
          fflush (NULL);
          exit (1);
        }

      delivery = dpws_gen_delivery_push (&client, pushTo);
      filter = dpws_gen_filter_action (&client, action);

      /* TODO: fix wrong type of 4. parameter */
      id =
        dpws_subscribe (&client, &service, NULL, &duration, delivery, filter);
      if (!id)
        {
          soap_print_fault (&client, stderr);
          exit (1);
        }

      printf ("OK %s %s\n", dpws_subs_get_subsman (&service, id), id);
    }

  ws4d_epr_done (&service);

  fflush (NULL);
  exit (0);
}
