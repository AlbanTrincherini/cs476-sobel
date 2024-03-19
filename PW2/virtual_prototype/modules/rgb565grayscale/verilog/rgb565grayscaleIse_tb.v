`timescale 1ps/1ps

module rgb565grayscaleIseTestbench;

  reg clock,reset;
  
  initial
    begin
      reset = 1'b1;
      clock = 1'b0;
      repeat (4) #5 clock = ~clock;
      reset = 1'b0;
      forever #5 clock = ~clock;
    end


    reg start;
    reg [7:0] cIn;
    reg [31:0] valueA;
    wire done;
    wire [31:0] result; 

    rgb565grayscaleIse #(.customInstructionId(8'd11))
    grayscale(
        .start(start),
        .valueA(valueA),
        .iseId(cIn),
        .done(done),
        .result(result)
    );
       
  initial
    begin
      start = 1'b1;
      valueA = 32'b0;
      cIn = 8'd11;
      @(negedge reset);
      
      valueA = {16'b0, 16'hFFFF};
    
      @(negedge clock);

    
      $finish;
    end
      
  
  initial
    begin
      $dumpfile("rgb565grayscaleIseSignals.vcd");
      $dumpvars(1,grayscale);
    end
endmodule