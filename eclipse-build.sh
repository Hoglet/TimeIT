#!/bin/bash
#
# Wrapper to make building in eclipse smooth and automatic.
# calling cmake before make is ensuring that new files 
# will be called
# ------------
cmake -DCMAKE_BUILD_TYPE=Debug
/usr/bin/make $@
