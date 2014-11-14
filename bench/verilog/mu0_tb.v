`timescale 1 ns/1 ps

module mu0_tb;
  parameter MAXWIDTH = 16, MAXDEPTH = 12;
  parameter STP = 4'b0111;
  parameter CLK_PERIOD=10/*ns*/;
  reg clk, reset;
  wire [MAXWIDTH-1:0] pc;
  wire [MAXWIDTH-1:0] ir;
  wire [MAXWIDTH-1:0] acc;

  mu0 uut (
    .clk(clk), 
    .reset(reset),
    .pc(pc), 
    .ir(ir), 
    .acc(acc)
  );

  // Test clock generation.
  //
  // Clock pulse starts from 0.
  initial
  begin
    clk   = 1'b0;
    reset = 1'b1;
    #(CLK_PERIOD) reset = 1'b0;
  end
  // Free-running clock
  always
    #(CLK_PERIOD/2) clk = ~clk;

  // Write simulation data to a VCD waveform file.
  initial
  begin
    // Open a VCD file for writing
    $dumpfile("mu0.vcd");
    // Dump the values of all nets and wires in module "gcd_main", seen
    // from hierarchy level 0
    $dumpvars(0, mu0_tb);
  end

  always
  begin
    #(CLK_PERIOD);
    // Detect STOP/STP opcode.
    if (ir[MAXWIDTH-1:MAXWIDTH-4] == STP) 
    begin
      // Automatic end of the current simulation.
      $display("End simulation time reached.");
      $finish;
    end
  end

endmodule
