cmake_minimum_required(VERSION 2.8)

# Append local cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

#  about TimeIT
# ---------------
project(timeit)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")


#  dependencies
# ---------------
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(SQLITE sqlite3)
pkg_check_modules(XSCRNS xscrnsaver)

# make dist target
# --------------------
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${VERSION})
add_custom_target(dist
    COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
        | gzip > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.gz
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


#  Subfolders
# ---------------
add_subdirectory(src)
add_subdirectory(images)
add_subdirectory(doc)
add_subdirectory(po)
add_subdirectory(misc)

