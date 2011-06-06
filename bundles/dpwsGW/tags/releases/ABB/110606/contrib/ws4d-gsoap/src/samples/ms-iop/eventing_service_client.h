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

#ifndef EVENTING_SERVICE_CLIENT_H_
#define EVENTING_SERVICE_CLIENT_H_

const char *do_subscribe (struct ws4d_epr *service, const char *actionfilter,
                          const char *deliverto, ws4d_time duration);

int do_renew (struct ws4d_epr *service, const char *id, ws4d_time duration);

int do_unsubscribe (struct ws4d_epr *service, const char *id);

int event_handler_init (struct dpws_s *dpws, struct soap *soap);

int event_handler_exit (void);

const char *event_handler_geturi (void);

#endif /* EVENTING_SERVICE_CLIENT_H_ */
