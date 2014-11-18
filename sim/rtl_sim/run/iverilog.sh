#!/bin/sh

IVERILOG_PATH=/c/CompSci/iverilog/bin
#IVERILOG_PATH=/cygdrive/c/CompSci/iverilog/bin

echo "Testing: mu0 (behavioral)"
${IVERILOG_PATH}/iverilog -g1995 -o mu0_tb ../../../rtl/verilog/mu0_behav.v \
  ../../../bench/verilog/mu0_tb.v
${IVERILOG_PATH}/vvp mu0_tb
cp -f mu0.vcd mu0_behavioral.vcd
rm -rf mu0.vcd
mv mu0_behavioral.vcd ../out

if [ "$SECONDS" -eq 1 ]
then
  units=second
else
  units=seconds
fi
echo "This script has been running for $SECONDS $units."

echo "Ran all tests."
exit 0
