
file( GLOB_RECURSE sources *.cpp)

add_library( libtimeit STATIC
             ${sources})

target_include_directories ( libtimeit PUBLIC
  inc
  inc/DB
  inc/Logic
  inc/Misc
  )


