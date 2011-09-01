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

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

INCLUDE_DIRECTORIES(AFTER ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES})

IF("${ALT_IO_MODE}" STREQUAL "none")
  INCLUDE_DIRECTORIES(AFTER ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES})
ELSE("${ALT_IO_MODE}" STREQUAL "none")
  ADD_DEFINITIONS ( "-DWITH_NOIO -DWITH_SOAPDEFS_H" )
  INCLUDE_DIRECTORIES(AFTER ${DPWS_ALT_IO_INCLUDES} ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES})
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

# stdsoap2.c
IF (TOOL_COMPILE_FLAGS)
	GSOAP_SET_RUNTIME_FLAGS("-DWITH_NONAMESPACES -DWITH_UDP ${TOOL_COMPILE_FLAGS}")
ELSE (TOOL_COMPILE_FLAGS)
	GSOAP_SET_RUNTIME_FLAGS("-DWITH_NONAMESPACES -DWITH_UDP")
ENDIF (TOOL_COMPILE_FLAGS)


SET(DPWS_DEVICE_METADATATOC ${CMAKE_CURRENT_SOURCE_DIR}/deviceMetadataToC.xslt CACHE PATH "")
SET(DPWS_DEVICE_METADATATOH ${CMAKE_CURRENT_SOURCE_DIR}/deviceMetadataToH.xslt CACHE PATH "")
SET(DPWS_HOSTED_METADATATOC ${CMAKE_CURRENT_SOURCE_DIR}/hostedMetadataToC.xslt CACHE PATH "")
SET(DPWS_HOSTED_METADATATOH ${CMAKE_CURRENT_SOURCE_DIR}/hostedMetadataToH.xslt CACHE PATH "")


################################################################################
# embedwsdl                                                                    #
################################################################################
IF(NOT CMAKE_CROSSCOMPILING)

	ADD_EXECUTABLE(embedwsdl
		embedwsdl.c)
		
	GET_TARGET_PROPERTY(EMBEDWSDL_BINARY embedwsdl LOCATION)
    SET(DPWS_EMBEDWSDL ${EMBEDWSDL_BINARY} CACHE PATH "")
    
ELSE(NOT CMAKE_CROSSCOMPILING)

	ADD_EXECUTABLE(embedwsdl
		embedwsdl.c)

	FIND_PROGRAM(DPWS_EMBEDWSDL
 		NAMES embedwsdl
		PATHS ${DPWS_EMBEDWSDL_PATH}
		NO_DEFAULT_PATH
		NO_CMAKE_ENVIRONMENT_PATH
		NO_CMAKE_PATH
		NO_SYSTEM_ENVIRONMENT_PATH
		NO_CMAKE_SYSTEM_PATH)

	IF(NOT DPWS_EMBEDWSDL)
		SET(ERROR_MSG "${ERROR_MSG} can't find host executable embedwsdl in ${DPWS_EMBEDWSDL_PATH}")
	ENDIF(NOT DPWS_EMBEDWSDL)
  
ENDIF(NOT CMAKE_CROSSCOMPILING)

ADD_CUSTOM_TARGET(tool_embedwsdl DEPENDS ${DPWS_EMBEDWSDL})


################################################################################
# supervisor                                                                   #
################################################################################

ADD_EXECUTABLE(supervisor
	supervisor.c)

EXTEND_TGT_COMPILE_FLAGS(supervisor 
	FLAGS "-DWITH_MUTEXES")

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(supervisor 
		FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(supervisor
	${DPWS-CMT_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-CMT_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${OPENSSL_LIBRARIES}
	gsoap
)

ADD_DEPENDENCIES(supervisor
	${DPWS_LIBRARIES}
	${DPWS-CMT_LIBRARIES})


################################################################################
# resolve                                                                      #
################################################################################

ADD_EXECUTABLE(resolve
	resolve.c)
GET_TARGET_PROPERTY(RESOLVE_LOCATION resolve LOCATION)
SET(RESOLVE_BINARY ${RESOLVE_LOCATION} CACHE PATH "")

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(resolve FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(resolve
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
	${OPENSSL_LIBRARIES}
	gsoap
)
	
ADD_DEPENDENCIES(resolve
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
)


################################################################################
# probe                                                                        #
################################################################################

ADD_EXECUTABLE(probe
	probe.c)

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(probe FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(probe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
	${OPENSSL_LIBRARIES})

ADD_DEPENDENCIES(probe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})


################################################################################
# dprobe                                                                       #
################################################################################

ADD_EXECUTABLE(dprobe
	dprobe.c)

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(dprobe FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(dprobe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
	${OPENSSL_LIBRARIES})

ADD_DEPENDENCIES(dprobe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})


################################################################################
# describe                                                                     #
################################################################################

ADD_EXECUTABLE(describe
	describe.c)

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(describe FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(describe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
	${OPENSSL_LIBRARIES}
	gsoap
)
	
ADD_DEPENDENCIES (describe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})


################################################################################
# subscribe                                                                    #
################################################################################

ADD_EXECUTABLE(subscribe
	subscribe.c)

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(subscribe FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(subscribe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
	${OPENSSL_LIBRARIES})

ADD_DEPENDENCIES (subscribe
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})


################################################################################
# unsubscribe                                                                  #
################################################################################

ADD_EXECUTABLE(unsubscribe
	unsubscribe.c)

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(unsubscribe FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(unsubscribe
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
	${OPENSSL_LIBRARIES}
	gsoap
)

ADD_DEPENDENCIES (unsubscribe
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})


################################################################################
# subsstatus                                                                   #
################################################################################

ADD_EXECUTABLE(subsstatus
	subsstatus.c)

IF (TOOL_COMPILE_FLAGS)
	EXTEND_TGT_COMPILE_FLAGS(subsstatus FLAGS "${TOOL_COMPILE_FLAGS}")
ENDIF (TOOL_COMPILE_FLAGS)

TARGET_LINK_LIBRARIES(subsstatus
	${DPWS-C_LIBRARIES}
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES}
	${OPENSSL_LIBRARIES}
	gsoap
)

ADD_DEPENDENCIES (subsstatus
	${DPWS_LIBRARIES}
	${DPWS-C_LIBRARIES})


INSTALL(TARGETS embedwsdl describe dprobe probe resolve subscribe subsstatus supervisor unsubscribe
	RUNTIME DESTINATION bin)

FILE(WRITE ${EXECUTABLE_OUTPUT_PATH}/HostExecutables.cmake "SET(DPWS_EMBEDWSDL_PATH ${CMAKE_INSTALL_PREFIX}/bin CACHE PATH \"\")\n")
FILE(APPEND ${EXECUTABLE_OUTPUT_PATH}/HostExecutables.cmake "MARK_AS_ADVANCED(DPWS_EMBEDWSDL_PATH)")

INSTALL(FILES ${EXECUTABLE_OUTPUT_PATH}/HostExecutables.cmake DESTINATION share/ws4d-gSOAP)

IF(CMAKE_CROSSCOMPILING)
	INSTALL(PROGRAMS ${DPWS_EMBEDWSDL} DESTINATION bin RENAME embedwsdl_host)
ENDIF(CMAKE_CROSSCOMPILING)	

INSTALL(FILES deviceMetadataToC.xslt deviceMetadataToH.xslt hostedMetadataToC.xslt hostedMetadataToH.xslt invert.xslt DESTINATION share/ws4d-gSOAP)

MARK_AS_ADVANCED(
	DPWS_EMBEDWSDL
	DPWS_DEVICE_METADATATOC
	DPWS_DEVICE_METADATATOH
	DPWS_HOSTED_METADATATOC
	DPWS_HOSTED_METADATATOH
	RESOLVE_BINARY
	DEVICETEST_BINARY
)

