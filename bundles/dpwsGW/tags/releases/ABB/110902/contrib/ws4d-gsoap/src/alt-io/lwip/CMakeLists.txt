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

SET(WITH_ALT_IO true)

SET(LWIP_DIR "/home/elmex/Dokumente/WS4D/ws4d-gsoap/lwip/lwip-1.3.1/src")

SET(LWIP_PORT "/home/elmex/Dokumente/WS4D/ws4d-gsoap/lwip/contrib/ports/unix")

SET(DPWS_ALT_IO_INCLUDES
	${LWIP_DIR}/include
	${LWIP_PORT}/include
	${LWIP_DIR}/include/ipv4
	${CMAKE_CURRENT_SOURCE_DIR}
	CACHE INTERNAL "")

INCLUDE_DIRECTORIES(AFTER ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES} ${DPWS_ALT_IO_INCLUDES})

SET(LWIP_CORE_SRCS
	${LWIP_DIR}/core/mem.c
	${LWIP_DIR}/core/memp.c
	${LWIP_DIR}/core/netif.c
	${LWIP_DIR}/core/pbuf.c
	${LWIP_DIR}/core/stats.c
	${LWIP_DIR}/core/sys.c
	${LWIP_DIR}/core/tcp.c
	${LWIP_DIR}/core/tcp_in.c
	${LWIP_DIR}/core/tcp_out.c
	${LWIP_DIR}/core/udp.c
	${LWIP_DIR}/core/raw.c
	${LWIP_DIR}/core/init.c)

SET(LWIP_CORE4_SRCS
	${LWIP_DIR}/core/ipv4/icmp.c
	${LWIP_DIR}/core/ipv4/igmp.c
	${LWIP_DIR}/core/ipv4/ip.c
	${LWIP_DIR}/core/ipv4/ip_frag.c
	${LWIP_DIR}/core/ipv4/inet.c
	${LWIP_DIR}/core/ipv4/ip_addr.c
	${LWIP_DIR}/core/ipv4/inet_chksum.c)

SET(LWIP_API_SRCS
	${LWIP_DIR}/api/api_lib.c
	${LWIP_DIR}/api/api_msg.c
	${LWIP_DIR}/api/tcpip.c
	${LWIP_DIR}/api/err.c
	${LWIP_DIR}/api/sockets.c
	${LWIP_DIR}/api/netbuf.c
	${LWIP_DIR}/api/netdb.c)

SET(LWIP_NETIF_SRCS
	${LWIP_DIR}/netif/loopif.c
	${LWIP_DIR}/netif/etharp.c)

FILE(GLOB LWIP_PORT_ARCH_SRCS "${LWIP_PORT}/*.c")
FILE(GLOB LWIP_PORT_NETIF_SRCS "${LWIP_PORT}/netif/*.c")

SET(LWIP_SRCS
	${LWIP_CORE_SRCS}
	${LWIP_CORE4_SRCS}
	${LWIP_API_SRCS}
	${LWIP_NETIF_SRCS}
	${LWIP_PORT_ARCH_SRCS}
	${LWIP_PORT_NETIF_SRCS})

################################################################################
# lwip io sources                                                    #
################################################################################

SET(lwip_io_SRCS
	${LWIP_SRCS}
	${CMAKE_CURRENT_SOURCE_DIR}/gsoap-lwip-io.c)

################################################################################
# thread safe lwip io static library                                             #
################################################################################

ADD_LIBRARY(dpwsIOmt STATIC 
	${lwip_io_SRCS})

EXTEND_TGT_COMPILE_FLAGS(dpwsIOmt 
	FLAGS "-DWITH_NOIO -DWITH_MUTEXES -Dunix -DIPv4 -DLWIP_DEBUG")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsIOmt FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

ADD_LIBRARY(dpwsIO STATIC 
	${lwip_io_SRCS})

EXTEND_TGT_COMPILE_FLAGS(dpwsIO 
	FLAGS "-DWITH_NOIO -DWITH_MUTEXES -Dunix -DIPv4 -DLWIP_DEBUG")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsIO FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

	
INSTALL(TARGETS dpwsIOmt
	ARCHIVE DESTINATION lib/ws4d-gSOAP)

INSTALL(TARGETS dpwsIO
	ARCHIVE DESTINATION lib/ws4d-gSOAP)

SET(ALTIO_LIBRARIES
		${CMAKE_THREAD_LIBS_INIT}
		dpwsIOmt
		CACHE STRING ""
		FORCE)

SET(ALTIOMT_LIBRARIES
		${CMAKE_THREAD_LIBS_INIT}
		dpwsIOmt
		CACHE STRING ""
		FORCE)

INSTALL(FILES
  alt_io.h
  gsoap-lwip-io.h
  lwipopts.h
  soapdefs.h
	DESTINATION include/ws4d-gSOAP)