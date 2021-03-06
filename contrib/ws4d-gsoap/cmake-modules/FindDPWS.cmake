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

# Find DPWS includes and library.
#
# This module defines
#  DPWS_INCLUDE_DIR
#  DPWS_LIBRARIES, the libraries to link against to use DPWS.
#  DPWS_LIB_DIR, the location of the libraries.
#  DPWS_FOUND, If false, do not try to use DPWS
#
# Note: The Option DPWS_PATH should be set in order to find the dpws base
#       directory.
#
# Author: S. Feldhorst <sascha.feldhorst@uni-dortmund.de>

IF(NOT DPWS_FIND_COMPONENTS)
SET (DPWS_FIND_COMPONENTS client device peer)
ENDIF(NOT DPWS_FIND_COMPONENTS)

FIND_PACKAGE(XSLTproc QUIET REQUIRED)

SET(_DPWS_SEARCH_PATHS "")

# if set append DPWS_PATH to path list
IF(DPWS_PATH)
  LIST(APPEND _DPWS_SEARCH_PATHS
    ${DPWS_PATH})
ENDIF(DPWS_PATH)

# if set append DPWS_PATH environment to path list
IF(NOT "$ENV{DPWS_PATH}" STREQUAL "")
  LIST(APPEND _DPWS_SEARCH_PATHS
    "$ENV{DPWS_PATH}")
ENDIF(NOT "$ENV{DPWS_PATH}" STREQUAL "")

# append standard gsoap location directories
LIST(APPEND _DPWS_SEARCH_PATHS
    /usr
    /usr/local
)

SET(DPWS_FOUND FALSE)
SET(DPWS_FOUND_CLIENT FALSE)
SET(DPWS_FOUND_DEVICE FALSE)
SET(DPWS_FOUND_PEER FALSE)
SET(DPWS_FOUND_HOSTED FALSE)

FOREACH(_DPWS_LOCATION ${_DPWS_SEARCH_PATHS})
  IF(NOT DPWS_FOUND)

    SET(DPWS_INC_SEARCH_PATHS
      ${_DPWS_LOCATION}
      ${_DPWS_LOCATION}/include/
      ${_DPWS_LOCATION}/include/ws4d-gSOAP)

    SET(DPWS_WS_SEARCH_PATHS
      ${_DPWS_LOCATION}/WS
      ${_DPWS_LOCATION}/include/WS
      ${_DPWS_LOCATION}/include/ws4d-gSOAP/WS)

    SET(DPWS_LIB_SEARCH_PATHS
      ${_DPWS_LOCATION}
      ${_DPWS_LOCATION}/lib
      ${_DPWS_LOCATION}/lib/ws4d-gSOAP)

    SET(DPWS_TOOLS_SEARCH_PATHS
      ${_DPWS_LOCATION}
      ${_DPWS_LOCATION}/share/ws4d-gSOAP
      ${_DPWS_LOCATION}/src/tools)

    SET(DPWS_BIN_SEARCH_PATHS
      ${_DPWS_LOCATION}
      ${_DPWS_LOCATION}/bin)

    #clear variables
    SET(DPWS_INCLUDE_DIR "DPWS_INCLUDE_DIR-NOTFOUND" CACHE PATH "ws4d include directory" FORCE)
    SET(DPWS_WS_DIR "DPWS_WS_DIR-NOTFOUND" CACHE PATH "ws4d gsoap import directory" FORCE)
    SET(DPWS_LIB_DIR "DPWS_LIB_DIR-NOTFOUND" CACHE PATH "ws4d library directory" FORCE)
    SET(DPWS_TOOLS_DIR "DPWS_TOOLS_DIR-NOTFOUND" CACHE PATH "ws4d tools directory" FORCE)
    SET(DPWS_EMBEDWSDL "DPWS_EMBEDWSDL-NOTFOUND" CACHE PATH "ws4d tool to embed wsdls" FORCE)

    # Search the include dir.
    FIND_PATH(DPWS_INCLUDE_DIR
      NAMES dpws_client.h dpws_device.h dpws_peer.h
      PATHS ${DPWS_INC_SEARCH_PATHS}
      DOC "ws4d include directory"
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH)

    # Search the gsoap include dir.
    FIND_PATH(DPWS_WS_DIR
      NAMES devprof.gsoap
      PATHS ${DPWS_WS_SEARCH_PATHS}
      DOC "ws4d include directory"
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH)

    # Search the libraries.
    FOREACH(_DPWS_LIB_LOCATION ${DPWS_LIB_SEARCH_PATHS})
      IF (NOT DPWS_LIB_DIR)

        #clear variables
        SET(DPWS_LIBRARIES "DPWS_LIBRARIES-NOTFOUND" CACHE PATH "dpws common libraries" FORCE)
        SET(DPWS_CLIENT_LIBRARY "DPWS_CLIENT_LIBRARY-NOTFOUND" CACHE PATH "gsoap includes directory" FORCE)
        SET(DPWS_CLIENTMT_LIBRARY "DPWS_CLIENTMT_LIBRARY-NOTFOUND" CACHE PATH "ws4d tools directory" FORCE)
        SET(DPWS_DEVICE_LIBRARY "DPWS_DEVICE_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_DEVICEMT_LIBRARY "DPWS_DEVICEMT_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_PEER_LIBRARY "DPWS_DEVICE_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_PEERMT_LIBRARY "DPWS_DEVICEMT_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
                SET(DPWS_HOSTED_LIBRARY "DPWS_HOSTED_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
                SET(DPWS_HOSTEDMT_LIBRARY "DPWS_HOSTEDMT_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_LIST_LIBRARY "DPWS_LIST_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_LISTMT_LIBRARY "DPWS_LISTMT_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_ALTIO_LIBRARY "DPWS_ALTIO_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_ALTIOMT_LIBRARY "DPWS_ALTIOMT_LIBRARY-NOTFOUND" CACHE PATH "ws4d bin directory" FORCE)
        SET(DPWS_ALT_IO FALSE CACHE BOOL "ws4d bin directory" FORCE)

        FIND_LIBRARY(DPWS_LIBRARIES
          NAMES dpwsS
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_CLIENT_LIBRARY
          NAMES dpwsC
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_CLIENTMT_LIBRARY
          NAMES dpwsCmt
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_DEVICE_LIBRARY
          NAMES dpwsD
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_DEVICEMT_LIBRARY
          NAMES dpwsDmt
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_PEER_LIBRARY
          NAMES dpwsP
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_PEERMT_LIBRARY
          NAMES dpwsPmt
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_HOSTED_LIBRARY
          NAMES dpwsH
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_HOSTEDMT_LIBRARY
          NAMES dpwsHmt
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_LIST_LIBRARY
          NAMES ws4d_listL
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_LISTMT_LIBRARY
          NAMES ws4d_listLmt
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_ALTIO_LIBRARY
          NAMES dpwsIO
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        FIND_LIBRARY(DPWS_ALTIOMT_LIBRARY
          NAMES dpwsIOmt
          PATHS ${_DPWS_LIB_LOCATION}
          NO_DEFAULT_PATH
          NO_CMAKE_ENVIRONMENT_PATH
          NO_CMAKE_PATH
          NO_SYSTEM_ENVIRONMENT_PATH
          NO_CMAKE_SYSTEM_PATH)

        # for debugging
        #MESSAGE(STATUS "\t_DPWS_LIB_LOCATION: ${_DPWS_LIB_LOCATION}")
        #MESSAGE(STATUS "\t\tDPWS_LIBRARIES: ${DPWS_LIBRARIES}")
        #MESSAGE(STATUS "\t\tDPWS_CLIENT_LIBRARY: ${DPWS_LISTMT_LIBRARY}")
        #MESSAGE(STATUS "\t\tDPWS_CLIENTMT_LIBRARY: ${DPWS_LISTMT_LIBRARY}")
        #MESSAGE(STATUS "\t\tDPWS_DEVICE_LIBRARY: ${DPWS_LISTMT_LIBRARY}")
        #MESSAGE(STATUS "\t\tDPWS_DEVICEMT_LIBRARY: ${DPWS_LISTMT_LIBRARY}")
        #MESSAGE(STATUS "\t\tDPWS_HOSTED_LIBRARY: ${DPWS_LISTMT_LIBRARY}")
        #MESSAGE(STATUS "\t\tDPWS_HOSTEDMT_LIBRARY: ${DPWS_LISTMT_LIBRARY}")
        #MESSAGE(STATUS "\t\tDPWS_LIST_LIBRARY: ${DPWS_LISTMT_LIBRARY}")
        #MESSAGE(STATUS "\t\tDPWS_LISTMT_LIBRARY: ${DPWS_LISTMT_LIBRARY}")

        IF (DPWS_LIBRARIES AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)
          SET(DPWS_LIB_DIR ${_DPWS_LIB_LOCATION} CACHE PATH "ws4d library directory" FORCE)
        ENDIF (DPWS_LIBRARIES AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)

        IF (DPWS_LIBRARIES AND DPWS_CLIENT_LIBRARY AND DPWS_CLIENTMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)
          SET(DPWS_FOUND_CLIENT TRUE)
        ENDIF (DPWS_LIBRARIES AND DPWS_CLIENT_LIBRARY AND DPWS_CLIENTMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)

        IF (DPWS_LIBRARIES AND DPWS_DEVICE_LIBRARY AND DPWS_DEVICEMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)
          SET(DPWS_FOUND_DEVICE TRUE)
        ENDIF (DPWS_LIBRARIES AND DPWS_DEVICE_LIBRARY AND DPWS_DEVICEMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)

        IF (DPWS_LIBRARIES AND DPWS_PEER_LIBRARY AND DPWS_PEERMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)
          SET(DPWS_FOUND_PEER TRUE)
        ENDIF (DPWS_LIBRARIES AND DPWS_PEER_LIBRARY AND DPWS_PEERMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)

        IF (DPWS_LIBRARIES AND DPWS_HOSTED_LIBRARY AND DPWS_HOSTEDMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)
          SET(DPWS_FOUND_HOSTED TRUE)
        ENDIF (DPWS_LIBRARIES AND DPWS_HOSTED_LIBRARY AND DPWS_HOSTEDMT_LIBRARY AND DPWS_LIST_LIBRARY AND DPWS_LISTMT_LIBRARY)

      ENDIF (NOT DPWS_LIB_DIR)
    ENDFOREACH(_DPWS_LIB_LOCATION)

    # Search for the tool directory
    FIND_PATH(DPWS_TOOLS_DIR
      NAMES deviceMetadataToC.xslt deviceMetadataToH.xslt hostedMetadataToC.xslt hostedMetadataToH.xslt
      PATHS ${DPWS_TOOLS_SEARCH_PATHS}
      DOC "ws4d tools directory"
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH)

    IF(DPWS_TOOLS_DIR)
      FIND_FILE(DPWS_DEVICE_METADATATOC
        NAMES deviceMetadataToC.xslt
        PATHS ${DPWS_TOOLS_DIR}
        DOC "ws4d code generation tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

      FIND_FILE(DPWS_DEVICE_METADATATOH
        NAMES deviceMetadataToH.xslt
        PATHS ${DPWS_TOOLS_DIR}
        DOC "ws4d code generation tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

      FIND_FILE(DPWS_HOSTED_METADATATOC
        NAMES hostedMetadataToC.xslt
        PATHS ${DPWS_TOOLS_DIR}
        DOC "ws4d code generation tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

      FIND_FILE(DPWS_HOSTED_METADATATOH
        NAMES hostedMetadataToH.xslt
        PATHS ${DPWS_TOOLS_DIR}
        DOC "ws4d code generation tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)
    ENDIF(DPWS_TOOLS_DIR)

    IF(NOT CMAKE_CROSSCOMPILING)

      # Search for embedwsdl
      FIND_PROGRAM(DPWS_EMBEDWSDL
        NAMES embedwsdl
        PATHS ${DPWS_BIN_SEARCH_PATHS}
        DOC "ws4d bin directory"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

    ELSE(NOT CMAKE_CROSSCOMPILING)

      # Search for embedwsdl
      FIND_PROGRAM(DPWS_EMBEDWSDL
        NAMES embedwsdl_host
        PATHS ${DPWS_BIN_SEARCH_PATHS}
        DOC "ws4d bin directory"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

    ENDIF(NOT CMAKE_CROSSCOMPILING)

    #for debugging
    #MESSAGE(STATUS "_DPWS_LOCATION: ${_DPWS_LOCATION}")
    #MESSAGE(STATUS "\tDPWS_INCLUDE_DIR: ${DPWS_INCLUDE_DIR}")
    #MESSAGE(STATUS "\tDPWS_WS_DIR: ${DPWS_WS_DIR}")
    #MESSAGE(STATUS "\tDPWS_LIB_DIR: ${DPWS_LIB_DIR}")
    #MESSAGE(STATUS "\tDPWS_TOOLS_DIR: ${DPWS_TOOLS_DIR}")
    #MESSAGE(STATUS "\tDPWS_BIN_DIR: ${DPWS_BIN_DIR}")
    #MESSAGE(STATUS "\tDPWS_BIN_SEARCH_PATHS: ${DPWS_BIN_SEARCH_PATHS}")
    #MESSAGE(STATUS "\tDPWS_EMBEDWSDL: ${DPWS_EMBEDWSDL}")

    # Determine if gSOAP was found
    IF(DPWS_INCLUDE_DIR AND DPWS_WS_DIR AND DPWS_LIB_DIR AND DPWS_TOOLS_DIR AND DPWS_EMBEDWSDL)
      SET(DPWS_FOUND TRUE)
      SET(DPWS_PATH_FOUND ${_DPWS_LOCATION})
    ENDIF(DPWS_INCLUDE_DIR AND DPWS_WS_DIR AND DPWS_LIB_DIR AND DPWS_TOOLS_DIR AND DPWS_EMBEDWSDL)

    IF (DPWS_ALTIO_LIBRARY AND DPWS_ALTIOMT_LIBRARY)
      SET(DPWS_ALT_IO ON CACHE BOOL "ws4d bin directory" FORCE)
    ENDIF (DPWS_ALTIO_LIBRARY AND DPWS_ALTIOMT_LIBRARY)
  ENDIF(NOT DPWS_FOUND)
ENDFOREACH(_DPWS_LOCATION)

IF(DPWS_FOUND AND DPWS_FOUND_CLIENT AND DPWS_FIND_COMPONENTS MATCHES client)

  IF(DPWS_ALTIO_LIBRARY)
    SET(DPWS-C_LIBRARIES
      ${DPWS_CLIENT_LIBRARY}
      ${DPWS_ALTIO_LIBRARY}
      ${DPWS_LIST_LIBRARY}
      CACHE STRING ""
      FORCE)
  ELSE(DPWS_ALTIO_LIBRARY)
    SET(DPWS-C_LIBRARIES
      ${DPWS_CLIENT_LIBRARY}
      ${DPWS_LIST_LIBRARY}
      CACHE STRING ""
      FORCE)
  ENDIF(DPWS_ALTIO_LIBRARY)

  IF(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-CMT_LIBRARIES
      ${DPWS_CLIENTMT_LIBRARY}
      ${DPWS_ALTIOMT_LIBRARY}
      ${DPWS_LISTMT_LIBRARY}
      CACHE STRING ""
      FORCE)
  ELSE(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-CMT_LIBRARIES
      ${DPWS_CLIENTMT_LIBRARY}
      ${DPWS_LISTMT_LIBRARY}
      CACHE STRING ""
      FORCE)
  ENDIF(DPWS_ALTIOMT_LIBRARY)

  SET(DPWS-C_LIB_DIR
    ${DPWS_LIB_DIR}
    CACHE STRING ""
    FORCE)
ELSEIF(NOT DPWS_FOUND_CLIENT AND DPWS_FIND_COMPONENTS MATCHES client)

  SET(DPWS-C_LIBRARIES
    "DPWS-C_LIBRARIES-NOTFOUND"
    CACHE STRING "" )

  SET(DPWS-CMT_LIBRARIES
    "DPWS-CMT_LIBRARIES-NOTFOUND"
    CACHE STRING ""
    FORCE)

  SET(DPWS-C_LIB_DIR
    "DPWS-C_LIB_DIR-NOTFOUND"
    CACHE STRING ""
    FORCE)

ENDIF(DPWS_FOUND AND DPWS_FOUND_CLIENT AND DPWS_FIND_COMPONENTS MATCHES client)

IF(DPWS_FOUND AND DPWS_FOUND_DEVICE AND DPWS_FIND_COMPONENTS MATCHES device)
  
  IF(DPWS_ALTIO_LIBRARY)
    SET(DPWS-D_LIBRARIES
      ${DPWS_DEVICE_LIBRARY}
      ${DPWS_ALTIO_LIBRARY}
      ${DPWS_LIST_LIBRARY}
      CACHE STRING ""
      FORCE)
  ELSE(DPWS_ALTIO_LIBRARY)
    SET(DPWS-D_LIBRARIES
      ${DPWS_DEVICE_LIBRARY}
      ${DPWS_LIST_LIBRARY}
      CACHE STRING ""
      FORCE)
  ENDIF(DPWS_ALTIO_LIBRARY)

  IF(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-DMT_LIBRARIES
      ${DPWS_DEVICEMT_LIBRARY}
      ${DPWS_ALTIOMT_LIBRARY}
      ${DPWS_LISTMT_LIBRARY}
      CACHE STRING ""
      FORCE)
  ELSE(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-DMT_LIBRARIES
      ${DPWS_DEVICEMT_LIBRARY}
      ${DPWS_LISTMT_LIBRARY}
      CACHE STRING ""
      FORCE)
  ENDIF(DPWS_ALTIOMT_LIBRARY)

  SET(DPWS-D_LIB_DIR
    ${DPWS_LIB_DIR}
    CACHE STRING ""
    FORCE)

ELSEIF(NOT DPWS_FOUND_DEVICE AND DPWS_FIND_COMPONENTS MATCHES device)

  SET(DPWS-D_LIBRARIES
    "DPWS-D_LIBRARIES-NOTFOUND"
    CACHE STRING ""
    FORCE)

  SET(DPWS-DMT_LIBRARIES
    "DPWS-DMT_LIBRARIES-NOTFOUND"
    CACHE STRING ""
    FORCE)

  SET(DPWS-D_LIB_DIR
    "DPWS-D_LIB_DIR-NOTFOUND"
    CACHE STRING ""
    FORCE)

ENDIF(DPWS_FOUND AND DPWS_FOUND_DEVICE AND DPWS_FIND_COMPONENTS MATCHES device)

IF(DPWS_FOUND AND DPWS_FOUND_DEVICE AND DPWS_FIND_COMPONENTS MATCHES peer)

  IF(DPWS_ALTIO_LIBRARY)
    SET(DPWS-P_LIBRARIES
      ${DPWS_PEER_LIBRARY}
      ${DPWS_ALTIO_LIBRARY}
      ${DPWS_LIST_LIBRARY}
      CACHE STRING ""
      FORCE)
  ELSE(DPWS_ALTIO_LIBRARY)
    SET(DPWS-P_LIBRARIES
      ${DPWS_PEER_LIBRARY}
      ${DPWS_LIST_LIBRARY}
      CACHE STRING ""
      FORCE)
  ENDIF(DPWS_ALTIO_LIBRARY)

  IF(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-PMT_LIBRARIES
      ${DPWS_PEERMT_LIBRARY}
      ${DPWS_ALTIOMT_LIBRARY}
      ${DPWS_LISTMT_LIBRARY}
      CACHE STRING ""
      FORCE)
  ELSE(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-PMT_LIBRARIES
      ${DPWS_PEERMT_LIBRARY}
      ${DPWS_LISTMT_LIBRARY}
      CACHE STRING ""
      FORCE)
  ENDIF(DPWS_ALTIOMT_LIBRARY)

  SET(DPWS-P_LIB_DIR
    ${DPWS_LIB_DIR}
    CACHE STRING ""
    FORCE)

ELSEIF(NOT DPWS_FOUND_DEVICE AND DPWS_FIND_COMPONENTS MATCHES peer)

  SET(DPWS-P_LIBRARIES
    "DPWS-P_LIBRARIES-NOTFOUND"
    CACHE STRING ""
    FORCE)

  SET(DPWS-PMT_LIBRARIES
    "DPWS-PMT_LIBRARIES-NOTFOUND"
    CACHE STRING ""
    FORCE)

  SET(DPWS-P_LIB_DIR
    "DPWS-P_LIB_DIR-NOTFOUND"
    CACHE STRING ""
    FORCE)

ENDIF(DPWS_FOUND AND DPWS_FOUND_DEVICE AND DPWS_FIND_COMPONENTS MATCHES peer)

IF(DPWS_FOUND AND DPWS_FOUND_HOSTED AND DPWS_FIND_COMPONENTS MATCHES hosted)

  IF(DPWS_ALTIO_LIBRARY)
    SET(DPWS-H_LIBRARIES
        ${DPWS_HOSTED_LIBRARY}
        ${DPWS_ALTIO_LIBRARY}
        ${DPWS_LIST_LIBRARY}
        CACHE STRING ""
        FORCE)
  ELSE(DPWS_ALTIO_LIBRARY)
    SET(DPWS-H_LIBRARIES
        ${DPWS_HOSTED_LIBRARY}
        ${DPWS_LIST_LIBRARY}
        CACHE STRING ""
        FORCE)
  ENDIF(DPWS_ALTIO_LIBRARY)

  IF(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-HMT_LIBRARIES
        ${DPWS_HOSTEDMT_LIBRARY}
        ${DPWS_ALTIOMT_LIBRARY}
        ${DPWS_LISTMT_LIBRARY}
        CACHE STRING ""
        FORCE)
  ELSE(DPWS_ALTIOMT_LIBRARY)
    SET(DPWS-HMT_LIBRARIES
        ${DPWS_HOSTEDMT_LIBRARY}
        ${DPWS_LISTMT_LIBRARY}
        CACHE STRING ""
        FORCE)
  ENDIF(DPWS_ALTIOMT_LIBRARY)

  SET(DPWS-H_LIB_DIR
      ${DPWS_LIB_DIR}
      CACHE STRING ""
      FORCE)

ELSEIF(DPWS_FOUND AND DPWS_FOUND_HOSTED AND DPWS_FIND_COMPONENTS MATCHES hosted)

    SET(DPWS-H_LIBRARIES
        "DPWS-H_LIBRARIES-NOTFOUND"
        CACHE STRING ""
        FORCE)

    SET(DPWS-HMT_LIBRARIES
        "DPWS-HMT_LIBRARIES-NOTFOUND"
        CACHE STRING ""
        FORCE)

    SET(DPWS-H_LIB_DIR
        "DPWS-H_LIB_DIR-NOTFOUND"
        CACHE STRING ""
        FORCE)

ENDIF(DPWS_FOUND AND DPWS_FOUND_HOSTED AND DPWS_FIND_COMPONENTS MATCHES hosted)

# Finally deal with the result of our search :-)
IF(DPWS_FOUND)

  IF(DPWS_FIND_COMPONENTS MATCHES device AND NOT DPWS_FOUND_DEVICE)

    IF(DPWS_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could not find DPWS device component")
    ELSE(DPWS_FIND_REQUIRED)
      IF ( NOT DPWS_FIND_QUIETLY )
        MESSAGE(STATUS "Could not find DPWS device component")
      ENDIF ( NOT DPWS_FIND_QUIETLY )
    ENDIF(DPWS_FIND_REQUIRED)

  ENDIF(DPWS_FIND_COMPONENTS MATCHES device AND NOT DPWS_FOUND_DEVICE)

  IF(DPWS_FIND_COMPONENTS MATCHES peer AND NOT DPWS_FOUND_PEER)

    IF(DPWS_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could not find DPWS peer component")
    ELSE(DPWS_FIND_REQUIRED)
      IF ( NOT DPWS_FIND_QUIETLY )
        MESSAGE(STATUS "Could not find DPWS peer component")
      ENDIF ( NOT DPWS_FIND_QUIETLY )
    ENDIF(DPWS_FIND_REQUIRED)

  ENDIF(DPWS_FIND_COMPONENTS MATCHES peer AND NOT DPWS_FOUND_PEER)

  IF(DPWS_FIND_COMPONENTS MATCHES client AND NOT DPWS_FOUND_CLIENT)

    IF(DPWS_FIND_REQUIRED)
      MESSAGE(SEND_ERROR "Could not find DPWS client component")
    ELSE(DPWS_FIND_REQUIRED)
      IF ( NOT DPWS_FIND_QUIETLY )
        MESSAGE(STATUS "Could not find DPWS client component")
      ENDIF ( NOT DPWS_FIND_QUIETLY )
    ENDIF(DPWS_FIND_REQUIRED)

  ENDIF(DPWS_FIND_COMPONENTS MATCHES client AND NOT DPWS_FOUND_CLIENT)

    IF(DPWS_FIND_COMPONENTS MATCHES hosted AND NOT DPWS_FOUND_HOSTED)

        IF(DPWS_FIND_REQUIRED)
            MESSAGE(SEND_ERROR "Could not find DPWS hosted component")
        ELSE(DPWS_FIND_REQUIRED)
            IF ( NOT DPWS_FIND_QUIETLY )
                MESSAGE(STATUS "Could not find DPWS hosted component")
            ENDIF ( NOT DPWS_FIND_QUIETLY )
        ENDIF(DPWS_FIND_REQUIRED)

    ENDIF(DPWS_FIND_COMPONENTS MATCHES hosted AND NOT DPWS_FOUND_HOSTED)

  IF ( NOT DPWS_FIND_QUIETLY )
    MESSAGE(STATUS "Found DPWS in ${DPWS_PATH_FOUND}")
  ENDIF ( NOT DPWS_FIND_QUIETLY )

ELSE(DPWS_FOUND)
  IF(DPWS_FIND_REQUIRED)
    MESSAGE(SEND_ERROR "Could not find DPWS in ${_DPWS_SEARCH_PATHS}")
  ELSE(DPWS_FIND_REQUIRED)
    IF ( NOT DPWS_FIND_QUIETLY )
      MESSAGE(STATUS "Could not find DPWS in ${_DPWS_SEARCH_PATHS}")
    ENDIF ( NOT DPWS_FIND_QUIETLY )
  ENDIF(DPWS_FIND_REQUIRED)
ENDIF(DPWS_FOUND)

MACRO(DPWS_METADATA_GENERATE metadata_file prefix destination)
  IF(XSLTPROC_EXECUTABLE)
    SET(dpws_metadata_generated_files
      ${destination}/${prefix}_metadata.c
      ${destination}/${prefix}_metadata.h)

    IF(${ARGC} GREATER 3)

      FOREACH(target ${ARGN})
        IF(target MATCHES "device")
          SET(DPWS_METADATATOC ${DPWS_DEVICE_METADATATOC})
          SET(DPWS_METADATATOH ${DPWS_DEVICE_METADATATOH})
        ELSEIF(target MATCHES "hosted")
          SET(DPWS_METADATATOC ${DPWS_HOSTED_METADATATOC})
          SET(DPWS_METADATATOH ${DPWS_HOSTED_METADATATOH})
        ELSE(target MATCHES "device")
          MESSAGE(SEND_ERROR "target \"${target}\" not supported in DPWS_METADATA_GENERATE")
        ENDIF(target MATCHES "device")
      ENDFOREACH(target)

    ELSE(${ARGC} GREATER 3)
      MESSAGE(STATUS "Choosing target is: device - none specified")
      SET(DPWS_METADATATOC ${DPWS_DEVICE_METADATATOC})
      SET(DPWS_METADATATOH ${DPWS_DEVICE_METADATATOH})
    ENDIF(${ARGC} GREATER 3)

    ADD_CUSTOM_COMMAND(OUTPUT ${dpws_metadata_generated_files}
      COMMAND ${XSLTPROC_EXECUTABLE} --stringparam nsprefix ${prefix} ${DPWS_METADATATOC} ${CMAKE_CURRENT_SOURCE_DIR}/${metadata_file} > ${destination}/${prefix}_metadata.c
      COMMAND ${XSLTPROC_EXECUTABLE} --stringparam nsprefix ${prefix} ${DPWS_METADATATOH} ${CMAKE_CURRENT_SOURCE_DIR}/${metadata_file} > ${destination}/${prefix}_metadata.h
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${metadata_file})
  ENDIF(XSLTPROC_EXECUTABLE)
ENDMACRO(DPWS_METADATA_GENERATE)

MACRO(DPWS_EMBEDD_WSDL destination prefix)
  IF(DPWS_EMBEDWSDL)
    SET(dpws_wsdl_generated_files
      ${destination}/${prefix}_wsdl.c
      ${destination}/${prefix}_wsdl.h)
    ADD_CUSTOM_COMMAND(OUTPUT ${dpws_wsdl_generated_files}
      COMMAND "${DPWS_EMBEDWSDL}" -d ${destination} -p ${prefix} ${ARGN}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${ARGN})
  ELSE(DPWS_EMBEDWSDL)
    SET(dpws_wsdl_generated_files
      ${destination}/${prefix}_wsdl.c
      ${destination}/${prefix}_wsdl.h)
    ADD_CUSTOM_COMMAND(OUTPUT ${dpws_wsdl_generated_files}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${ARGN})
  ENDIF(DPWS_EMBEDWSDL)
ENDMACRO(DPWS_EMBEDD_WSDL)

MARK_AS_ADVANCED(
  DPWS_LIBRARIES
  DPWS_CLIENT_LIBRARY
  DPWS_CLIENTMT_LIBRARY
  DPWS_DEVICE_LIBRARY
  DPWS_DEVICEMT_LIBRARY
  DPWS_PEER_LIBRARY
  DPWS_PEERMT_LIBRARY
  DPWS_HOSTED_LIBRARY
  DPWS_HOSTEDMT_LIBRARY
  DPWS_LIST_LIBRARY
  DPWS_LISTMT_LIBRARY
  DPWS_METADATATOC
  DPWS_METADATATOH
  DPWS_EMBEDWSDL
  DPWS_INCLUDE_DIR
  DPWS_WS_DIR
  DPWS_LIB_DIR
  DPWS_TOOLS_DIR
  DPWS_LIBRARIES
  DPWS_FOUND
  DPWS-C_LIBRARIES
  DPWS-CMT_LIBRARIES
  DPWS-C_LIB_DIR
  DPWS-D_LIBRARIES
  DPWS-DMT_LIBRARIES
  DPWS-D_LIB_DIR
  DPWS-P_LIBRARIES
  DPWS-PMT_LIBRARIES
  DPWS-P_LIB_DIR
  DPWS-H_LIBRARIES
  DPWS-HMT_LIBRARIES
  DPWS-H_LIB_DIR
  DPWS_DEVICE_METADATATOC
  DPWS_DEVICE_METADATATOH
  DPWS_HOSTED_METADATATOC
  DPWS_HOSTED_METADATATOH
  DPWS_ALTIO_LIBRARY
  DPWS_ALTIOMT_LIBRARY
  DPWS_ALT_IO
)
