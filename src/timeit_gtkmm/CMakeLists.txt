project(timeit)

# Append local cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )

ENABLE_TESTING()
set(DATADIR "${CMAKE_INSTALL_PREFIX}/share")
set(PROGRAMNAME_LOCALEDIR "\"${DATADIR}/locale\"")
set(GETTEXT_PACKAGE "\"${PROJECT_NAME}\"")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -pg -O0 -fprofile-arcs --coverage -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs --coverage -ftest-coverage")

add_definitions( -DPROGRAMNAME_DATADIR=${PROGRAMNAME_DATADIR})
add_definitions( -DPACKAGE_DATA_DIR=\"${DATADIR}\" )
add_definitions( -DGETTEXT_PACKAGE=${GETTEXT_PACKAGE})
add_definitions( -DPROGRAMNAME_LOCALEDIR=${PROGRAMNAME_LOCALEDIR})
add_definitions( -DIMAGE_SRC_DIR=\"${CMAKE_CURRENT_LIST_DIR}/../images/\" )

#  Source files
# --------------
file( GLOB_RECURSE headers *.h)
file( GLOB_RECURSE sources *.cpp)


find_package(GTK2        2.6  REQUIRED  gtk gtkmm)
find_package(Notify           REQUIRED)


link_directories(
		${NOTIFY_LIBRARY_DIRS}
                 )

include_directories(${GTK2_INCLUDE_DIRS}
                    ${NOTIFY_INCLUDE_DIRS}
                    "${PROJECT_SOURCE_DIR}"
                    "${PROJECT_SOURCE_DIR}/logic/"
                    "${PROJECT_SOURCE_DIR}/gui/"
                    "${PROJECT_SOURCE_DIR}/misc/"
                    )


add_executable(${PROJECT_NAME} ${sources} ${headers} )

target_link_libraries(
		${PROJECT_NAME}
		${NOTIFY_LIBRARIES}
		${GTK2_LIBRARIES}
		libtimeit
                      )

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

