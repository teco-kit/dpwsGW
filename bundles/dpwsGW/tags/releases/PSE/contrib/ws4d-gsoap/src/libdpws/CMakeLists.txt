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

INCLUDE_DIRECTORIES(AFTER ${DPWS_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})

SET(WS4D_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(ws4d_listF STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ws4d_eprflist.c)

ADD_LIBRARY(ws4d_listFmt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ws4d_eprflist.c)

EXTEND_TGT_COMPILE_FLAGS(ws4d_listFmt FLAGS "-DWITH_MUTEXES")

ADD_LIBRARY(ws4d_listL STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ws4d_eprllist.c)

ADD_LIBRARY(ws4d_listLmt STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ws4d_eprllist.c)

EXTEND_TGT_COMPILE_FLAGS(ws4d_listLmt FLAGS "-DWITH_MUTEXES")

INSTALL(TARGETS ws4d_listF ws4d_listFmt ws4d_listL ws4d_listLmt
	ARCHIVE DESTINATION lib/ws4d-gSOAP)

SET(DPWS_LIST_LIBRARY
		ws4d_listL
		CACHE STRING ""
		FORCE)

SET(DPWS_LISTMT_LIBRARY
		ws4d_listLmt
		CACHE STRING ""
		FORCE)

ADD_SUBDIRECTORY(gsoap)

MARK_AS_ADVANCED(
	DPWS_LIST_LIBRARY
	DPWS_LISTMT_LIBRARY)