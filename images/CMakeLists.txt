project(timeit)

file( GLOB_RECURSE SVG *.png)
file( GLOB_RECURSE PNG *.svg)

install(FILES ${SVG} ${PNG} DESTINATION share/${PROJECT_NAME})
install(FILES timeit.svg DESTINATION share/pixmaps)
       