`timescale 1ps/1ps
`include "./counter.v"

module profileCiTestBench;

  reg clock,reset;
  
  initial
    begin
      reset = 1'b1;
      clock = 1'b0;
      repeat (4) #5 clock = ~clock;
      reset = 1'b0;
      forever #5 clock = ~clock;
    end


    reg start, stalled, idle;
    reg [7:0] cIn;
    reg [31:0] valueA, valueB;
    wire done;
    wire [31:0] result; 

    profileCi #(.customId(8'd9))
    profiler(
        .start(start),
        .clock(clock),
        .reset(reset),
        .stall(stalled),
        .busIdle(idle),
        .valueA(valueA),
        .valueB(valueB),
        .ciN(cIn),
        .done(done),
        .result(result)
    );
       
  initial
    begin
      start = 1'b0;
      stalled = 1'b0;
      idle = 1'b0;
      valueA = 32'b0;
      valueB = 32'b0;
      @(negedge reset);

      // Try changing component while cIn != 9
      valueB = 32'b1;
      start = 1'b1;
      repeat(2) @(negedge clock);

      //try starting the first counter
      cIn = 8'd9;
      @(negedge clock);

      //start counter 2 and 3 but they shouldn't count yet
      valueB = 32'b0110;
      repeat(2) @(negedge clock);

      //counters 2 and 3 should start counting
      idle = 1'b1;
      stalled = 1'b1;
      repeat(4) @(negedge clock);

      //stop counter 1 and 2 (while trying to activate counter 1, which shouldn't work)
      valueB = 32'b00110010;
      @(negedge clock);
  
      //reset counter 3
      valueB = 32'b010000000000;
      @(negedge clock);
      valueB = 32'b0;

      //try reading counters
      valueA = 32'b00;
      @(negedge clock);
      valueA = 32'b01;
      @(negedge clock);
      valueA = 32'b10;
      @(negedge clock);
      valueA = 32'b11;
      @(negedge clock);

      $finish;
    end
      
  
  initial
    begin
      $dumpfile("profileCiSignals.vcd");
      $dumpvars(1,profiler);
    end
endmodule