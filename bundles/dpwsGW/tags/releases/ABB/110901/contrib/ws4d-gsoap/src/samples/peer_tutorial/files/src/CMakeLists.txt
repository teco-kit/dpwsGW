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
# Don't change this !
################################################################################

# directory to put all generated files in
SET(gen_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)

# If the directory for the generated sources does not
# already exists, create it.
IF(NOT EXISTS ${gen_DIR})
	FILE(MAKE_DIRECTORY ${gen_DIR})
ENDIF(NOT EXISTS ${gen_DIR})

# set include paths
INCLUDE_DIRECTORIES(${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES} ${gen_DIR})

###############################################################################
# Code generation - for custom projects this must be modified
###############################################################################

# generate code and C data binding operations of service
GSOAP_GENERATE(acc.gsoap acc1 ${gen_DIR})   # normal service

# generate device description and setup code
DPWS_METADATA_GENERATE(metadata.xml acc ${gen_DIR})

# embed wsdl files into c code
SET(AIRCONDITIONERCONTROL_WSDL ${CMAKE_CURRENT_SOURCE_DIR}/AirConditionerController.wsdl)
DPWS_EMBEDD_WSDL(${gen_DIR} acc ${AIRCONDITIONERCONTROL_WSDL})

################################################################################
# create config.h
################################################################################

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${gen_DIR}/config.h
               @ONLY IMMEDIATE)

################################################################################
# set options for gSOAP runtime
################################################################################

# stdsoap2.c
GSOAP_SET_RUNTIME_FLAGS("-DWITH_NONAMESPACES -DWITH_UDP")

################################################################################
# build AirConditioner device - for custom projects this must be modified
################################################################################

# create airconditioner executable und define dependencies
# the device includes the airconditioner.c and event_worker.c, the
# gsoap runtime (), the server lib by gsoap
# (${gen_DIR}/acs1ServerLib.c), the client lib of the inverted service
# by gsoap for eventing (${gen_DIR}/acs_inv1ClientLib.c), the device
# description and setup code (${gen_DIR}/acs_metadata.c) and the embedded
# wsdl files (${gen_DIR}/acs_wsdl.c)

SET(acc_device_SRCS
	acc_device.c
	discovery_helper.c
	
	${gen_DIR}/acc1ServerLib.c
	${gen_DIR}/acc_metadata.c
	${gen_DIR}/acc_wsdl.c)

EXTEND_COMPILE_FLAGS(${acc_device_SRCS}
	FLAGS "-DWITH_MUTEXES -DDPWS_PEER")

ADD_EXECUTABLE(acc_device
	${acc_device_SRCS})

# link the device specific dpws libraries and the pthread library into
# the airconditioner executable
TARGET_LINK_LIBRARIES(acc_device ${DPWS_LIBRARIES} ${DPWS-PMT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
