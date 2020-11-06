
file( GLOB_RECURSE headers *.h)
file( GLOB_RECURSE sources *.cpp)

add_library( libtimeit STATIC
  ${sources}
  ${headers}
  )

target_include_directories ( libtimeit PUBLIC
  inc
  inc/DB
  inc/Logic
  inc/Misc
  )


