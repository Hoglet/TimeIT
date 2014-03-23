#/bin/bash
lcov --capture --directory src --output-file  coverage.info
lcov --extract coverage.info --output-file timeit.coverage "*/src/Sync/*" "*/src/DB/*"  "*/src/Logic/*" "*/src/Misc/*" 
genhtml timeit.coverage --no-branch-coverage --output-directory .coverage
firefox .coverage/index.html&
rm coverage.info
rm timeit.coverage
