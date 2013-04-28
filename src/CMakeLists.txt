project(timeit)

ENABLE_TESTING()
set(DATADIR "${CMAKE_INSTALL_PREFIX}/share")

set(PROGRAMNAME_DATADIR "\"${DATADIR}/${PROJECT_NAME}\"")
set(PROGRAMNAME_LOCALEDIR "\"${DATADIR}/locale\"")
set(GETTEXT_PACKAGE "\"${PROJECT_NAME}\"")

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")

add_definitions( -DPROGRAMNAME_DATADIR=${PROGRAMNAME_DATADIR})
add_definitions( -DPACKAGE_VERSION=\"${VERSION}\" )
add_definitions( -DPACKAGE_DATA_DIR=\"${DATADIR}\" )
add_definitions( -DGETTEXT_PACKAGE=${GETTEXT_PACKAGE})
add_definitions( -DPROGRAMNAME_LOCALEDIR=${PROGRAMNAME_LOCALEDIR})

#  Source files
# --------------
file( GLOB_RECURSE headers *.h)
file( GLOB_RECURSE sources *.cpp)

link_directories(${GTKMM_LIBRARY_DIRS}
                 ${SQLITE_LIBRARY_DIRS}
								 ${XSCRNS_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS}
                    ${SQLITE_INCLUDE_DIRS}
                    ${XSCRNS_INCLUDE_DIRS}
                    "${PROJECT_SOURCE_DIR}/DB/"
                    "${PROJECT_SOURCE_DIR}/Logic/"
                    "${PROJECT_SOURCE_DIR}/GUI/"
                    "${PROJECT_SOURCE_DIR}/Misc/"
										"${PROJECT_SOURCE_DIR}/cute_lib/"
  									"${PROJECT_SOURCE_DIR}/Tests/"
                    "${PROJECT_SOURCE_DIR}/DB/DBAbstraction/"
										"${PROJECT_SOURCE_DIR}/../cute_lib/")

add_executable(${PROJECT_NAME} ${sources} ${headers} )

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES} ${SQLITE_LIBRARIES} ${XSCRNS_LIBRARIES} )


install(TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static)