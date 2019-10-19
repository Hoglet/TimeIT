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

	
	set(DATADIR "${CMAKE_INSTALL_PREFIX}/share")
	set(PROGRAMNAME_DATADIR "\"${DATADIR}/${PROJECT_NAME}\"")
	set(PROGRAMNAME_LOCALEDIR "\"${DATADIR}/locale\"")
	set(GETTEXT_PACKAGE "\"${PROJECT_NAME}\"")

	add_definitions( -DPROGRAMNAME_DATADIR=${PROGRAMNAME_DATADIR})
	add_definitions( -DPACKAGE_DATA_DIR=\"${DATADIR}\" )
	add_definitions( -DGETTEXT_PACKAGE=${GETTEXT_PACKAGE})
	add_definitions( -DPROGRAMNAME_LOCALEDIR=${PROGRAMNAME_LOCALEDIR})


	set( tested_headers 
		../src/DB/Task.h
	    ../src/Logic/UUID.h 
	    ../src/Misc/Utils.h 
	   )
	set( tested_sources
	    ../src/DB/Task.cpp 
	    ../src/Logic/UUID.cpp 
	    ../src/Misc/Utils.cpp 
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
	                "${CMAKE_CURRENT_SOURCE_DIR}/../src/Logic/"
	                "${CMAKE_CURRENT_SOURCE_DIR}/../src/Misc/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../src/DB/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../src/DB/DBAbstraction/"
                    )
	message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/../src/DB/")
  
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
					)
endif()
