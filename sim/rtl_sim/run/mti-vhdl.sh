#!/bin/bash

##########################################################################
# Run Mentor Modelsim simulation.
vsim -c -do "../bin/mu0_behav_vhdl.do"
mv mu0_behavioral.vcd ../out
##########################################################################

# Generate statistics.
if [ "$SECONDS" -eq 1 ]
then
  units=second
else
  units=seconds
fi
echo "This script has been running for $SECONDS $units."
exit 0
