/* <one line to give the program's name and a brief idea of what it does.>
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

#include "ws4d_misc.h"
#include "test_support.h"
#include <libgen.h>

static void
ws4d_create_case (Suite * s, const char *name, TFun function)
{
  TCase *tc_new = tcase_create (name);
  tcase_set_timeout (tc_new, 30);
  suite_add_tcase (s, tc_new);
  tcase_add_test (tc_new, function);
}

static void
ws4d_testsuite_all (Suite * s, struct ws4d_testcase_s *tc)
{
  unsigned int i;
  for (i = 0; tc[i].name; i++)
    ws4d_create_case (s, tc[i].name, tc[i].func);
}

static unsigned int
ws4d_testsuite_selected (Suite * s, int argc, char **argv,
                         struct ws4d_testcase_s *tc)
{
  int i, j, n = 0;
  /* Also argv[0]! for symlink-ed calls */
  for (i = 0; argc > i; i++)
    {
      for (j = 0; tc[j].name; j++)
        {
          if (strcmp (basename (argv[i]), tc[j].name))
            continue;

          ws4d_create_case (s, tc[j].name, tc[j].func);
          n++;
        }
    }

  return n;
}

int
ws4d_testsuite (int argc, char **argv, const char *unittest,
                struct ws4d_testcase_s *tc)
{
  int nf;
  Suite *s = suite_create (unittest);
  SRunner *sr;

  if (!ws4d_testsuite_selected (s, argc, argv, tc))
    ws4d_testsuite_all (s, tc);

  sr = srunner_create (s);
  srunner_run_all (sr, CK_VERBOSE);
  nf = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
