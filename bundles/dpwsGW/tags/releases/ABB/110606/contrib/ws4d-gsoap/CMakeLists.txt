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

# DPWS CMake file - 30.07.2007
#
# Top level cmake file of the dpws core library.
#
# author: S.Feldhorst <sascha.feldhorst@udo.edu>

# Name of the project
PROJECT(WS4D-gSOAP C)

# The required version of cmake. This is needed for full qt4-support.
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0 FATAL_ERROR)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# Sets the default build type to Debug if not already done
IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE
		Debug
		CACHE STRING "Choose the build type, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE
	)
ENDIF (NOT CMAKE_BUILD_TYPE)

# additional compiler flags for WS4D-gSOAP

IF (CMAKE_COMPILER_IS_GNUCC AND NOT CMAKE_CROSSCOMPILING)
	SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -funsigned-char -fstrict-aliasing -Wall -Wextra -Wchar-subscripts -Wundef -Wshadow -Wcast-align -Wwrite-strings -Wsign-compare -Wunused -Wno-unused-value -Wpointer-arith -Wredundant-decls -Wmissing-prototypes -Wno-variadic-macros")
ENDIF (CMAKE_COMPILER_IS_GNUCC AND NOT CMAKE_CROSSCOMPILING)
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

IF (CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -ffunction-sections -fdata-sections -Wl,--gc-sections")
ENDIF (CMAKE_COMPILER_IS_GNUCC)

INCLUDE(CMakeDependentOption)

# Read the current build version.
FILE(READ ${CMAKE_SOURCE_DIR}/buildversion VERSION)
MESSAGE(STATUS "Buildversion: ${VERSION}")

# Some cmake specific settings.
SET(CMAKE_COLOR_MAKEFILE ON) # For a colorful makefile output.
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake-modules/) # Include own modules.

IF(CMAKE_TOOLCHAIN_FILE)
	CMAKE_MINIMUM_REQUIRED(VERSION 2.5.0 FATAL_ERROR)
ENDIF(CMAKE_TOOLCHAIN_FILE)


################################################################################
# Check dependencies                                                           #
################################################################################

# required
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(XSLTproc REQUIRED)
FIND_PACKAGE(GnuPatch REQUIRED)
FIND_PACKAGE(GnuSed REQUIRED)

# find gsoap
SET(GSOAP_PATH "/usr/local" CACHE PATH "Path where gSOAP is installed")
FIND_PACKAGE(GSOAP REQUIRED)
SET(GSOAP_PATH ${GSOAP_PATH_FOUND} CACHE PATH "Path where gSOAP is installed" FORCE)

# find host executables
IF(CMAKE_CROSSCOMPILING)
    SET(DPWS_HOSTEXEC_PATH "/usr/local" CACHE PATH "Path where gSOAP is installed")
    FIND_PACKAGE(DPWSHostExecutables REQUIRED)
    SET(DPWS_HOSTEXEC_PATH ${DPWS_HOSTEXEC_PATH_FOUND} CACHE PATH "Path where gSOAP is installed" FORCE)
ENDIF(CMAKE_CROSSCOMPILING)

# optional
FIND_PACKAGE(LATEX)
FIND_PACKAGE(Doxygen)


################################################################################
# Compile time configuration options of ws4d-gsoap                             #
################################################################################

IF(CMAKE_CROSSCOMPILING)
    MESSAGE(STATUS "Doing a cross compile CC=${CMAKE_C_COMPILER}")
ENDIF(CMAKE_CROSSCOMPILING)

OPTION (MAINTAINER_MODE "Activate options for maintenance purpose" OFF)

# Abort on assert() (default: off)
CMAKE_DEPENDENT_OPTION (ABORT_ON_ASSERT "Abort programm on assert" OFF "MAINTAINER_MODE" OFF)
IF(ABORT_ON_ASSERT)
SET (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DABORT_ON_ASSERT")
ENDIF(ABORT_ON_ASSERT)

CMAKE_DEPENDENT_OPTION (WITH_DOCU "Toggle generation of docu" ON "MAINTAINER_MODE" ON)

# Generate PDF documentation (default: on)
CMAKE_DEPENDENT_OPTION (WITH_PDF_DOCU "Toggle generation of pdf docu" ON "MAINTAINER_MODE" ON)

# Unit tests (default: off)
CMAKE_DEPENDENT_OPTION (WITH_UNIT_TESTS "Toggle generation of unit tests" OFF "MAINTAINER_MODE" OFF)

# Activate IPV6 support (default: off)
OPTION (WITH_IPV6 "Toggle IPv6 support" OFF)

IF(WITH_IPV6)
	ADD_DEFINITIONS ( "-DWITH_IPV6")
ENDIF(WITH_IPV6)

# Activate hosting service (default: on)
OPTION (WITH_HOSTING_SERVICE "Toggle generation of hosting_service" ON)

# Activate life cycle manager service (default: off)
OPTION (WITH_LCM "Toggle generation of life cycle manager" OFF)

# Activate gSOAP WS-Security support (default: off)
OPTION (WITH_WS_SECURITY "Toggle gSOAP WS-Security support" OFF)

# Activate DPWS Security support (default: off)
OPTION (WITH_DPWS_SECURITY "Toggle DPWS security support" OFF)
IF (WITH_DPWS_SECURITY)
	FIND_PACKAGE(OpenSSL REQUIRED)
	IF(TOOL_COMPILE_FLAGS)
		SET(TOOL_COMPILE_FLAGS "${TOOL_COMPILE_FLAGS} -DWITH_OPENSSL")
	ELSE(GLOB_COMPILE_FLAGS)
		SET(TOOL_COMPILE_FLAGS "-DWITH_OPENSSL")
	ENDIF(TOOL_COMPILE_FLAGS)
ENDIF (WITH_DPWS_SECURITY)

# DPWS specification version
SET (DPWS_VERSION "2006_02" CACHE STRING "DPWS version: 2006_02 (default), 2009_01")

# Alternative io mode (default: none)
SET (ALT_IO_MODE "none" CACHE STRING "Alternate io support: none (default), lwip")

################################################################################
# Include further cmake files                                                  #
################################################################################

# ws4d-gsoap specific macros
INCLUDE(WS4D-Misc)

# make target to create source package
INCLUDE(PackSourceTar)

# make target to indent all source files
INCLUDE(IndentSources)

# make target to indent all source files
INCLUDE(Uninstall)

################################################################################
# Set paths to build ws4d-gsoap                                                #
################################################################################

# Set the output paths.
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

SET(DPWS_WS_DIR
	${CMAKE_BINARY_DIR}/WS)

SET(DPWS_INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/include)

SET(DPWS_GENERATED_INCLUDE_DIR
	${CMAKE_BINARY_DIR}/include)

SET(DPWS_INCLUDES
	${DPWS_INCLUDE_DIR}
	${DPWS_GENERATED_INCLUDE_DIR})

SET(DPWS_SRC_DIR
	${CMAKE_SOURCE_DIR}/src)

SET(LIBDPWS_SRC_DIR
	${DPWS_SRC_DIR}/libdpws)

################################################################################
# CTest configuration                                                          #
################################################################################

IF(WITH_UNIT_TESTS AND NOT CMAKE_CROSSCOMPILING)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
               COPYONLY)

ENABLE_TESTING()
INCLUDE(CTest)

FIND_PACKAGE(Check REQUIRED)

INCLUDE(TestingMacros)

ENDIF(WITH_UNIT_TESTS AND NOT CMAKE_CROSSCOMPILING)

################################################################################
# Process sub directories                                                      #
################################################################################

ADD_SUBDIRECTORY(WS)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(cmake-modules)
ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(test)


MARK_AS_ADVANCED(
    EXECUTABLE_OUTPUT_PATH
    LIBRARY_OUTPUT_PATH
)
