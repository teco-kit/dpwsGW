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

# A collection of little helpers.

# A small macro to add a specified suffix to a list of
# filenames for example .c
#
# Note: This macro uses double evaluator so use it this way
# ADD_SUFFIX(<varname-containin-src-list> <suffix)
# example:
# SET(foo_SRCS main test etc)
# ADD_SUFFIX(foo_SRCS .c)
#
# Thanks to the chicken project (www.call-with-current-continuation.org)
MACRO(ADD_SUFFIX rootlist suffix)
  SET(outlist )
  FOREACH(root ${${rootlist}})
    LIST(APPEND outlist ${root}${suffix})
  ENDFOREACH(root)
  SET(${rootlist} ${outlist})
ENDMACRO(ADD_SUFFIX)

# Analogon to ADD_SUFFIX macro.
#
# Thanks to the chicken project (www.call-with-current-continuation.org)
MACRO(ADD_PREFIX prefix rootlist)
  SET(outlist )
  FOREACH(root ${${rootlist}})
    LIST(APPEND outlist ${prefix}${root})
  ENDFOREACH(root)
  SET(${rootlist} ${outlist})
ENDMACRO(ADD_PREFIX)

MACRO(DPWS_METADATA_GENERATE metadata_file prefix destination)
  IF(XSLTPROC_EXECUTABLE)
    SET(dpws_metadata_generated_files
      ${destination}/${prefix}_metadata.c
      ${destination}/${prefix}_metadata.h)
    SET_SOURCE_FILES_PROPERTIES(${dpws_metadata_generated_files}
      PROPERTIES GENERATED TRUE)

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
  SET(dpws_wsdl_generated_files
    ${destination}/${prefix}_wsdl.c
    ${destination}/${prefix}_wsdl.h)
  SET_SOURCE_FILES_PROPERTIES( ${dpws_wsdl_generated_files}
    PROPERTIES GENERATED TRUE)

  IF(DPWS_EMBEDWSDL)
    ADD_CUSTOM_COMMAND(OUTPUT ${dpws_wsdl_generated_files}
      COMMAND "${DPWS_EMBEDWSDL}" -d ${destination} -p ${prefix} ${ARGN}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${ARGN})
  ELSE(DPWS_EMBEDWSDL)
    ADD_CUSTOM_COMMAND(OUTPUT ${dpws_wsdl_generated_files}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${ARGN})
  ENDIF(DPWS_EMBEDWSDL)
ENDMACRO(DPWS_EMBEDD_WSDL)

MACRO(DO_WSDL2H outfiles opts typemap)
  FOREACH (it ${ARGN})
    GET_FILENAME_COMPONENT(name ${it} 	NAME_WE)
    SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
        IF(CYGWIN)
            ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
                COMMAND ${GSOAP_WSDL2H} ARGS ${opts} -t\"$\(shell cygpath -am ${typemap}\)\" -n\"$\(shell cygpath -am ${name}\)\" -o\"$\(shell cygpath -am ${outfile}\)\" \"$\(shell cygpath -am ${it}\)\"
                COMMAND ${GNU_SED_EXECUTABLE} ARGS -i -e '/^..gsoapopt/d' ${outfile}
                DEPENDS ${it} ${typemap})
            ELSE(CYGWIN)
            ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
                COMMAND ${GSOAP_WSDL2H} ARGS ${opts} -t${typemap} -n${name} -o${outfile} ${it}
                COMMAND ${GNU_SED_EXECUTABLE} ARGS -i -e '/^..gsoapopt/d' ${outfile}
                DEPENDS ${it} ${typemap})
            ENDIF(CYGWIN)
    SET(${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)
ENDMACRO(DO_WSDL2H)

INCLUDE (CheckFunctionExists)

CHECK_FUNCTION_EXISTS ("inet_pton" HAVE_INET_PTON_H)
CHECK_FUNCTION_EXISTS ("inet_ntop" HAVE_INET_NTOP_H)
