project(timeit)

install(FILES timeit.1 DESTINATION man/man1)
install(DIRECTORY html DESTINATION share/doc/${PROJECT_NAME})
        
       