/* Copyright (C) 2007  University of Rostock
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

#ifndef DISCOVERY_CLIENT_H_
#define DISCOVERY_CLIENT_H_

int wait_for_hello (struct dpws_s *dpws, struct ws4d_tc_int *tc,
                    const char *uuid, int tries, int wait);

int wait_for_epr_invalid (struct dpws_s *dpws, struct ws4d_tc_int *tc,
                          const char *uuid, int tries, int wait);

int do_resolve (struct dpws_s *dpws, struct ws4d_tc_int *tc,
                const char *uuid);

int do_probe (struct dpws_s *dpws, struct ws4d_tc_int *tc, const char *uuid);

int do_notmatching_dprobe (struct dpws_s *dpws, struct ws4d_epr *device);

#endif /* DISCOVERY_CLIENT_H_ */
