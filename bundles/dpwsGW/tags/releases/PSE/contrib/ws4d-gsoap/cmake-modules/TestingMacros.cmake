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

SET(BUILDNAME "${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}")

MACRO( BUILD_CHECK_TEST _testName _testSource ) 

	if(COMMAND cmake_policy)
		cmake_policy(SET CMP0003 NEW)
		## The macro ADD_CHECK_TEST creates false warning.
		cmake_policy(SET CMP0002 OLD)
	endif(COMMAND cmake_policy)
	ADD_EXECUTABLE( ${_testName} ${_testSource} )
	TARGET_LINK_LIBRARIES( ${_testName} ${ARGN} )

ENDMACRO( BUILD_CHECK_TEST )

MACRO( WS4D_TESTCASE _testSource _testDest ) 

	if(COMMAND cmake_policy)
		cmake_policy(SET CMP0003 NEW)
		## The macro ADD_CHECK_TEST creates false warning.
		cmake_policy(SET CMP0002 OLD)
	endif(COMMAND cmake_policy)

  GET_TARGET_PROPERTY(_test_LOCATION ${_testSource} LOCATION)

	# 1st Argument is the testcase name
	# 2nd Argument is the executable name
  # 3rd Argument the testcase name, again, as argument for the executable
	ADD_TEST( ${_testDest} ${_test_LOCATION} ${_testDest} )

ENDMACRO( WS4D_TESTCASE )

