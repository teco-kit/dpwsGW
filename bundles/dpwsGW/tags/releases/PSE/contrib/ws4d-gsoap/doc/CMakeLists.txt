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

IF (WITH_DOCU)

IF (NOT DOXYGEN_EXECUTABLE)
MESSAGE(STATUS "Don't build documentation because doxygen is missing")
ELSE (NOT DOXYGEN_EXECUTABLE)
MESSAGE(STATUS "Will build documentation")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/dpws_doxyfile.in
	${CMAKE_CURRENT_BINARY_DIR}/dpws_doxyfile
	@ONLY)

FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html)

SET(dpws_doc_DEPS
	${CMAKE_CURRENT_SOURCE_DIR}/dpws_mainpage 
	${DPWS_INCLUDE_DIR}/stddpws.h
	${DPWS_INCLUDE_DIR}/ws4d_uri.h
	${DPWS_INCLUDE_DIR}/ws4d_epr.h
	${DPWS_INCLUDE_DIR}/ws4d_device_description.h
	${DPWS_INCLUDE_DIR}/ws4d_localizedstring.h
	${DPWS_INCLUDE_DIR}/ws4d_abstract_eprlist.h
	${DPWS_INCLUDE_DIR}/ws-addressing.h
	${DPWS_INCLUDE_DIR}/ws4d_discovery_ctx.h
	${DPWS_INCLUDE_DIR}/ws-discovery.h
	${DPWS_INCLUDE_DIR}/ws4d_eprflist.h
	${DPWS_INCLUDE_DIR}/ws-eventing.h
	${DPWS_INCLUDE_DIR}/ws4d_eprllist.h
	${DPWS_INCLUDE_DIR}/ws-metadataexchange.h
	${DPWS_INCLUDE_DIR}/ws4d_servicecache.h
	${DPWS_INCLUDE_DIR}/ws4d_alloclist.h
	${DPWS_INCLUDE_DIR}/ws4d_list.h
	${DPWS_INCLUDE_DIR}/ws4d_misc.h
	${DPWS_INCLUDE_DIR}/ws4d_mutex.h
	${DPWS_INCLUDE_DIR}/ws4d_qname.h
	${DPWS_INCLUDE_DIR}/ws4d_stringarray.h
	${DPWS_INCLUDE_DIR}/ws4d_time.h
	${DPWS_INCLUDE_DIR}/ws4d_uuid.h
	${DPWS_INCLUDE_DIR}/soap_misc.h
	${DPWS_INCLUDE_DIR}/ws4d_target.h
	${DPWS_INCLUDE_DIR}/ws4d_service.h
	${DPWS_INCLUDE_DIR}/ws4d_hostingservice.h
	${DPWS_INCLUDE_DIR}/ws4d_targetservice.h
	${DPWS_INCLUDE_DIR}/ws4d_targetcache.h
	${DPWS_INCLUDE_DIR}/ws4d_epr_kv.h
	${DPWS_INCLUDE_DIR}/ws4d_subscription.h
	${DPWS_INCLUDE_DIR}/ws4d_subsmanager.h
	${DPWS_INCLUDE_DIR}/ws4d_subsmanproxy.h)

SET(dpws_doc_FILES
		${CMAKE_CURRENT_BINARY_DIR}/dpws.tags
		${CMAKE_CURRENT_BINARY_DIR}/html/index.html
		${CMAKE_CURRENT_BINARY_DIR}/latex/refman.tex)

ADD_CUSTOM_COMMAND(OUTPUT ${dpws_doc_FILES}
	COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/dpws_doxyfile
	DEPENDS ${dpws_doc_DEPS})

ADD_CUSTOM_TARGET(doc ALL
	DEPENDS ${dpws_doc_FILES})

ADD_DEPENDENCIES(doc gen_headers gen_sources)

INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc/ws4d-gSOAP/)
	
IF(WITH_PDF_DOCU)

IF(NOT PDFLATEX_COMPILER OR NOT MAKEINDEX_COMPILER)
MESSAGE(STATUS "Don't build pdf documentation because pdflatex or makeindex is missing")
ELSE(NOT PDFLATEX_COMPILER OR NOT MAKEINDEX_COMPILER)
MESSAGE(STATUS "Will build pdf documentation")

SET(dpws_refman
    ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf)

ADD_CUSTOM_COMMAND(OUTPUT ${dpws_refman}
    COMMAND make -C latex refman.pdf
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.tex)

ADD_CUSTOM_TARGET(gen_pdfs ALL DEPENDS ${dpws_refman})

ADD_DEPENDENCIES(gen_pdfs gen_headers)

INSTALL(FILES ${dpws_refman} DESTINATION share/doc/ws4d-gSOAP/)

ENDIF(NOT PDFLATEX_COMPILER OR NOT MAKEINDEX_COMPILER)                 
ENDIF(WITH_PDF_DOCU)

ENDIF (NOT DOXYGEN_EXECUTABLE)
ENDIF (WITH_DOCU)

ENDIF(NOT CMAKE_CROSSCOMPILING)
