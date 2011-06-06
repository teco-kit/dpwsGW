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

# Find DPWS host executables

SET(_HOSTEXECUTABLE_SEARCH_PATHS "")

# if set append DPWS_HOSTEXEC_PATH to path list
IF(DPWS_HOSTEXEC_PATH)
	LIST(APPEND _HOSTEXECUTABLE_SEARCH_PATHS
		${DPWS_HOSTEXEC_PATH})
ENDIF(DPWS_HOSTEXEC_PATH)

# if set append DPWS_HOST_EXECUTABLES environment to path list
IF(NOT "$ENV{DPWS_HOSTEXEC_PATH}" STREQUAL "")
    LIST(APPEND _HOSTEXECUTABLE_SEARCH_PATHS
        "$ENV{DPWS_HOSTEXEC_PATH}")
ENDIF(NOT "$ENV{DPWS_HOSTEXEC_PATH}" STREQUAL "")

# append standard gsoap location directories
LIST(APPEND _HOSTEXECUTABLE_SEARCH_PATHS
	/usr
	/usr/local)

SET(DPWS_HOSTEXEC "DPWS_HOSTEXEC-NOTFOUND" CACHE PATH "ws4d host executables" FORCE)

SET(DPWS_HOSTEXEC_FOUND FALSE)
FOREACH(_HOST_EXECUTABLE_LOCATION ${_HOSTEXECUTABLE_SEARCH_PATHS})
    IF(NOT DPWS_HOSTEXEC_FOUND)
    
    FIND_FILE(DPWS_HOSTEXEC
        NAMES HostExecutables.cmake
        PATHS ${_HOST_EXECUTABLE_LOCATION}/share/ws4d-gSOAP
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_CMAKE_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH)

    IF(DPWS_HOSTEXEC)
        SET(DPWS_HOSTEXEC_FOUND TRUE)
        SET(DPWS_HOSTEXEC_PATH_FOUND ${_HOST_EXECUTABLE_LOCATION})
    ENDIF(DPWS_HOSTEXEC)

    ENDIF(NOT DPWS_HOSTEXEC_FOUND)
ENDFOREACH(_HOST_EXECUTABLE_LOCATION)

# Finally deal with the result of our search :-) 
IF(DPWS_HOSTEXEC_FOUND)
    INCLUDE(${DPWS_HOSTEXEC})
    IF ( NOT DPWSHostExecutables_FIND_QUIETLY )
        MESSAGE(STATUS "Found and included DPWS host executables in ${DPWS_HOSTEXEC_PATH_FOUND}")
    ENDIF ( NOT DPWSHostExecutables_FIND_QUIETLY )
ELSE(DPWS_HOSTEXEC_FOUND)
    IF(DPWSHostExecutables_FIND_REQUIRED)
        MESSAGE(SEND_ERROR "Could not find DPWS host executables in ${_HOSTEXECUTABLE_SEARCH_PATHS}")
    ELSE(DPWSHostExecutables_FIND_REQUIRED)
        IF ( NOT DPWSHostExecutables_FIND_QUIETLY )
            MESSAGE(STATUS "Could not find DPWS host executables in ${_HOSTEXECUTABLE_SEARCH_PATHS}")
        ENDIF ( NOT DPWSHostExecutables_FIND_QUIETLY )
    ENDIF(DPWSHostExecutables_FIND_REQUIRED)
ENDIF(DPWS_HOSTEXEC_FOUND)

MARK_AS_ADVANCED(
    DPWS_HOSTEXEC
)