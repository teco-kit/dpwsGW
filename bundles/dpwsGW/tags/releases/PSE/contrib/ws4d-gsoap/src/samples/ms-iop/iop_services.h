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
 *
 *  Created on: 11.09.2008
 *      Author: Elmar Zeeb
 */

#ifndef IOP_SERVICES_H_
#define IOP_SERVICES_H_

int
setup_AttachmentService (struct dpws_s *device, struct soap *handle,
                         const char *wsdl, int backlog);

int
setup_EventingService (struct dpws_s *device, struct soap *handle,
                       const char *wsdl, int backlog);

int fire_simple_event (struct dpws_s *dpws);

int fire_integer_event (struct dpws_s *dpws, int integer);

int
setup_SimpleService1 (struct dpws_s *device, struct soap *handle,
                      const char *wsdl, int backlog);

int
setup_SimpleService2 (struct dpws_s *device, struct soap *handle,
                      const char *wsdl, int backlog);

#endif /* IOP_SERVICES_H_ */
