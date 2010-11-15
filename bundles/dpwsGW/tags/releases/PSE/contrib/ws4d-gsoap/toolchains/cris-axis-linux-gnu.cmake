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

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)

#-mlinux -mno-mul-bug-workaround
# specify the cross compiler
SET(CRIS_CROSS_COMPILER_PATH /usr/local/cris)
SET(C_CROSS_COMPILER cris-)

SET(CMAKE_C_COMPILER "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}gcc" "-mlinux -mno-mul-bug-workaround")
SET(CMAKE_CXX_COMPILER "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}c++" "-mlinux -mno-mul-bug-workaround")
SET(CMAKE_RANLIB "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}ranlib")
SET(CMAKE_AR "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}ar")

SET(CMAKE_COMPILER_IS_GNUCC 1)
SET(CMAKE_COMPILER_IS_GNUCXX 1)

SET(CMAKE_SKIP_RPATH ON)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

IF("$ENV{DEVBOARD_PATH}" STREQUAL "")
    MESSAGE(SEND_ERROR "Could not find devboard, please declare DEVBOARD_PATH environment variable")
ELSE("$ENV{DEVBOARD_PATH}" STREQUAL "")
    SET(DEVBOARD_PATH "$ENV{DEVBOARD_PATH}")
ENDIF("$ENV{DEVBOARD_PATH}" STREQUAL "")

INCLUDE_DIRECTORIES(BEFORE SYSTEM ${DEVBOARD_PATH}/target/cris-axis-linux-gnu/include)
LINK_DIRECTORIES(${DEVBOARD_PATH}/target/cris-axis-linux-gnu/lib)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH ${DEVBOARD_PATH}/target/cris-axis-linux-gnu)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

