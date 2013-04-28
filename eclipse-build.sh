#!/bin/bash
#
# Wrapper to make building in eclipse smooth and automatic.
# calling cmake before make is ensuring that new files 
# will be called
# ------------
cmake .
/usr/bin/make $@
