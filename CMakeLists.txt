cmake_minimum_required(VERSION 3.13)

#  about TimeIT
# ---------------
project(timeit)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--config-file=${PROJECT_SOURCE_DIR}/.clang-tidy")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



#  Subfolders
# ---------------
add_subdirectory(src)
