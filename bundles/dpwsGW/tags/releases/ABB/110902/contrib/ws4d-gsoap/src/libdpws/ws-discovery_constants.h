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

#ifndef WSDISCOVERY_CONSTANTS_H_
#define WSDISCOVERY_CONSTANTS_H_

#if defined(WSD_2005_04)

#define WSD_MC_IPADDR "239.255.255.250"
#define WSD_MC_IPADDR6 "FF02::C"
#define WSD_MC_UDPPORT 3702
#define WSD_MC_ADDR "soap.udp://239.255.255.250:3702/"
#define WSD_MC_ADDR6 "soap.udp://[FF02::C]:3702/"

#define WSD_EP "urn:schemas-xmlsoap-org:ws:2005:04:discovery"
#define WSD_HELLO_ACTION "http://schemas.xmlsoap.org/ws/2005/04/discovery/Hello"
#define WSD_BYE_ACTION "http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye"
#define WSD_PROBEMATCHES_ACTION "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches"
#define WSD_RESOLVEMATCHES_ACTION "http://schemas.xmlsoap.org/ws/2005/04/discovery/ResolveMatches"
#define WSD_PROBE_ACTION "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"
#define WSD_RESOLVE_ACTION "http://schemas.xmlsoap.org/ws/2005/04/discovery/Resolve"

#elif defined(WSD_2009_01)

#define WSD_MC_IPADDR "239.255.255.250"
#define WSD_MC_UDPPORT 3702
#define WSD_MC_ADDR "soap.udp://239.255.255.250:3702/"

#define WSD_EP "urn:docs-oasis-open-org:ws-dd:ns:discovery:2009:01"
#define WSD_HELLO_ACTION "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/Hello"
#define WSD_BYE_ACTION "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/Bye"
#define WSD_PROBEMATCHES_ACTION "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/ProbeMatches"
#define WSD_RESOLVEMATCHES_ACTION "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/ResolveMatches"
#define WSD_PROBE_ACTION "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/Probe"
#define WSD_RESOLVE_ACTION "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/Resolve"

#else
#error WS-Discovery version not supported
#endif

#endif /* WSDISCOVERY_CONSTANTS_H_ */
