#!/bin/bash

##########################################################################
# Load program to VHDL or Verilog HDL model.
#
# Usage:
# $ ./load-program.sh $1
# where:
#  $1 = <program name>
#
# This means that a pre-existing *.lst file is copied to prog.lst.
##########################################################################
cp -f $1 prog.lst
##########################################################################
exit 0
