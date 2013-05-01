cmake_minimum_required(VERSION 2.8)

# Append local cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

#  about TimeIT
# ---------------
project(timeit)

add_custom_target(version ALL
                  COMMAND ./git_version_gen.sh .                  
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
file (STRINGS "version" PACKAGE_VERSION)


#  dependencies
# ---------------
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(SQLITE sqlite3)
pkg_check_modules(XSCRNS xscrnsaver)

# make dist target
# -------------------
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION})
set(ARCHIVE_NAME_P ${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION}/)
add_custom_target(dist
    COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD > ${ARCHIVE_NAME}.tar
    COMMAND ln -s . ${ARCHIVE_NAME}
		COMMAND tar  --append -f ${ARCHIVE_NAME}.tar ${ARCHIVE_NAME}/version ${ARCHIVE_NAME}/src/version.h
    COMMAND gzip -c ${ARCHIVE_NAME}.tar > ${ARCHIVE_NAME}.tar.gz 
    COMMAND rm ${ARCHIVE_NAME}.tar
    COMMAND rm ${ARCHIVE_NAME}
    WORKING_DIRECTORY ${CMAKE_ROOT_DIR}
    )

# make DEB & RPM package
#--------------------
set(CMAKE_INSTALL_PREFIX "/usr" )
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr" )
set(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
SET(CPACK_GENERATOR "DEB;RPM")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kent Asplund")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtkmm-2.4-1c2a (>=2.8), libsqlite3-0, libxss1")
set(CPACK_RPM_PACKAGE_REQUIRES "libgtkmm-2.4-1c2a (>=2.8), libsqlite3-0, libxss1")
set(CPACK_PACKAGE_DESCRIPTION "TimeIT is a time tracker somewhat similar to ktimetracker and gnotime written in C++ using gtkmm. It aims to be unobtrusive and simple to use.  It has Workspace tracking, Idle detection, hierarchical tasks and can show summaries over day/week/month")
set(CPACK_PACKAGE_CONTACT "Kent Asplund hoglet@solit.se")
INCLUDE(CPack)

#  Subfolders
# ---------------
add_subdirectory(src)
add_subdirectory(images)
add_subdirectory(doc)
add_subdirectory(po)
add_subdirectory(misc)

