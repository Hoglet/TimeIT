#.rst:
# FindXScreensaver
# -----------
#
# Find XScreensaver
#
# Find XScreensaver headers and libraries.
#
# ::
#
#   XSCREENSAVER_FOUND          - True if XScreensaver found.
#   XSCREENSAVER_INCLUDE_DIRS   - Where to find jansson.h.
#   XSCREENSAVER_LIBRARIES      - List of libraries when using XScreensaver.
#   XSCREENSAVER_VERSION_STRING - The version of XScreensaver found.

#=============================================================================
# Modified from FindJansson.cmake by Silvio Clecio
# Copyright 2018 Silvio Clecio <silvioprog@gmail.com>
# Copyright 2023 Kent Asplund <
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation;
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU GENERAL PUBLIC LICENSE for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library.	If not, see <http://www.gnu.org/licenses/>.
#=============================================================================

find_package(PkgConfig QUIET)
pkg_check_modules(PC_XSCRNS QUIET xscrnsaver )



find_path(XSCREENSAVER_INCLUDE_DIR
        NAMES X11/extensions/scrnsaver.h
        HINTS ${PC_XSCRNS_INCLUDE_DIR} ${PC_XSCRNS_INCLUDE_DIRS})

find_library(XSCREENSAVER_LIBRARY
        NAMES Xss libXss
        HINTS ${PC_XSCRNS_LIBDIR} ${PC_XSCRNS_LIBRARY_DIRS})


if (PC_XSCRNS_VERSION)
    set(XSCREENSAVER_VERSION_STRING ${PC_XSCRNS_VERSION})
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XScreensaver
        REQUIRED_VARS XSCREENSAVER_LIBRARY XSCREENSAVER_INCLUDE_DIR
        VERSION_VAR XSCREENSAVER_VERSION_STRING)



if (XSCREENSAVER_FOUND)
    message(STATUS "Found XScreensaver: ${XSCREENSAVER_LIBRARY}")
    set(XSCREENSAVER_LIBRARIES ${XSCREENSAVER_LIBRARY})
    set(XSCREENSAVER_INCLUDE_DIRS ${XSCREENSAVER_INCLUDE_DIR})
endif ()

mark_as_advanced(XSCREENSAVER_INCLUDE_DIR XSCREENSAVER_LIBRARY)
