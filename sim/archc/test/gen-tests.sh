#!/bin/bash

##########################################################################
for test in "test1" "test2" "test4" "test5" "test6" \
  "test7" "test8" "test9"
do
  ../../../sw/compile_mu0.exe -x -i ../../rtl_sim/run/${test}.mu0 -o ${test}.hex
done
##########################################################################

exit 0
