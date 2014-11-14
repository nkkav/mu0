###
vlib work
vlog ../../../rtl/verilog/mu0_behav.v
vlog ../../../bench/verilog/mu0_tb.v
vsim mu0_tb
vcd dumpports -file mu0_behavioral.vcd /mu0_tb/uut/* -unique
onbreak {quit -f}
run -all
exit -f
