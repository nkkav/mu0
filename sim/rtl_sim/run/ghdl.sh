#!/bin/bash

make -f ../bin/mu0_behav.mk clean
make -f ../bin/mu0_behav.mk init
make -f ../bin/mu0_behav.mk run
mv mu0_behavioral.vcd ../out

if [ "$SECONDS" -eq 1 ]
then
  units=second
else
  units=seconds
fi
echo "This script has been running for $SECONDS $units."
exit 0
