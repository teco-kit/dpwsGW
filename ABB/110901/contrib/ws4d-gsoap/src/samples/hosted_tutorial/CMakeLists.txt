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

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in
               ${CMAKE_CURRENT_BINARY_DIR}/files/CMakeLists.txt
               @ONLY IMMEDIATE)

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/files/ DESTINATION share/doc/ws4d-gSOAP/hosted-example
	PATTERN ".svn" EXCLUDE)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/files/CMakeLists.txt DESTINATION share/doc/ws4d-gSOAP/hosted-example)

ENDIF(NOT CMAKE_CROSSCOMPILING)
