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
# create source tarball                                                        #
################################################################################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "WS4D-gSOAP DPWS toolkit source tarball")
set(CPACK_PACKAGE_VENDOR "WS4D-gSOAP development team")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README)
set(
CPACK_SOURCE_PACKAGE_FILE_NAME
"ws4d-gsoap-${VERSION}.src"
CACHE INTERNAL "tarball basename"
)
set(CPACK_SOURCE_GENERATOR TGZ TBZ2 ZIP)

set(CPACK_SOURCE_IGNORE_FILES
"~$"
".git"
".project"
".cdtproject"
".settings"
".cproject"
"Build-*"
"Install-*"
"\\\\.cvsignore$"
"^${PROJECT_SOURCE_DIR}.*/CVS/"
"^${PROJECT_SOURCE_DIR}/debian/"
"^${PROJECT_SOURCE_DIR}/old/"
)
include(CPack)
