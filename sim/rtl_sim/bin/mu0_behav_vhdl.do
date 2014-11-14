###
vlib work
vcom ../vhdl/std_logic_textio.vhd
vcom ../../../rtl/vhdl/mu0_behav.vhd
vcom ../../../bench/vhdl/mu0_tb.vhd
vsim mu0_tb
vcd dumpports -file mu0_behavioral.vcd /mu0_tb/uut/* -unique
onbreak {quit -f}
run -all
exit -f
