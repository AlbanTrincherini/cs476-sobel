`timescale 1ps/1ps

module profileCi_tb;

  reg clock,reset;
  
  initial
    begin
      reset = 1'b1;
      clock = 1'b0;
      repeat (4) #5 clock = ~clock;
      reset = 1'b0;
      forever #5 clock = ~clock;
    end

    profileCi #(.customId(8'd9))
    profiler(
        .start(),
        .clock(clock),
        .reset(s_cpuReset),
        .stall(s_cpuIsStalled),
        .busIdle(s_busIdle),
        .valueA(s_cpu1CiDataA),
        .valueB(s_cpu1CiDataB),
        .ciN(s_cpu1CiN),
        .done(s_profilerDone),
        .result(s_profilerResult)
    );
       
  initial
    begin
      @(negedge reset);
      forever @(negedge clock) if (s_direction == 1'b0 && s_value == 8'd127) $finish;
      $finish;
    end
      
  
  initial
    begin
      $dumpfile("profileCiSignals.vcd");
      $dumpvars(1,dut);
    end
endmodule