cmake_minimum_required(VERSION 3.13)

# Setup vcpkg script with CMake (note: should be placed before project() call)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
list(APPEND VCPKG_FEATURE_FLAGS "versions")

#  about TimeIT
# ---------------
project(timeit)


#  Subfolders
# ---------------
add_subdirectory(src)
