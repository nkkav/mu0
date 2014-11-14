###
GHDL=ghdl
GHDLFLAGS=--ieee=standard -fexplicit --workdir=work
GHDLRUNFLAGS=--vcd=mu0_behavioral.vcd --stop-time=2147483647ns

all : run

elab : force
	$(GHDL) -c $(GHDLFLAGS) -e mu0_tb

run : force
	$(GHDL) --elab-run $(GHDLFLAGS) mu0_tb $(GHDLRUNFLAGS)

init : force
	mkdir work
	$(GHDL) -a $(GHDLFLAGS) ../vhdl/std_logic_textio.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../../rtl/vhdl/mu0_behav.vhd
	$(GHDL) -a $(GHDLFLAGS) ../../../bench/vhdl/mu0_tb.vhd

force : 

clean :
	rm -rf *.o work
