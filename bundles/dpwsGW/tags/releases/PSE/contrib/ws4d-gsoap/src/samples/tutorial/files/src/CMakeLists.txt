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

################################################################################
# Common for all ws4d-gsoap projects, don't change this !
################################################################################

# directory to put all generated files in
SET(gen_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)

# If the directory for the generated sources does not
# already exists, create it.
IF(NOT EXISTS ${gen_DIR})
	FILE(MAKE_DIRECTORY ${gen_DIR})
ENDIF(NOT EXISTS ${gen_DIR})

# set include paths
IF(DPWS_ALT_IO)
  SET(LWIP_DIR "/home/elmex/Dokumente/WS4D/ws4d-gsoap/lwip/lwip/src")

  SET(LWIP_PORT "/home/elmex/Dokumente/WS4D/ws4d-gsoap/lwip/contrib/ports/unix")

  SET(ALT_IO_INCLUDE
    ${LWIP_DIR}/include
    ${LWIP_PORT}/include
    ${LWIP_DIR}/include/ipv4)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${ALT_IO_INCLUDE} ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES} ${gen_DIR})

ELSE(DPWS_ALT_IO)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES} ${gen_DIR})

ENDIF(DPWS_ALT_IO)

# set options for gSOAP runtime
GSOAP_SET_RUNTIME_FLAGS("-DWITH_NONAMESPACES -DWITH_UDP")

###############################################################################
###############################################################################
##                                                                           ##
## Simple airconditioner device                                              ##
##                                                                           ##
###############################################################################
###############################################################################

###############################################################################
# Code generation - for custom projects this must be modified
###############################################################################

# generate code and C data binding operations of service
GSOAP_GENERATE(acs.gsoap acs1 ${gen_DIR})   # normal service

# generate device description and setup code
DPWS_METADATA_GENERATE(metadata.xml acs ${gen_DIR})

# embed wsdl files into c code
SET(AIRCONDITIONER_WSDL ${CMAKE_CURRENT_SOURCE_DIR}/AirConditioner.wsdl)
DPWS_EMBEDD_WSDL(${gen_DIR} acs ${AIRCONDITIONER_WSDL})

################################################################################
# create config.h
################################################################################

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${gen_DIR}/config.h
               @ONLY IMMEDIATE)

################################################################################
# build simple air conditioner - for custom projects this must be modified
################################################################################

# create airconditioner executable und define dependencies
# the device includes the airconditioner.c and event_worker.c, the
# gsoap runtime (), the server lib by gsoap
# (${gen_DIR}/acs1ServerLib.c), the client lib of the inverted service
# by gsoap for eventing (${gen_DIR}/acs_inv1ClientLib.c), the device
# description and setup code (${gen_DIR}/acs_metadata.c) and the embedded
# wsdl files (${gen_DIR}/acs_wsdl.c)

SET(simple_arcondtitioner_SRCS
	simple_airconditioner.c
	
	${gen_DIR}/acs1ServerLib.c
	${gen_DIR}/acs_metadata.c
	${gen_DIR}/acs_wsdl.c)

# compile all files in simple_arcondtitioner_SRCS and link them into
# the acs_simple_device executable
ADD_EXECUTABLE(acs_simple_device ${simple_arcondtitioner_SRCS})

EXTEND_TGT_COMPILE_FLAGS(acs_simple_device FLAGS "-DDPWS_DEVICE")

# link the device specific dpws libraries into the airconditioner executable
TARGET_LINK_LIBRARIES(acs_simple_device ${DPWS_LIBRARIES} ${DPWS-D_LIBRARIES})

###############################################################################
###############################################################################
##                                                                           ##
## GetStatus client                                                          ##
##                                                                           ##
###############################################################################
###############################################################################

################################################################################
# build simple client - for custom projects this must be modified
################################################################################

SET(GetStatus_client_SRCS
	GetStatus_client.c
	
	${gen_DIR}/acs1ClientLib.c)

ADD_EXECUTABLE(acs_GetStatus_client ${GetStatus_client_SRCS})

EXTEND_TGT_COMPILE_FLAGS(acs_GetStatus_client FLAGS "-DDPWS_CLIENT")

TARGET_LINK_LIBRARIES(acs_GetStatus_client ${DPWS_LIBRARIES} ${DPWS-C_LIBRARIES})

###############################################################################
###############################################################################
##                                                                           ##
## SetTargetTemperature client                                               ##
##                                                                           ##
###############################################################################
###############################################################################

################################################################################
# build simple client - for custom projects this must be modified
################################################################################

SET(SetTargetTemperature_client_SRCS
	SetTargetTemperature_client.c
	
	${gen_DIR}/acs1ClientLib.c)

ADD_EXECUTABLE(acs_SetTargetTemperature_client
	${SetTargetTemperature_client_SRCS})

EXTEND_TGT_COMPILE_FLAGS(acs_SetTargetTemperature_client
	FLAGS "-DDPWS_CLIENT")

TARGET_LINK_LIBRARIES(acs_SetTargetTemperature_client
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})

###############################################################################
###############################################################################
##                                                                           ##
## advanced GetStatus client                                                 ##
##                                                                           ##
###############################################################################
###############################################################################

################################################################################
# build advanced client - for custom projects this must be modified
################################################################################

SET(advanced_GetStatus_client_SRCS
	advanced_GetStatus_client.c
	
	${gen_DIR}/acs1ClientLib.c)

ADD_EXECUTABLE(acs_advanced_GetStatus_client
	${advanced_GetStatus_client_SRCS})

EXTEND_TGT_COMPILE_FLAGS(acs_advanced_GetStatus_client
	FLAGS "-DDPWS_CLIENT")

TARGET_LINK_LIBRARIES(acs_advanced_GetStatus_client
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})

###############################################################################
###############################################################################
##                                                                           ##
## air conditioner device with eventing                                      ##
##                                                                           ##
###############################################################################
###############################################################################

# generate code and C data binding for events of service
GSOAP_GENERATE(acs_inv.gsoap acs_inv1 ${gen_DIR}) # event service

SET (advanced_arcondtitioner_SRCS
	advanced_airconditioner.c
	event_worker.c
	
	${gen_DIR}/acs1ServerLib.c
	${gen_DIR}/acs_inv1ClientLib.c
	${gen_DIR}/acs_metadata.c
	${gen_DIR}/acs_wsdl.c)

# compile all files in simple_arcondtitioner_SRCS and link them into
# the acs_simple_device executable
ADD_EXECUTABLE(acs_advanced_device ${advanced_arcondtitioner_SRCS})

EXTEND_TGT_COMPILE_FLAGS(acs_advanced_device FLAGS "-DDPWS_DEVICE -DWITH_MUTEXES")

IF(DPWS_ALT_IO)
EXTEND_TGT_COMPILE_FLAGS(acs_advanced_device FLAGS "-DWITH_NOIO -DWITH_SOAPDEFS_H")
ENDIF(DPWS_ALT_IO)

# link the device specific dpws libraries into the airconditioner executable
TARGET_LINK_LIBRARIES(acs_advanced_device
	${DPWS_LIBRARIES} ${DPWS-DMT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

###############################################################################
###############################################################################
##                                                                           ##
## client with eventing                                                      ##
##                                                                           ##
###############################################################################
###############################################################################

################################################################################
# build eventing client - for custom projects this must be modified
################################################################################
SET(TemperatureEvent_client_SRCS
	TemperatureEvent_client.c
	
	${gen_DIR}/acs_inv1ServerLib.c)

ADD_EXECUTABLE(acs_TemperatureEvent_client
	${TemperatureEvent_client_SRCS})

EXTEND_TGT_COMPILE_FLAGS(acs_TemperatureEvent_client
	FLAGS "-DDPWS_CLIENT")

TARGET_LINK_LIBRARIES(acs_TemperatureEvent_client
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})

###############################################################################
###############################################################################
##                                                                           ##
## airconditioner device with custom databinding                             ##
##                                                                           ##
###############################################################################
###############################################################################

IF(CUSTOM_DATABINDING)

#generate custom databinding
GSOAP_GENERATE_DATABINDING(acs_databinding.gsoap dpws ${gen_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/dpwsC.c.patch)

SET(custom_arcondtitioner_SRCS
	advanced_airconditioner.c
	event_worker.c
	
	${gen_DIR}/dpwsC.c
	${gen_DIR}/acs1Server.c
	${gen_DIR}/acs_inv1Client.c
	${gen_DIR}/acs_metadata.c
	${gen_DIR}/acs_wsdl.c)

# use a custom databinding
ADD_EXECUTABLE(acs_custom_device ${custom_arcondtitioner_SRCS})

EXTEND_TGT_COMPILE_FLAGS(acs_custom_device FLAGS "-DDPWS_DEVICE -DWITH_MUTEXES")

TARGET_LINK_LIBRARIES(acs_custom_device ${DPWS-DMT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

ENDIF(CUSTOM_DATABINDING)