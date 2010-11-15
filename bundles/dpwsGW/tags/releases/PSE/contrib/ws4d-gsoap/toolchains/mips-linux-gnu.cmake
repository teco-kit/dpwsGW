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
SET(CRIS_CROSS_COMPILER_PATH /home/lordi/eldk-mips/usr/bin)
SET(C_CROSS_COMPILER mips_4KCle-)

SET(CMAKE_C_COMPILER "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}gcc")
SET(CMAKE_CXX_COMPILER "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}c++")
SET(CMAKE_RANLIB "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}ranlib")
SET(CMAKE_AR "${CRIS_CROSS_COMPILER_PATH}/${C_CROSS_COMPILER}ar")

SET(CMAKE_COMPILER_IS_GNUCC 1)
SET(CMAKE_COMPILER_IS_GNUCXX 1)

#SET(CMAKE_SKIP_RPATH ON)

#SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

#noch keine Compiler-Flags gesetz ... standard debug

#SET(CMAKE_C_FLAGS )
#SET(CMAKE_CXX_FLAGS )
#SET(CMAKE_C_FLAGS_DEBUG -g)
#SET(CMAKE_CXX_FLAGS_DEBUG -g)
#SET(CMAKE_C_FLAGS_RELEASE )
#SET(CMAKE_CXX_FLAGS_RELEASE )
#SET(CMAKE_C_FLAGS_RELWITHDEBINFO -g)
#SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO -g)
#SET(CMAKE_C_FLAGS_MINSIZEREL )
#SET(CMAKE_CXX_FLAGS_MINSIZEREL )

INCLUDE_DIRECTORIES(BEFORE SYSTEM /home/lordi/eldk-mips/mips_4KCle/usr/include)
LINK_DIRECTORIES(/home/lordi/eldk-mips/mips_4KCle/usr/lib)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH /home/lordi/eldk-mips/usr/bin /home/lordi/eldk-mips/mips_4KCle)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

