
find_package(GTest)

if( GTEST_FOUND )
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -pg -O0 -coverage -fprofile-arcs -ftest-coverage ")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} -coverage -lgcov")
endif()

file( GLOB_RECURSE headers *.h)
file( GLOB_RECURSE sources *.cpp)




add_library( libtimeit STATIC
		${sources}
		${headers}
		inc/libtimeit/internal/gettext.h inc/libtimeit/EventObserver.cpp)

target_link_libraries( libtimeit
        ${SQLITE_LIBRARIES}
        ${XSCRNS_LIBRARIES}
        ${XLIB_LIBRARIES}
        ${JANSSON_LIBRARIES}
        ${CURL_LIBRARIES}
		pthread
        )

target_include_directories ( libtimeit PUBLIC
  inc
  )


