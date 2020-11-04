cmake_minimum_required(VERSION 2.6)

find_package(GTest)

if( GTEST_FOUND )
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTKMM gtkmm-2.4 REQUIRED)
	pkg_check_modules(SQLITE sqlite3 REQUIRED)
	pkg_check_modules(XSCRNS xscrnsaver REQUIRED)
	pkg_check_modules(XLIB x11 REQUIRED)
	pkg_check_modules(JANSSON jansson REQUIRED)
	pkg_check_modules(CURL libcurl REQUIRED)
	pkg_check_modules(NOTIFY libnotify REQUIRED)

	
	set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
	set(DATADIR "${CMAKE_INSTALL_PREFIX}/share")
	set(PROGRAMNAME_DATADIR "\"${DATADIR}/${PROJECT_NAME}\"")
	set(PROGRAMNAME_LOCALEDIR "\"${DATADIR}/locale\"")
	set(GETTEXT_PACKAGE "\"${PROJECT_NAME}\"")
	
	set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -pg -O0 ")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} ")

	add_definitions( -DPROGRAMNAME_DATADIR=${PROGRAMNAME_DATADIR})
	add_definitions( -DPACKAGE_DATA_DIR=\"${DATADIR}\" )
	add_definitions( -DGETTEXT_PACKAGE=${GETTEXT_PACKAGE})
	add_definitions( -DPROGRAMNAME_LOCALEDIR=${PROGRAMNAME_LOCALEDIR})

	file( GLOB_RECURSE tested_headers 
		${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/DB/*.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/Logic/*.h 
	    ${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/Misc/*.h 
	    ${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/GUI/*.h 
	   )
	file( GLOB_RECURSE tested_sources
	    ${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/DB/*.cpp 
	    ${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/Logic/*.cpp 
	    ${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/Misc/*.cpp 
	    ${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/GUI/*.cpp 
	   )
	
	file( GLOB_RECURSE headers src/*.h)
	file( GLOB_RECURSE sources src/*.cpp)


	include_directories(${GTEST_INCLUDE_DIRS}
	                ${GTKMM_INCLUDE_DIRS}
	                ${SQLITE_INCLUDE_DIRS}
	                ${XSCRNS_INCLUDE_DIRS}
	                ${XLIB_INCLUDE_DIRS}
	                ${JANSSON_INCLUDE_DIRS}
	                ${CURL_INCLUDE_DIRS}
	                ${NOTIFY_INCLUDE_DIRS}
	                "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/Logic/"
	                "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/Misc/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/DB/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/DB/DBAbstraction/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/GUI/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/Sync/"
                    )
	message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/../timeit_gtkmm/DB/")
  

#	if(CMAKE_COMPILER_IS_GNUCXX)
#    	include(CodeCoverage)
#    	setup_target_for_coverage_gcovr_html(
#    		NAME tests_coverage
#    		EXECUTABLE tests)
#	endif()
  
	add_executable(tests ${sources} ${headers} ${tested_headers}  ${tested_sources})
  
  
  
	target_link_libraries(tests
                      ${GTKMM_LIBRARIES}
                      ${SQLITE_LIBRARIES}
                      ${XSCRNS_LIBRARIES}
                      ${XLIB_LIBRARIES}
                      ${JANSSON_LIBRARIES}
                      ${CURL_LIBRARIES}
                      ${NOTIFY_LIBRARIES}	 
                      ${GTEST_LIBRARIES} 
                      ${GTEST_MAIN_LIBRARIES}
                      pthread
                      gcov
					)
endif()
