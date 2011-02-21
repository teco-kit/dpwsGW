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

################################################################################
# Indent target to indent all source and header files                          #
################################################################################

FILE(GLOB_RECURSE INC_INDENT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.[ch]")
FILE(GLOB_RECURSE SRC_INDENT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch]")

ADD_CUSTOM_TARGET(indent)

FOREACH(file ${INC_INDENT_FILES})
      ADD_CUSTOM_COMMAND(TARGET indent
                         PRE_BUILD
                         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/ws4d_indent.sh ${file}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
                         COMMENT "Indenting ${file}"
                         )
ENDFOREACH(file ${INC_INDENT_FILES})

FOREACH(file ${SRC_INDENT_FILES})
      ADD_CUSTOM_COMMAND(TARGET indent
                         PRE_BUILD
                         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/ws4d_indent.sh ${file}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
                         COMMENT "Indenting ${file}"
                         )
ENDFOREACH(file ${SRC_INDENT_FILES})
