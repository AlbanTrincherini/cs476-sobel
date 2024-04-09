`timescale 1ps/1ps

module dmaEx1;

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
    reg [31:0] valueA, valueB;
    wire done;
    wire [31:0] result; 
    
    ramDmaCi #(.customId(8'd9))
        dma(
            .start(start),
            .clock(clock),
            .reset(reset),
            .valueA(valueA),
            .valueB(valueB),
            .ciN(cIn),
            .done(done),
            .result(result)
        );
       
  initial
    begin
        cIn = 8'd9;
        start = 1'b0;
        valueA = 32'b0;
        valueB = 32'b0;
        @(negedge reset);

        //write
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0001, 9'b000001000};
        valueB = 32'b1111;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0000, 9'b000001000};
        repeat(2) @(negedge clock);

        $finish;
    end
      
  
  initial
    begin
      $dumpfile("ramEx1.vcd");
      $dumpvars(1,dma);
    end
endmodule