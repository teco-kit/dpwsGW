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

# Find DPWS-Device includes and library.
#
# This module defines
#  DPWS_INCLUDE_DIR
#  DPWS_LIBRARIES, the libraries needed for devices and clients.
#  DPWS_LIB_DIR, the location of the common libraries.
#  DPWS-D_LIBRARIES, the libraries to link against to use the device part of DPWS.
#  DPWS-D_LIB_DIR, the location of the device specific libraries.
#  DPWS-D_FOUND, If false, do not try to use DPWS
#
# Note: The Option DPWS_PATH should be set in order to find the dpws base 
#       directory.
#
# Author: S. Feldhorst <sascha.feldhorst@uni-dortmund.de>

# First search the common DPWS components.
FIND_PACKAGE(DPWS REQUIRED device)

