# Copyright (C) 2007  University of Rostock
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA.

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/dpws_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/dpws_config.h
               @ONLY IMMEDIATE)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/ws4d_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/ws4d_config.h
               @ONLY IMMEDIATE)

INSTALL(FILES
	dpws.nsmap
	soap_misc.h
	${CMAKE_CURRENT_BINARY_DIR}/dpws_config.h
	${CMAKE_CURRENT_BINARY_DIR}/ws4d_config.h
	stddpws.h
	dpws_client.h
	dpws_device.h
	dpws_hosted.h
	dpws_peer.h
	ws-addressing.h
	ws-discovery.h
	ws-eventing.h
	ws-metadataexchange.h
	ws4d_abstract_eprlist.h
	ws4d_abstract_allocator.h
	ws4d_eprflist.h
	ws4d_eprllist.h
	ws4d_target.h
	ws4d_service.h
	ws4d_targetcache.h
	ws4d_servicecache.h
	ws4d_targetservice.h
	ws4d_hostingservice.h
	ws4d_discovery_ctx.h
	ws4d_uri.h
	ws4d_misc.h
	ws4d_mutex.h
	ws4d_list.h
	ws4d_alloclist.h
	ws4d_stringarray.h
	ws4d_time.h
	ws4d_uuid.h
	ws4d_qname.h
	ws4d_epr.h
	ws4d_scopelist.h
	ws4d_device_description.h
	ws4d_localizedstring.h
	ws4d_error.h
	ws4d_epr_kv.h
	ws4d_subscription.h
	ws4d_subsmanager.h
	ws4d_subsmanproxy.h
	DESTINATION include/ws4d-gSOAP)
