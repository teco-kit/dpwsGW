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

INCLUDE_DIRECTORIES(AFTER ${IOP_DEVICE_LIB_INCLUDES})

SET(IOP_DEVICE_TEST_LIBS 
	${IOP_DEVICE_LIB}
	${DPWS_LIBRARIES}
	${DPWS-D_LIBRARIES}
	${TEST_TARGET_LIBRARIES})

BUILD_CHECK_TEST(device_test device_test.c ${IOP_DEVICE_TEST_LIBS} )
EXTEND_TGT_COMPILE_FLAGS(device_test FLAGS "-DDPWS_DEVICE")

WS4D_TESTCASE(device_test device_create_destroy)
WS4D_TESTCASE(device_test device_create_destroy_two_times)
WS4D_TESTCASE(device_test device_without_wsdl)