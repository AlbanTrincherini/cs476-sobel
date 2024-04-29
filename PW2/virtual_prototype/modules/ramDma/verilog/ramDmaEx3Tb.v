`timescale 1ps/1ps

module dmaEx3;

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

    reg [31:0] bus_in;
    reg bus_grants;
    reg bus_error;
    reg bus_valid;
    reg bus_busy_in;
    wire bus_terminated;
    wire bus_request;
    wire [7:0] bus_burst_size;
    wire [31:0] bus_out;
    
    ramDmaCi #(.customId(8'd9))
        dma(
            .start(start),
            .clock(clock),
            .reset(reset),
            .valueA(valueA),
            .valueB(valueB),
            .ciN(cIn),
            .done(done),
            .result(result),
            .bus_grants(bus_grants),
            .bus_terminated(bus_terminated),
            .bus_error(bus_error),
            .bus_valid(bus_valid),
            .bus_request(bus_request),
            .bus_burst_size(bus_burst_size),
            .bus_out(bus_out),
            .bus_in(bus_in),
            .bus_busy_in(bus_busy_in)
        );
       
  initial
    begin
        cIn = 8'd9;
        start = 1'b0;
        valueA = 32'b0;
        valueB = 32'b0;
        bus_busy_in = 1'b0;

        bus_grants = 1'b0;
        bus_error = 1'b0;
        bus_valid = 1'b0;
        bus_in = 32'b0;
        @(negedge reset);

        //write
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0001, 9'd0};
        valueB = 32'd1;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0000, 9'd0};
        repeat(2) @(negedge clock);
  
        //write
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0001, 9'd1};
        valueB = 32'd2;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0000, 9'd1};
        repeat(2) @(negedge clock);

        //write
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0001, 9'd2};
        valueB = 32'd3;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0000, 9'd2};
        repeat(2) @(negedge clock);
        
        //bus start
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0011, 9'b000000000};
        valueB = 32'b1000;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0010, 9'b000000000};
        repeat(2) @(negedge clock);

        //memory start
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0101, 9'b000000000};
        valueB = 32'h0000;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0100, 9'b000000000};
        repeat(2) @(negedge clock);

        //block size
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0111, 9'b000000000};
        valueB = 32'd3;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0110, 9'b000000000};
        repeat(2) @(negedge clock);

        //burst size
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b1001, 9'b000000000};
        valueB = 32'd0;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b1000, 9'b000000000};
        repeat(2) @(negedge clock);







        //false start dma
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b1011, 9'b000000000};
        valueB = 32'b11;
        @(negedge clock);
        repeat(5) @(negedge clock);

        //start dma
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b1011, 9'b000000000};
        valueB = 32'b10;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b1010, 9'b000000000};
        repeat(5) @(posedge clock); //waiting for bus_grants
        
        bus_grants = 1'b1;
        bus_busy_in = 1'b1;

        repeat(5) @(posedge clock); //waiting for not busy

        bus_busy_in = 1'b0;

        
        repeat(40) @(negedge clock);
        //We should now be able to observe the bus_out contain 1, 2, 3
        $finish;
    end
      
  
  initial
    begin
      $dumpfile("ramEx3.vcd");
      $dumpvars(1,dma);
    end
endmodule