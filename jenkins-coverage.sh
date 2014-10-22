#!/bin/bash
#
# ------------
cd Debug
./coverage.sh
find -name "*.gcda" | xargs rm
sed "s/\.\.\///g"  < coverage.xml > ../coverage.xml