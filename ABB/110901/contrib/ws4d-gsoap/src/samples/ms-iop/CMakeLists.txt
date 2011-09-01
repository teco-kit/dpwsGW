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

IF(NOT CMAKE_CROSSCOMPILING)

SET(gen_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)

# If the directory for the generated sources does not
# already exists, create it.
IF(NOT EXISTS ${gen_DIR})
	FILE(MAKE_DIRECTORY ${gen_DIR})
ENDIF(NOT EXISTS ${gen_DIR})

INCLUDE_DIRECTORIES(AFTER ${gen_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${OPENSSL_INCLUDE_DIR})

GSOAP_GENERATE(SimpleService.h sis ${gen_DIR})
GSOAP_GENERATE(AttachmentService.h ats ${gen_DIR})
GSOAP_GENERATE(EventingService.h evs ${gen_DIR})

# generate device description and setup code
IF (WITH_DPWS_SECURITY)
  DPWS_METADATA_GENERATE(metadata_sec.xml msiop ${gen_DIR})
ELSE (WITH_DPWS_SECURITY)
  DPWS_METADATA_GENERATE(metadata.xml msiop ${gen_DIR})
ENDIF (WITH_DPWS_SECURITY)

DPWS_EMBEDD_WSDL(${gen_DIR} msiop SimpleService.wsdl AttachmentService.wsdl EventingService.wsdl)

# stdsoap2.c
IF (WITH_DPWS_SECURITY)
	GSOAP_SET_RUNTIME_FLAGS("-DWITH_NONAMESPACES -DWITH_UDP -DWITH_OPENSSL")
ELSE (WITH_DPWS_SECURITY)
	GSOAP_SET_RUNTIME_FLAGS("-DWITH_NONAMESPACES -DWITH_UDP")
ENDIF (WITH_DPWS_SECURITY)

IF (WITH_DPWS_SECURITY)
ELSE (WITH_DPWS_SECURITY)
ENDIF (WITH_DPWS_SECURITY)

SET(iop_device_lib_SRCS
	fileio.c
	SimpleService.c
	AttachmentService.c
	EventingService.c
	${gen_DIR}/sisServerLib.c
	${gen_DIR}/atsServerLib.c
	${gen_DIR}/evsClientLib.c
	${gen_DIR}/msiop_metadata.c
	${gen_DIR}/msiop_wsdl.c)
	
ADD_LIBRARY(iop_device_lib STATIC ${iop_device_lib_SRCS} )
EXTEND_TGT_COMPILE_FLAGS(iop_device_lib FLAGS "-DDPWS_DEVICE")
ADD_DEPENDENCIES(iop_device_lib tool_embedwsdl)

SET(IOP_DEVICE_LIB_INCLUDES 
	${gen_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
	CACHE STRING ""
	FORCE)

SET(IOP_DEVICE_LIB 
	iop_device_lib
	CACHE STRING ""
	FORCE)

# iop_device
ADD_EXECUTABLE(iop_device
	iop_device.c)
EXTEND_TGT_COMPILE_FLAGS(iop_device FLAGS "-DDPWS_DEVICE")

TARGET_LINK_LIBRARIES(iop_device
	iop_device_lib
	${DPWS_LIBRARIES}
	${DPWS-D_LIBRARIES}
	${OPENSSL_LIBRARIES})

# files for iop_device
GET_TARGET_PROPERTY(iop_device_LOCATION iop_device LOCATION)
GET_FILENAME_COMPONENT(iop_device_PATH ${iop_device_LOCATION} PATH)

ADD_CUSTOM_TARGET(iop_device_pics ALL
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dpws1ref.jpg ${iop_device_PATH}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dpws2.jpg ${iop_device_PATH}/
	DEPENDS iop_device)

ADD_CUSTOM_TARGET(iop_device_pems ALL
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/server.pem ${iop_device_PATH}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/client.pem ${iop_device_PATH}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cacerts.pem ${iop_device_PATH}/
	DEPENDS iop_device)

SET(iop_test_driver_SRCS
	iop_test_driver.c
	discovery_client.c
	description_client.c
	simple_service_client.c
	attachment_service_client.c
	eventing_service_client.c
	fileio.c
	${gen_DIR}/sisClientLib.c
	${gen_DIR}/atsClientLib.c
	${gen_DIR}/evsServerLib.c)

SET_SOURCE_FILES_PROPERTIES( ${iop_test_driver_SRCS}
	PROPERTIES COMPILE_FLAGS "-DWITH_MUTEXES")

ADD_EXECUTABLE(iop_test_driver ${iop_test_driver_SRCS} )

TARGET_LINK_LIBRARIES(iop_test_driver
	${DPWS_LIBRARIES}
	${DPWS-CMT_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${OPENSSL_LIBRARIES})

MARK_AS_ADVANCED(
	IOP_DEVICE_LIB_INCLUDES
	IOP_DEVICE_LIB
)

ENDIF(NOT CMAKE_CROSSCOMPILING)
