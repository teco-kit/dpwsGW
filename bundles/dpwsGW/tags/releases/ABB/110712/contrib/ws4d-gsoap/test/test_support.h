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

#include <check.h>

#define WS4D_TESTCASE_START(x)         \
        const char *_unittest = (#x);   \
        struct ws4d_testcase_s ws4d_testcase[] = {

#define WS4D_TESTCASE_ADD(x) { (#x), x },

#define WS4D_TESTCASE_END                      \
        { NULL, NULL }                          \
        };                                      \
                                                \
int main(int argc, char **argv)                 \
{                                               \
        return ws4d_testsuite(argc, argv, _unittest, ws4d_testcase);  \
}

struct ws4d_testcase_s
{
  const char *name;
  void *func;
};


int ws4d_testsuite (int argc, char **argv, const char *unittest,
                    struct ws4d_testcase_s *tc);
