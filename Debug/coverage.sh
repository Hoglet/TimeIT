#/bin/bash
lcov --capture --directory src --output-file  coverage.info
lcov --extract coverage.info --output-file timeit.coverage "*/src/Sync/*" "*/src/DB/*"  "*/src/Logic/*" "*/src/Misc/*" 
genhtml timeit.coverage --output-directory .coverage
python lcov_cobertura.py timeit.coverage
rm coverage.info
rm timeit.coverage 

