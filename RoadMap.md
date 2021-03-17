
#Roadmap

## Cleanup
### project tree
* Clean root with:
    * Source: 
        * lib
        * (+Debian?, ....)
    * utility scripts: 
        * create IDE
        * build debian package
        * etc....   
    * Update README

## Create lib

## Googletest
* In separate binary.
* Testing only the lib
* Coverage

## Build Debian package
* Can the debian directory be outside project root?

## Build rpm

## Remove cmake package
Use only native.

## Separate the UI
* gtkmm
* wxWidgets?

## Readability 

* no null.
* pass by value.
* camelCase -> snake_case.
* parameter lists formatted as lists.
* Remove noise
    * ::getName() -> ::name()
    * liberal use of "using namespace" 

## robustness
* immutable.
* no pointers if not needed.
* no naked news.
* No **un scoped** "using namespace" in header files

## C++20 (C++17 first)
* Use the new stuff when beneficial
* replace string_sprintf with fmt 


# Done

## Cleanup of directory structure
 * Moving src to subdirectory


