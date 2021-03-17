
#Roadmap
Instead of creating a ticket for each thing this document outlines the
direction of the development.

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
* Containing all non gui logic.
* Behaving like current "Quiet" mode.
* Message based communication to gui.

## Googletest
* In separate binary.
* Testing only the lib
* Coverage

## Build Debian package
* Can the debian directory be outside project root?

## Build rpm

## Remove cmake package
Use only native.

## Separate UI
* gtkmm
* wxWidgets?
* console?

## Optimization
In order of importance:
1. Readability and maintenance.
2. Robustness.
3. Performance.

## Readability 
* Guidelines to contributors on what is expected.
* Reduce complexity.
* Pass by value.
* camelCase -> snake_case.
* parameter lists formatted as lists.
* Remove noise
    * ::getName() -> ::name()
    * liberal use of "using namespace".

## robustness
* Immutable.
* No smart pointers if not needed.
* No raw pointers.
* No naked news.
* No null.
* No **unscoped** "using namespace" in header files

## New language features
Use the new stuff when beneficial.
* optional.
* replace string_sprintf with fmt
* auto.
* Copy elision (return value optimization).
* Lambda
* [[fallthrough]], [[nodiscard]] and [[maybe_unused]]
* string_view
* invoke?
* filesystem

And maybe more.


# Done

## Cleanup of directory structure
 * Moving src to subdirectory
 * async instead of thread


