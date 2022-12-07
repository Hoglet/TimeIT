cmake_minimum_required(VERSION 3.14)

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
find_package(Gettext          REQUIRED)
find_package(fmt              REQUIRED)


# make dist target
# -------------------
add_custom_target(dist
    COMMAND   ${CMAKE_SOURCE_DIR}/makedist.sh ${CMAKE_PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS version
    )

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
