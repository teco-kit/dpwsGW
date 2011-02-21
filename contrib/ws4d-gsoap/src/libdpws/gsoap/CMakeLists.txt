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

SET(gen_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)

# If the directory for the generated sources does not
# already exists, create it.
IF(NOT EXISTS ${gen_DIR})
	FILE(MAKE_DIRECTORY ${gen_DIR})
ENDIF(NOT EXISTS ${gen_DIR})

IF("${ALT_IO_MODE}" STREQUAL "none")
  INCLUDE_DIRECTORIES(AFTER ${GSOAP_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${gen_DIR})
ELSE("${ALT_IO_MODE}" STREQUAL "none")
  ADD_DEFINITIONS ( "-DWITH_NOIO -DWITH_SOAPDEFS_H" )
  INCLUDE_DIRECTORIES(AFTER ${DPWS_ALT_IO_INCLUDES} ${GSOAP_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${gen_DIR})
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

# the variable "dpws_GSOAP" contains all .gsoap files of this project
SET(gsoap_files
	${CMAKE_CURRENT_BINARY_DIR}/devprof.gsoap
	${CMAKE_CURRENT_BINARY_DIR}/target.gsoap
	${CMAKE_CURRENT_BINARY_DIR}/hosting.gsoap
	${CMAKE_CURRENT_BINARY_DIR}/wst.nsmap
	${CMAKE_CURRENT_BINARY_DIR}/wse.nsmap)
	
SET_SOURCE_FILES_PROPERTIES( ${gsoap_files}
	PROPERTIES GENERATED TRUE)

IF("${DPWS_VERSION}" STREQUAL "2006_02")

################################################################################
# 2006/02 version                                                              #
################################################################################

IF(NOT WITH_WS_SECURITY)
	ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_files}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/devprof_2006_02.gsoap ${CMAKE_CURRENT_BINARY_DIR}/devprof.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/target_2006_02.gsoap ${CMAKE_CURRENT_BINARY_DIR}/target.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/hosting_2006_02.gsoap ${CMAKE_CURRENT_BINARY_DIR}/hosting.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wst_2006_02.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wst.nsmap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wse_2006_02.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wse.nsmap)
ELSE(NOT WITH_WS_SECURITY)
	ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_files}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/devprof_2006_02.gsoap-wsse ${CMAKE_CURRENT_BINARY_DIR}/devprof.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/target_2006_02.gsoap-wsse ${CMAKE_CURRENT_BINARY_DIR}/target.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/hosting_2006_02.gsoap-wsse ${CMAKE_CURRENT_BINARY_DIR}/hosting.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wst_2006_02.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wst.nsmap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wse_2006_02.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wse.nsmap)
ENDIF(NOT WITH_WS_SECURITY)


ELSEIF("${DPWS_VERSION}" STREQUAL "2009_01")

################################################################################
# 2009/01 version                                                              #
################################################################################

IF(NOT WITH_WS_SECURITY)
	ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_files}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/devprof_2009_01.gsoap ${CMAKE_CURRENT_BINARY_DIR}/devprof.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/target_2009_01.gsoap ${CMAKE_CURRENT_BINARY_DIR}/target.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/hosting_2009_01.gsoap ${CMAKE_CURRENT_BINARY_DIR}/hosting.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wst_2009_01.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wst.nsmap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wse_2009_01.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wse.nsmap)
ELSE(NOT WITH_WS_SECURITY)
	ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_files}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/devprof_2009_01.gsoap-wsse ${CMAKE_CURRENT_BINARY_DIR}/devprof.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/target_2009_01.gsoap-wsse ${CMAKE_CURRENT_BINARY_DIR}/target.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/hosting_2009_01.gsoap-wsse ${CMAKE_CURRENT_BINARY_DIR}/hosting.gsoap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wst_2009_01.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wst.nsmap
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/wse_2009_01.nsmap ${CMAKE_CURRENT_BINARY_DIR}/wse.nsmap)
ENDIF(NOT WITH_WS_SECURITY)

ENDIF("${DPWS_VERSION}" STREQUAL "2006_02")

SET(generated_files1
		${gen_DIR}/targetServerLib.c
		${gen_DIR}/targetServer.c
		${gen_DIR}/targetClientLib.c
		${gen_DIR}/targetClient.c
		${gen_DIR}/targetC.c
		${gen_DIR}/targetH.h
		${gen_DIR}/targetStub.h
		${gen_DIR}/hostingServerLib.c
		${gen_DIR}/hostingServer.c
		${gen_DIR}/hostingClientLib.c
		${gen_DIR}/hostingClient.c
		${gen_DIR}/hostingC.c
		${gen_DIR}/hostingH.h
		${gen_DIR}/hostingStub.h
		${gen_DIR}/dpwsC.c
		${gen_DIR}/dpwsH.h
		${gen_DIR}/dpwsStub.h)

IF("${DPWS_VERSION}" STREQUAL "2009_01")
GSOAP_GENERATE_DATABINDING(${CMAKE_CURRENT_BINARY_DIR}/devprof.gsoap dpws ${gen_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/dpwsC.c-2009_01.patch)
ELSE("${DPWS_VERSION}" STREQUAL "2009_01")
GSOAP_GENERATE_DATABINDING(${CMAKE_CURRENT_BINARY_DIR}/devprof.gsoap dpws ${gen_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/dpwsC.c.patch)
ENDIF("${DPWS_VERSION}" STREQUAL "2009_01")

GSOAP_GENERATE(${CMAKE_CURRENT_BINARY_DIR}/target.gsoap target ${gen_DIR})
GSOAP_GENERATE(${CMAKE_CURRENT_BINARY_DIR}/hosting.gsoap hosting ${gen_DIR})

# the variable "dpws_generated_SRCS" contains all sources generated by soapcpp2
SET(generated_files2
	${gen_DIR}/targetPeerLib.c
	${gen_DIR}/hostingPeerLib.c
	${DPWS_GENERATED_INCLUDE_DIR}/dpwsH.h
	${DPWS_GENERATED_INCLUDE_DIR}/dpwsStub.h)

SET_SOURCE_FILES_PROPERTIES( ${generated_files2}
	PROPERTIES GENERATED TRUE)

ADD_CUSTOM_COMMAND(OUTPUT ${generated_files2}
	COMMAND ${CMAKE_COMMAND} -E copy ${gen_DIR}/dpwsH.h ${DPWS_GENERATED_INCLUDE_DIR}/dpwsH.h
	COMMAND ${CMAKE_COMMAND} -E copy ${gen_DIR}/dpwsStub.h ${DPWS_GENERATED_INCLUDE_DIR}/dpwsStub.h
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/targetPeerLib.c ${gen_DIR}/targetPeerLib.c
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/hostingPeerLib.c ${gen_DIR}/hostingPeerLib.c
	DEPENDS ${generated_files1})

ADD_CUSTOM_TARGET(gen_sources DEPENDS ${generated_files1} ${generated_files2})

ADD_DEPENDENCIES(gen_sources gen_headers)


################################################################################
# dpws client sources                                                          #
################################################################################

SET(dpws_client_SRCS
	${gen_DIR}/targetPeerLib.c
	${gen_DIR}/hostingPeerLib.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-addressing.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-discovery.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-discovery_custom.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-metadataexchange.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-eventing.c
	${CMAKE_CURRENT_SOURCE_DIR}/soap_misc.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws_custom.c
	${WS4D_COMMON_DIR}/ws4d_mm.c
	${WS4D_COMMON_DIR}/ws4d_misc.c
	${WS4D_COMMON_DIR}/ws4d_uri.c
	${WS4D_COMMON_DIR}/ws4d_scopelist.c
	${WS4D_COMMON_DIR}/ws4d_epr.c
	${WS4D_COMMON_DIR}/ws4d_target.c
	${WS4D_COMMON_DIR}/ws4d_service.c
	${WS4D_COMMON_DIR}/ws4d_servicecache.c
	${WS4D_COMMON_DIR}/ws4d_targetcache.c
	${WS4D_COMMON_DIR}/ws4d_discovery_ctx.c
	${WS4D_COMMON_DIR}/ws4d_subscription.c
	${WS4D_COMMON_DIR}/ws4d_subsmanproxy.c)


################################################################################
# dpws client static library                                                   #
################################################################################

ADD_LIBRARY(dpwsC STATIC ${dpws_client_SRCS})

EXTEND_TGT_COMPILE_FLAGS(dpwsC 
	FLAGS "-DWSD_CLIENT -DMEX_CLIENT -DWSE_CLIENT -DDPWS_CLIENT")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsC FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsC FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)


################################################################################
# thread safe dpws client static library                                       #
################################################################################

ADD_LIBRARY(dpwsCmt STATIC 
	${dpws_client_SRCS}
	${WS4D_COMMON_DIR}/ws4d_mutex.c)

EXTEND_TGT_COMPILE_FLAGS(dpwsCmt 
	FLAGS "-DWITH_MUTEXES -DWSD_CLIENT -DMEX_CLIENT -DWSE_CLIENT -DDPWS_CLIENT")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsCmt FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsCmt FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)


################################################################################
# dpws device sources                                                          #
################################################################################

SET(dpws_device_SRCS
	${gen_DIR}/targetPeerLib.c
	${gen_DIR}/hostingPeerLib.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-addressing.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-discovery.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-discovery_custom.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-metadataexchange.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-eventing.c
	${CMAKE_CURRENT_SOURCE_DIR}/soap_misc.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws_custom.c
	${WS4D_COMMON_DIR}/ws4d_mm.c
	${WS4D_COMMON_DIR}/ws4d_misc.c
	${WS4D_COMMON_DIR}/ws4d_uri.c
	${WS4D_COMMON_DIR}/ws4d_epr.c
	${WS4D_COMMON_DIR}/ws4d_scopelist.c
	${WS4D_COMMON_DIR}/ws4d_target.c
	${WS4D_COMMON_DIR}/ws4d_service.c
	${WS4D_COMMON_DIR}/ws4d_targetservice.c
	${WS4D_COMMON_DIR}/ws4d_hosting.c
	${WS4D_COMMON_DIR}/ws4d_subscription.c
	${WS4D_COMMON_DIR}/ws4d_subsmanager.c)


################################################################################
# dpws device static library                                                   #
################################################################################

ADD_LIBRARY(dpwsD STATIC ${dpws_device_SRCS})

EXTEND_TGT_COMPILE_FLAGS(dpwsD 
	FLAGS "-DWSD_TARGET -DMEX_SERVER -DWSE_SERVER -DDPWS_DEVICE")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsD FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsD FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)


################################################################################
# thread safe dpws device static library                                       #
################################################################################

ADD_LIBRARY(dpwsDmt STATIC 
	${dpws_device_SRCS}
	${WS4D_COMMON_DIR}/ws4d_mutex.c)

EXTEND_TGT_COMPILE_FLAGS(dpwsDmt 
	FLAGS "-DWITH_MUTEXES -DWSD_TARGET -DMEX_SERVER -DWSE_SERVER -DDPWS_DEVICE")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsDmt FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsDmt FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)


################################################################################
# dpws peer sources                                                            #
################################################################################

SET(dpws_peer_SRCS
	${gen_DIR}/targetPeerLib.c
	${gen_DIR}/hostingPeerLib.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-addressing.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-discovery.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-discovery_custom.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-metadataexchange.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-eventing.c
	${CMAKE_CURRENT_SOURCE_DIR}/soap_misc.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws_custom.c
	${WS4D_COMMON_DIR}/ws4d_mm.c
	${WS4D_COMMON_DIR}/ws4d_misc.c
	${WS4D_COMMON_DIR}/ws4d_uri.c
	${WS4D_COMMON_DIR}/ws4d_epr.c
	${WS4D_COMMON_DIR}/ws4d_scopelist.c
	${WS4D_COMMON_DIR}/ws4d_service.c
	${WS4D_COMMON_DIR}/ws4d_target.c
	${WS4D_COMMON_DIR}/ws4d_targetservice.c
	${WS4D_COMMON_DIR}/ws4d_hosting.c
	${WS4D_COMMON_DIR}/ws4d_servicecache.c
	${WS4D_COMMON_DIR}/ws4d_targetcache.c
	${WS4D_COMMON_DIR}/ws4d_discovery_ctx.c
	${WS4D_COMMON_DIR}/ws4d_subscription.c
	${WS4D_COMMON_DIR}/ws4d_subsmanproxy.c
	${WS4D_COMMON_DIR}/ws4d_subsmanager.c)


################################################################################
# dpws peer static library                                                     #
################################################################################

ADD_LIBRARY(dpwsP STATIC ${dpws_peer_SRCS})

EXTEND_TGT_COMPILE_FLAGS(dpwsP 
	FLAGS "-DWSD_CLIENT -DWSD_TARGET -DMEX_CLIENT -DMEX_SERVER -DWSE_CLIENT -DWSE_SERVER -DDPWS_PEER")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsP FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsP FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)


################################################################################
# thread safe dpws peer static library                                         #
################################################################################

ADD_LIBRARY(dpwsPmt STATIC 
	${dpws_peer_SRCS}
	${WS4D_COMMON_DIR}/ws4d_mutex.c)

EXTEND_TGT_COMPILE_FLAGS(dpwsPmt 
	FLAGS "-DWITH_MUTEXES -DWSD_CLIENT -DWSD_TARGET -DMEX_CLIENT -DMEX_SERVER -DWSE_CLIENT -DWSE_SERVER -DDPWS_PEER")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsPmt FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsPmt FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)


IF (WITH_HOSTING_SERVICE)

################################################################################
# dpws hosted service sources                                                  #
################################################################################

SET(dpws_hosted_SRCS
	${gen_DIR}/hostingPeerLib.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-addressing.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-discovery_custom.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-metadataexchange.c
	${CMAKE_CURRENT_SOURCE_DIR}/ws-eventing.c
	${CMAKE_CURRENT_SOURCE_DIR}/soap_misc.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws.c
	${CMAKE_CURRENT_SOURCE_DIR}/dpws_custom.c
	${WS4D_COMMON_DIR}/ws4d_mm.c
	${WS4D_COMMON_DIR}/ws4d_misc.c
	${WS4D_COMMON_DIR}/ws4d_uri.c
	${WS4D_COMMON_DIR}/ws4d_epr.c
	${WS4D_COMMON_DIR}/ws4d_service.c
	${WS4D_COMMON_DIR}/ws4d_hosting.c
	${WS4D_COMMON_DIR}/ws4d_subscription.c
	${WS4D_COMMON_DIR}/ws4d_subsmanager.c)


################################################################################
# dpws hosted service static library                                           #
################################################################################

ADD_LIBRARY(dpwsH STATIC ${dpws_hosted_SRCS})

EXTEND_TGT_COMPILE_FLAGS(dpwsH 
	FLAGS "-DMEX_SERVER -DWSE_SERVER -DDPWS_DEVICE -DDPWS_HOSTED_SERVICE")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsH FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsH FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)


################################################################################
# thread safe dpws hosted service static library                               #
################################################################################

ADD_LIBRARY(dpwsHmt STATIC 
	${dpws_hosted_SRCS}
	${WS4D_COMMON_DIR}/ws4d_mutex.c)

EXTEND_TGT_COMPILE_FLAGS(dpwsHmt 
	FLAGS "-DWITH_MUTEXES -DMEX_SERVER -DWSE_SERVER -DDPWS_DEVICE -DDPWS_HOSTED_SERVICE")

IF (DPWS_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dpwsHmt FLAGS "${DPWS_COMPILE_FLAGS}")
ENDIF (DPWS_COMPILE_FLAGS)

IF(MSVC)
	EXTEND_TGT_COMPILE_FLAGS(dpwsHmt FLAGS "-D_CRT_SECURE_NO_DEPRECATE")
ENDIF(MSVC)

ENDIF (WITH_HOSTING_SERVICE)

ADD_LIBRARY(dpwsS STATIC ${gen_DIR}/dpwsC.c)

IF("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-C_LIBRARIES
		dpwsC
		ws4d_listL
		CACHE STRING ""
		FORCE)
ELSE("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-C_LIBRARIES
		dpwsC
		${ALTIO_LIBRARIES}
		ws4d_listL
		CACHE STRING ""
		FORCE)
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

IF("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-CMT_LIBRARIES
		dpwsCmt
		ws4d_listLmt
		CACHE STRING ""
		FORCE)
ELSE("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-CMT_LIBRARIES
		dpwsCmt
		${ALTIOMT_LIBRARIES}
		ws4d_listLmt
		CACHE STRING ""
		FORCE)
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

IF("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-D_LIBRARIES
		dpwsD
		ws4d_listL
		CACHE STRING ""
		FORCE)
ELSE("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-D_LIBRARIES
		dpwsD
		${ALTIO_LIBRARIES}
		ws4d_listL
		CACHE STRING ""
		FORCE)
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

IF("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-DMT_LIBRARIES
		dpwsDmt
		ws4d_listLmt
		CACHE STRING ""
		FORCE)
ELSE("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-DMT_LIBRARIES
		dpwsDmt
		${ALTIOMT_LIBRARIES}
		ws4d_listLmt
		CACHE STRING ""
		FORCE)
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

IF("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-H_LIBRARIES
		dpwsH
		ws4d_listL
		CACHE STRING ""
		FORCE)
ELSE("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-H_LIBRARIES
		dpwsH
		${ALTIO_LIBRARIES}
		ws4d_listL
		CACHE STRING ""
		FORCE)
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

IF("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-HMT_LIBRARIES
		dpwsHmt
		ws4d_listLmt
		CACHE STRING ""
		FORCE)
ELSE("${ALT_IO_MODE}" STREQUAL "none")
SET(DPWS-HMT_LIBRARIES
		dpwsHmt
		${ALTIOMT_LIBRARIES}
		ws4d_listLmt
		CACHE STRING ""
		FORCE)
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

SET(DPWS_LIBRARIES
		dpwsS gsoap
		CACHE STRING ""
		FORCE)

ADD_DEPENDENCIES (dpwsC gen_headers gen_sources ${ALTIO_LIBRARIES})
ADD_DEPENDENCIES (dpwsCmt gen_headers gen_sources ${ALTIOMT_LIBRARIES})
ADD_DEPENDENCIES (dpwsD gen_headers gen_sources ${ALTIO_LIBRARIES})
ADD_DEPENDENCIES (dpwsDmt gen_headers gen_sources ${ALTIOMT_LIBRARIES})
ADD_DEPENDENCIES (dpwsP gen_headers gen_sources ${ALTIO_LIBRARIES})
ADD_DEPENDENCIES (dpwsPmt gen_headers gen_sources ${ALTIOMT_LIBRARIES})
IF (WITH_HOSTING_SERVICE)
ADD_DEPENDENCIES (dpwsH gen_headers gen_sources ${ALTIO_LIBRARIES})
ADD_DEPENDENCIES (dpwsHmt gen_headers gen_sources ${ALTIOMT_LIBRARIES})
ENDIF (WITH_HOSTING_SERVICE)
ADD_DEPENDENCIES (dpwsS gen_headers gen_sources)

INSTALL(TARGETS dpwsC dpwsCmt dpwsD dpwsDmt dpwsP dpwsPmt dpwsS
	ARCHIVE DESTINATION lib/ws4d-gSOAP)

IF (WITH_HOSTING_SERVICE)
INSTALL(TARGETS dpwsH dpwsHmt 	ARCHIVE DESTINATION lib/ws4d-gSOAP)
ENDIF (WITH_HOSTING_SERVICE)
INSTALL(FILES ${DPWS_GENERATED_INCLUDE_DIR}/dpwsH.h ${DPWS_GENERATED_INCLUDE_DIR}/dpwsStub.h DESTINATION include/ws4d-gSOAP)

MARK_AS_ADVANCED(
	DPWS-C_LIBRARIES
	DPWS-CMT_LIBRARIES
	DPWS-D_LIBRARIES
	DPWS-DMT_LIBRARIES
	DPWS-H_LIBRARIES
	DPWS-HMT_LIBRARIES
	DPWS_LIBRARIES)
