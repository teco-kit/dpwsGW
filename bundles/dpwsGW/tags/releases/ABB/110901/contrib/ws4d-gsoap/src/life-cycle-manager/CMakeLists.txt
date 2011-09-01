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

FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Lua51 REQUIRED)


# directory to put all generated files in
SET(gen_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)

# If the directory for the generated sources does not
# already exists, create it.
IF(NOT EXISTS ${gen_DIR})
	FILE(MAKE_DIRECTORY ${gen_DIR})
ENDIF(NOT EXISTS ${gen_DIR})

# set include paths
INCLUDE_DIRECTORIES(${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES} ${gen_DIR} ${ZLIB_INCLUDE_DIR} ${LUA_INCLUDE_DIR})

INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)

CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
IF(HAVE_UNISTD_H)
  ADD_DEFINITIONS(-DHAVE_UNISTD_H)
ENDIF(HAVE_UNISTD_H)

###############################################################################
# Code generation - for custom projects this must be modified
###############################################################################

# generate code and C data binding operations of service
GSOAP_GENERATE(lcm.gsoap lcm ${gen_DIR})   # normal service

# generate device description and setup code
DPWS_METADATA_GENERATE(metadata.xml lcm ${gen_DIR} hosted)

# embed wsdl files into c code
SET(LCMHOSTEDSERVICE_WSDL
	    ${CMAKE_CURRENT_SOURCE_DIR}/life-cycle-manager.wsdl)
DPWS_EMBEDD_WSDL(${gen_DIR} lcm ${LCMHOSTEDSERVICE_WSDL})

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
GSOAP_SET_RUNTIME_FLAGS("-DWITH_NONAMESPACES")

################################################################################
# build LCM
################################################################################

SET(life-cycle-manager_SRCS
	life-cycle-manager.c
	fileio.c
	ioapi.c
	unzip.c
	
	${gen_DIR}/lcmServerLib.c
	${gen_DIR}/lcm_metadata.c
	${gen_DIR}/lcm_wsdl.c)

# create LCM executable und define dependencies
ADD_EXECUTABLE(life-cycle-manager ${life-cycle-manager_SRCS})

EXTEND_TGT_COMPILE_FLAGS(life-cycle-manager
	FLAGS "-DDPWS_DEVICE -DNOUNCRYPT")

# link the device specific dpws libraries and the pthread library into
# the executables
TARGET_LINK_LIBRARIES(life-cycle-manager 
	${DPWS_LIBRARIES}
	${DPWS-H_LIBRARIES}
	${DPWS_LIST_LIBRARY}
	${ZLIB_LIBRARIES}
	${LUA_LIBRARIES})

INSTALL(TARGETS life-cycle-manager RUNTIME DESTINATION bin)

INSTALL(FILES metadata.xml RENAME lcm.xml DESTINATION ${HOSTEDSERVICE_META_DIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/init_script.sh RENAME lcm
		DESTINATION ${HOSTEDSERVICE_INIT_DIR}
		PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)
