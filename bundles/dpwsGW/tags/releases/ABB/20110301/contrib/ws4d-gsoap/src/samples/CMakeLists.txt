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

IF("${ALT_IO_MODE}" STREQUAL "none")
  INCLUDE_DIRECTORIES(AFTER ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES})
ELSE("${ALT_IO_MODE}" STREQUAL "none")
  ADD_DEFINITIONS ( "-DWITH_NOIO -DWITH_SOAPDEFS_H" )
  INCLUDE_DIRECTORIES(AFTER ${DPWS_ALT_IO_INCLUDES} ${GSOAP_INCLUDE_DIR} ${DPWS_INCLUDES})
ENDIF("${ALT_IO_MODE}" STREQUAL "none")

ADD_SUBDIRECTORY(ms-iop)
ADD_SUBDIRECTORY(tutorial)
ADD_SUBDIRECTORY(peer_tutorial)

IF (WITH_HOSTING_SERVICE)
        ADD_SUBDIRECTORY(hosted_tutorial)
ENDIF(WITH_HOSTING_SERVICE)

