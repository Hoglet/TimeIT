project(timeit)

macro(subdirlist result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
        set(dirlist ${dirlist} ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

subdirlist( SUBDIRS "${PROJECT_SOURCE_DIR}/../src")

include(Translations)

add_translations_directory(${PROJECT_NAME})
add_translations_catalog(${PROJECT_NAME} ../src/)

       