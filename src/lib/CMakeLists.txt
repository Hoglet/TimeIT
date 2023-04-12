
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )

find_package(GTest)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads             REQUIRED)
find_package(CURL                REQUIRED)
find_package(SQLite3             REQUIRED)
find_package(Jansson             REQUIRED)


find_package(X11                 REQUIRED)
find_package(XScreensaver        REQUIRED)

if( XSCREENSAVER_FOUND )
	add_definitions(-DXSCREENSAVER_FOUND)
endif()

if( GTEST_FOUND AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -pg -O0 -coverage -fprofile-arcs -ftest-coverage ")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} -coverage -lgcov")
endif()

file( GLOB_RECURSE headers *.h)
file( GLOB_RECURSE sources *.cpp)




add_library( libtimeit STATIC
		${sources}
		${headers}
		)

target_link_libraries( libtimeit
		${SQLite3_LIBRARIES}
		${XSCREENSAVER_LIBRARIES}
		${X11_LIBRARIES}
		${JANSSON_LIBRARIES}
		${CURL_LIBRARIES}
		fmt::fmt
		"${CMAKE_THREAD_LIBS_INIT}"
		)

target_include_directories ( libtimeit PUBLIC
		inc
		${X11_INCLUDE_DIR}
		${SQLite3_INCLUDE_DIRS}
		${CURL_INCLUDE_DIRS}
  )

if(MSVC)
	target_compile_options(libtimeit PRIVATE /W4 /WX)
else()
	target_compile_options(libtimeit PRIVATE -Wall -Wextra -pedantic)
endif()
