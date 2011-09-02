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

# Find gSOAP includes, source files and tools.
#
# This module defines
#  GSOAP_INCLUDE_DIR
#  GSOAP_SOAPCPP2, the soapcpp2 tool.
#  GSOAP_WSDL2H, the wsdl2h tool.
#  GSOAP_STDSOAP2_SOURCE, the C source file to build in gSOAP functionalities.
#  GSOAP_FOUND, If false, do not try to use gSOAP
#
# Note: The Option GSOAP_PATH should be set in order to find the gSOAP base
#       directory.
#
# Author: S. Feldhorst <sascha.feldhorst@uni-dortmund.de>

# Search stdsoap2.c and stdsoap2.h

# list of paths search for gsoap
SET(_GSOAP_SEARCH_PATHS "")

# if set append GSOAP_PATH to path list
IF(GSOAP_PATH)
  LIST(APPEND _GSOAP_SEARCH_PATHS
    ${GSOAP_PATH})
ENDIF(GSOAP_PATH)

# if set append GSOAP_PATH environment to path list
IF(NOT "$ENV{GSOAP_PATH}" STREQUAL "")
  LIST(APPEND _GSOAP_SEARCH_PATHS
    "$ENV{GSOAP_PATH}")
ENDIF(NOT "$ENV{GSOAP_PATH}" STREQUAL "")

# append standard gsoap location directories
LIST(APPEND _GSOAP_SEARCH_PATHS
    /usr
    /usr/local
)

FIND_PACKAGE(GnuPatch QUIET REQUIRED)

SET(GSOAP_FOUND FALSE)
FOREACH(_GSOAP_LOCATION ${_GSOAP_SEARCH_PATHS})
  IF(NOT GSOAP_FOUND)

    SET(GSOAP_LIB_SEARCH_PATHS
      ${_GSOAP_LOCATION}
      ${_GSOAP_LOCATION}/lib
      ${_GSOAP_LOCATION}/soapcpp2
      ${_GSOAP_LOCATION}/gsoap)

    SET(GSOAP_RT_SEARCH_PATHS
      ${_GSOAP_LOCATION}
      ${_GSOAP_LOCATION}/include
      ${_GSOAP_LOCATION}/share/gsoap
      ${_GSOAP_LOCATION}/soapcpp2
      ${_GSOAP_LOCATION}/gsoap)

    SET(GSOAP_BIN_SEARCH_PATHS
      ${_GSOAP_LOCATION}
      ${_GSOAP_LOCATION}/bin
      ${_GSOAP_LOCATION}/soapcpp2
      ${_GSOAP_LOCATION}/gsoap/bin/win32
      ${_GSOAP_LOCATION}/gsoap/bin/linux386)

    #clear variables
    SET(GSOAP_STDSOAP2_SOURCE "GSOAP_STDSOAP2_SOURCE-NOTFOUND" CACHE PATH "gsoap c runtime" FORCE)
    SET(GSOAP_INCLUDE_DIR "GSOAP_INCLUDE_DIR-NOTFOUND" CACHE PATH "gsoap includes directory" FORCE)
    SET(GSOAP_SOAPCPP2 "GSOAP_SOAPCPP2-NOTFOUND" CACHE PATH "soapcpp2 tool" FORCE)
    SET(GSOAP_WSDL2H "GSOAP_WSDL2H-NOTFOUND" CACHE PATH "wsdl2h tool" FORCE)

    # Search the runtime
    FIND_FILE(GSOAP_LIB_DIR
      NAMES libgsoap.a
      PATHS ${GSOAP_LIB_SEARCH_PATHS}
      DOC "gsoap c runtime"
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH)

    FIND_PATH(GSOAP_INCLUDE_DIR
      NAMES stdsoap2.h
      PATHS ${GSOAP_RT_SEARCH_PATHS}
      DOC "gsoap includes directory"
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH)

    # Search the tools.
    IF(WIN32) # Windows

      FIND_PROGRAM(GSOAP_SOAPCPP2
        NAMES soapcpp2.exe
        PATHS ${GSOAP_BIN_SEARCH_PATHS}
        DOC "soapcpp2 tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

      FIND_PROGRAM(GSOAP_WSDL2H
        NAMES wsdl2h.exe
        PATHS ${GSOAP_BIN_SEARCH_PATHS}
        DOC "wsdl2h tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

    ELSE(WIN32) # Unix

      FIND_PROGRAM(GSOAP_SOAPCPP2
        NAMES soapcpp2
        PATHS ${GSOAP_BIN_SEARCH_PATHS}
        DOC "soapcpp2 tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

      FIND_PROGRAM(GSOAP_WSDL2H
        NAMES wsdl2h
        PATHS ${GSOAP_BIN_SEARCH_PATHS}
        DOC "wsdl2h tool"
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

    ENDIF(WIN32)

    # Determine if gSOAP was found
    IF(GSOAP_INCLUDE_DIR AND GSOAP_LIB_DIR AND GSOAP_SOAPCPP2 AND GSOAP_WSDL2H)
  SET(GSOAP_FOUND TRUE)
      SET(GSOAP_PATH_FOUND ${_GSOAP_LOCATION})
    ENDIF(GSOAP_INCLUDE_DIR AND GSOAP_LIB_DIR AND GSOAP_SOAPCPP2 AND GSOAP_WSDL2H)

  ENDIF(NOT GSOAP_FOUND)
ENDFOREACH(_GSOAP_LOCATION)

# Define the variables, which shall be set.
IF(GSOAP_FOUND)

  IF (WIN32)

    SET(GSOAP_IMPORT_SEPARATOR
      "\\;"
      CACHE
      STRING "Separator of directories passed with -I parameter to soapcpp2" FORCE)

  ELSE (WIN32)

    SET(GSOAP_IMPORT_SEPARATOR
      ":"
      CACHE
      STRING "Separator of directories passed with -I parameter to soapcpp2" FORCE)

  ENDIF (WIN32)

ELSE(GSOAP_FOUND)
  SET(GSOAP_STDSOAP2_SOURCE "")
  SET(GSOAP_INCLUDE_DIR "")
ENDIF(GSOAP_FOUND)

# Finally deal with the result of our search :-)
IF(GSOAP_FOUND)
  IF ( NOT GSOAP_FIND_QUIETLY )
    MESSAGE(STATUS "Found gSOAP in: ${GSOAP_PATH_FOUND}")
  ENDIF ( NOT GSOAP_FIND_QUIETLY )
ELSE(GSOAP_FOUND)
  IF(GSOAP_FIND_REQUIRED)
    MESSAGE(SEND_ERROR "Could not find gSOAP in: ${_GSOAP_SEARCH_PATHS}")
  ELSE(GSOAP_FIND_REQUIRED)
    IF ( NOT GSOAP_FIND_QUIETLY )
      MESSAGE(STATUS "Could not find gSOAPin: ${_GSOAP_SEARCH_PATHS}")
    ENDIF ( NOT GSOAP_FIND_QUIETLY )
  ENDIF(GSOAP_FIND_REQUIRED)
ENDIF(GSOAP_FOUND)

IF ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.5)
MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})
    SET(larg_names ${arg_names})
    LIST(FIND larg_names "${arg}" is_arg_name)
    IF (is_arg_name GREATER -1)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET(loption_names ${option_names})
      LIST(FIND loption_names "${arg}" is_option)
      IF (is_option GREATER -1)
       SET(${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
       SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)
ELSE ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.5)
MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})
    LIST_CONTAINS(is_arg_name ${arg} ${arg_names})
    IF (is_arg_name)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name)
      LIST_CONTAINS(is_option ${arg} ${option_names})
      IF (is_option)
  SET(${prefix}_${arg} TRUE)
      ELSE (is_option)
  SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option)
    ENDIF (is_arg_name)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)
ENDIF ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.5)

MACRO(EXTEND_SRC_COMPILE_FLAGS)
  PARSE_ARGUMENTS(EXTEND_COMPILE_FLAGS "FLAGS" "" ${ARGN})
  FOREACH(file ${EXTEND_COMPILE_FLAGS_DEFAULT_ARGS})
    GET_SOURCE_FILE_PROPERTY(old_flags ${file} COMPILE_FLAGS)
    IF(old_flags)
      SET(new_flags "${old_flags} ${EXTEND_COMPILE_FLAGS_FLAGS}")
    ELSE(old_flags)
      SET(new_flags "${EXTEND_COMPILE_FLAGS_FLAGS}")
    ENDIF(old_flags)
    SET_SOURCE_FILES_PROPERTIES(${file}
      PROPERTIES COMPILE_FLAGS ${new_flags})
  ENDFOREACH(file)
ENDMACRO(EXTEND_SRC_COMPILE_FLAGS)

MACRO(EXTEND_TGT_COMPILE_FLAGS)
  PARSE_ARGUMENTS(EXTEND_COMPILE_FLAGS "FLAGS" "" ${ARGN})
  FOREACH(file ${EXTEND_COMPILE_FLAGS_DEFAULT_ARGS})
    GET_TARGET_PROPERTY(old_flags ${file} COMPILE_FLAGS)
    IF(old_flags)
      SET(new_flags "${old_flags} ${EXTEND_COMPILE_FLAGS_FLAGS}")
    ELSE(old_flags)
      SET(new_flags "${EXTEND_COMPILE_FLAGS_FLAGS}")
    ENDIF(old_flags)
    SET_TARGET_PROPERTIES(${file}
      PROPERTIES COMPILE_FLAGS ${new_flags})
  ENDFOREACH(file)
ENDMACRO(EXTEND_TGT_COMPILE_FLAGS)

MACRO(GSOAP_SET_RUNTIME_FLAGS flags)
  IF(GSOAP_FOUND)
    EXTEND_SRC_COMPILE_FLAGS ( ${GSOAP_STDSOAP2_SOURCE} FLAGS ${flags})
  ENDIF(GSOAP_FOUND)
ENDMACRO(GSOAP_SET_RUNTIME_FLAGS)

MACRO(GSOAP_GENERATE gsoap_file prefix destination)

  # Search for gsoap file to use
  IF(NOT EXISTS ${gsoap_file})
    GET_SOURCE_FILE_PROPERTY(gsoap_file_is_generated ${gsoap_file} GENERATED)
    IF(${gsoap_file_is_generated})
      SET(used_gsoap_file ${gsoap_file})
    ELSE(${gsoap_file_is_generated})
      IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
        SET(used_gsoap_file ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
      ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
        MESSAGE(FATAL_ERROR "Cant find file ${gsoap_file}")
      ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
    ENDIF(${gsoap_file_is_generated})
  ELSE(NOT EXISTS ${gsoap_file})
    SET(used_gsoap_file ${gsoap_file})
  ENDIF(NOT EXISTS ${gsoap_file})

  # declare generated files
  SET(gsoap_generated_libs
    ${destination}/${prefix}ServerLib.c
    ${destination}/${prefix}Server.c
    ${destination}/${prefix}ClientLib.c
    ${destination}/${prefix}Client.c
    ${destination}/${prefix}C.c
    ${destination}/${prefix}H.h
    ${destination}/${prefix}Stub.h
    ${destination}/${prefix}.nsmap)
  SET_SOURCE_FILES_PROPERTIES( ${gsoap_generated_libs}
    PROPERTIES GENERATED TRUE)

  # run gsoap code generator
  ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_generated_libs}
    COMMAND ${GSOAP_SOAPCPP2} -2 -can -xw -p${prefix}  -I${DPWS_WS_DIR} -I${GSOAP_INCLUDE_DIR}/../share/gsoap/import/ -d${destination} ${used_gsoap_file}
    DEPENDS ${used_gsoap_file})

  # apply patches to generated code
  IF(${ARGC} GREATER 3)
    FOREACH(patch ${ARGN})
      ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_generated_libs}
        COMMAND ${GNU_PATCH_EXECUTABLE} -u -d ${gen_DIR} -i ${patch}
        APPEND)
    ENDFOREACH(patch)
  ENDIF(${ARGC} GREATER 3)

ENDMACRO(GSOAP_GENERATE)

MACRO(GSOAP_GENERATE_DATABINDING gsoap_file prefix destination)

  # Search for gsoap file to use
  IF(NOT EXISTS ${gsoap_file})
    GET_SOURCE_FILE_PROPERTY(gsoap_file_is_generated ${gsoap_file} GENERATED)
    IF(${gsoap_file_is_generated})
      SET(used_gsoap_file ${gsoap_file})
    ELSE(${gsoap_file_is_generated})
      IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
        SET(used_gsoap_file ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
      ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
        MESSAGE(FATAL_ERROR "Cant find file ${gsoap_file}")
      ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${gsoap_file})
    ENDIF(${gsoap_file_is_generated})
  ELSE(NOT EXISTS ${gsoap_file})
    SET(used_gsoap_file ${gsoap_file})
  ENDIF(NOT EXISTS ${gsoap_file})

  # declare generated files
  SET(gsoap_generated_libs
    ${destination}/${prefix}C.c
    ${destination}/${prefix}H.h
    ${destination}/${prefix}Stub.h)
  SET_SOURCE_FILES_PROPERTIES( ${gsoap_generated_libs}
    PROPERTIES GENERATED TRUE)

  # run gsoap code generator
    ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_generated_libs}
      COMMAND ${GSOAP_SOAPCPP2} -2 -CS -xw -cp${prefix} -I${DPWS_WS_DIR}  -I${GSOAP_INCLUDE_DIR}/../share/gsoap/import/ -d${destination} ${used_gsoap_file}
      DEPENDS ${used_gsoap_file})

  # apply patches to generated code
  IF(${ARGC} GREATER 3)
    FOREACH(patch ${ARGN})
      ADD_CUSTOM_COMMAND(OUTPUT ${gsoap_generated_libs}
        COMMAND ${GNU_PATCH_EXECUTABLE} -u -d ${gen_DIR} -i ${patch}
        APPEND)
    ENDFOREACH(patch)
  ENDIF(${ARGC} GREATER 3)

ENDMACRO(GSOAP_GENERATE_DATABINDING)

MARK_AS_ADVANCED(
  _GSOAP_SEARCH_PATHS
  _GSOAP_LOCATION
    GSOAP_FOUND
  GSOAP_PATH_FOUND
    GSOAP_SOAPCPP2
    GSOAP_WSDL2H
    GSOAP_STDSOAP2_SOURCE
    GSOAP_INCLUDE_DIR
  GSOAP_IMPORT_SEPARATOR
)
