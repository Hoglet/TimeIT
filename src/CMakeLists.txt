cmake_minimum_required(VERSION 2.8)

# Append local cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )

#  about TimeIT
# ---------------
project(timeit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CONFIGURED)
	execute_process( COMMAND ${CMAKE_CURRENT_LIST_DIR}/../scripts/git_version_gen.sh ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/lib/inc/libtimeit/version.h)
	set(CONFIGURED TRUE CACHE BOOL "Is configured" FORCE)
endif()

message ( **************** )
message ( "COMMAND  ${CMAKE_CURRENT_LIST_DIR}/../scripts/git_version_gen.sh ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/lib/inc/libtimeit/version.h ")
message ( **************** )

add_custom_target(version ALL
                  COMMAND  ${CMAKE_CURRENT_LIST_DIR}/../scripts/git_version_gen.sh ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/lib/inc/libtimeit/version.h
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
file (STRINGS "version" PACKAGE_VERSION)

unset(VERSION_FILE CACHE)
find_file(VERSION_FILE NAME "version" PATHS ${CMAKE_CURRENT_SOURCE_DIR})

IF (NOT "${VERSION_FILE}" STREQUAL VERSION_FILE-NOTFOUND)
  file (STRINGS ${VERSION_FILE} PACKAGE_VERSION)
ELSE()
  SET(PACKAGE_VERSION "1.Unknown ")
ENDIF()

#  dependencies
# ---------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-2.4 REQUIRED)
pkg_check_modules(SQLITE sqlite3 REQUIRED)
pkg_check_modules(XSCRNS xscrnsaver REQUIRED)
pkg_check_modules(XLIB x11 REQUIRED)
pkg_check_modules(JANSSON jansson REQUIRED)
pkg_check_modules(CURL libcurl REQUIRED)
pkg_check_modules(NOTIFY libnotify REQUIRED)


# make dist target
# -------------------
add_custom_target(dist
    COMMAND   ${CMAKE_SOURCE_DIR}/makedist.sh ${CMAKE_PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS version
    )

# make DEB & RPM package
#--------------------
set(CMAKE_INSTALL_PREFIX "/usr" )
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr" )
set(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
SET(CPACK_GENERATOR "RPM")
set(CPACK_RPM_PACKAGE_REQUIRES "libgtkmm-2.4-1v5, libsqlite3-0, libxss1, libnotify-bin, libjansson4, libcurl")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The unobtrusive time tracker.")
set(CPACK_PACKAGE_DESCRIPTION "TimeIT is a time tracker somewhat similar to ktimetracker and gnotime written in C++ using gtkmm. It aims to be unobtrusive and simple to use.  It has Workspace tracking, Idle detection, hierarchical tasks and can show summaries over day/week/month")
set(CPACK_PACKAGE_CONTACT "Kent Asplund hoglet@solit.se")
set(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
INCLUDE(CPack)



set(DATADIR "${CMAKE_INSTALL_PREFIX}/share")
set(PROGRAMNAME_DATADIR "\"${DATADIR}/${PROJECT_NAME}\"")

add_definitions( -DPROGRAMNAME_DATADIR=${PROGRAMNAME_DATADIR})


#  Subfolders
# ---------------
add_subdirectory(lib)
add_subdirectory(timeit_gtkmm)
add_subdirectory(images)
add_subdirectory(doc)
add_subdirectory(po)
add_subdirectory(misc)
add_subdirectory(tests)

#  make ctags
# --------------
#set_source_files_properties(tags PROPERTIES GENERATED true)
#add_custom_target(tags
#    COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q timeit_gtkmm
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
#find_program(CTAGS_PATH ctags)
#if(CTAGS_PATH)
#	add_dependencies(timeit_gtkmm tags)
#endif(CTAGS_PATH)

install(TARGETS timeit
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static)
