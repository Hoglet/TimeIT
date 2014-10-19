#!/bin/bash
#
# ------------
cd Debug
./coverage.sh
find -name "*.gcda" | xargs rm
