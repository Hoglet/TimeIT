#!/bin/bash

# Generate projects for eclipse, codelite and maybe others in the future

# out of source builds!!!!
SCRIPT_DIR=$( dirname "${BASH_SOURCE[0]}" )
cd $SCRIPT_DIR/../Debug
pwd
echo cmake -G "CodeLite - Ninja" -D CMAKE_BUILD_TYPE=Debug ../src/
echo cmake -G "Eclipse CDT4 - Ninja" -D CMAKE_BUILD_TYPE=Debug ../src
