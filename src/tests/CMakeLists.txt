cmake_minimum_required(VERSION 3.14)

find_package(GTest)

if( GTEST_FOUND )
	set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
	
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -pg -O0 -coverage -fprofile-arcs -ftest-coverage ")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} -coverage -lgcov")

	file( GLOB_RECURSE headers src/*.h)
	file( GLOB_RECURSE sources src/*.cpp)


	include_directories(${GTEST_INCLUDE_DIRS}
					"${CMAKE_CURRENT_LIST_DIR}/../lib/inc/"
                    )
  

#	if(CMAKE_COMPILER_IS_GNUCXX)
#    	include(CodeCoverage)
#    	setup_target_for_coverage_gcovr_html(
#    		NAME tests_coverage
#    		EXECUTABLE tests)
#	endif()
  
	add_executable(tests ${sources} ${headers}  )
  
	target_link_libraries(
			tests
			${GTEST_LIBRARIES}
			${GTEST_MAIN_LIBRARIES}
			libtimeit
			"${CMAKE_THREAD_LIBS_INIT}"
			gcov
			)

	if(MSVC)
		target_compile_options(tests PRIVATE /W4 /WX)
	else()
		target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
	endif()

endif()
