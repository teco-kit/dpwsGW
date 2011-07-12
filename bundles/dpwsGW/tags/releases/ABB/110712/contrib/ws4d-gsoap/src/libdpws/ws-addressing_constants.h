/* WS4D-gSOAP - Implementation of the Devices Profile for Web Services
 * (DPWS) on top of gSOAP
 * Copyright (C) 2007 University of Rostock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 *  Created on: 04.03.2009
 *      Author: elmex
 */

#ifndef WSADDRESSING_CONSTANTS_H_
#define WSADDRESSING_CONSTANTS_H_

#if defined(WSA_2005_08)

#define WSA_FAULTACTION "http://www.w3.org/2005/08/addressing/fault";
#define WSA_ANONYMOUSURI "http://www.w3.org/2005/08/addressing/anonymous";

#else

#define WSA_FAULTACTION "http://schemas.xmlsoap.org/ws/2004/08/addressing/fault";
#define WSA_ANONYMOUSURI "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";

#endif

#endif /* WSADDRESSING_CONSTANTS_H_ */
