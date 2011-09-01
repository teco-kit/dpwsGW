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
 *  Created on: 02.01.2009
 *      Author: elmex
 */

#ifndef DEVPROF_CONSTANTS_H_
#define DEVPROF_CONSTANTS_H_

#if defined(DEVPROF_2006_02)

#define DPWS_NS "http://schemas.xmlsoap.org/ws/2006/02/devprof"
#define DPWS_FAULT_ACTION "http://schemas.xmlsoap.org/ws/2006/02/devprof/Fault"
#define DPWS_FILTERTYPE_ACTION "http://schemas.xmlsoap.org/ws/2006/02/devprof/Action"
#define DPWS_DELIVERYMODE_PUSH "http://schemas.xmlsoap.org/ws/2004/08/eventing/DeliveryModes/Push"
#define DPWS_STRCMP0_MATCHING_FUNC "http://schemas.xmlsoap.org/ws/2005/04/discovery/strcmp0"

#elif defined(DEVPROF_2009_01)

#define DPWS_NS "http://docs.oasis-open.org/ws-dd/ns/dpws/2009/01"
#define DPWS_FAULT_ACTION "http://docs.oasis-open.org/ws-dd/ns/dpws/2009/01/Fault"
#define DPWS_FILTERTYPE_ACTION "http://docs.oasis-open.org/ws-dd/ns/dpws/2009/01/Action"
#define DPWS_DELIVERYMODE_PUSH "http://schemas.xmlsoap.org/ws/2004/08/eventing/DeliveryModes/Push"
#define DPWS_STRCMP0_MATCHING_FUNC "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/strcmp0"

#else
#error DPWS version not supported
#endif

#endif /* DEVPROF_CONSTANTS_H_ */
