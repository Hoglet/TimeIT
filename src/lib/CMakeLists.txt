
file( GLOB_RECURSE headers *.h)
file( GLOB_RECURSE sources *.cpp)

add_library( libtimeit STATIC
  ${sources}
  ${headers}
  )

target_link_libraries( libtimeit
        ${SQLITE_LIBRARIES}
        ${XSCRNS_LIBRARIES}
        ${XLIB_LIBRARIES}
        ${JANSSON_LIBRARIES}
        ${CURL_LIBRARIES}
        )

target_include_directories ( libtimeit PUBLIC
  inc
  inc/DB
  inc/Logic
  inc/Misc
  )


