# Coding guidelines
This document is intended to encourage good coding practices 
without inhibiting progress. Rules herein should be open ended 
to allow the author to "do better". 

The main rules in order of importance:
1. Readability and maintenance.
2. Robustness.
3. Performance.

Old code is often breaking rules and should be rewritten when time permits.
## Hard rules
Indent with tabs. 

## Readability 
* Ensure horizontal and vertical whitespace.
* Form parameters as lists.
* Symmetry. 
* Reduce complexity.
* camelCase -> snake_case.
* Remove noise
    * ::getName() -> ::name()
    * liberal use of "using namespace" 

## robustness
* Immutable.
* No smart pointers if not really needed.
* Pass by value.
* No raw pointers 
* No naked new.
* No null.
* No **unscoped** "using namespace" in header files
