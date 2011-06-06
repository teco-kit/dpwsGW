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

SET(CMAKE_SYSTEM_NAME Linux)

SET(CMAKE_C_COMPILER "gcc" "-m64")
SET(CMAKE_CXX_COMPILER "c++" "-m64")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m64")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m64")

SET(CMAKE_COMPILER_IS_GNUCC 1)
SET(CMAKE_COMPILER_IS_GNUCXX 1)

SET(CMAKE_SKIP_RPATH ON)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
