`timescale 1ps/1ps

module dmaEx2;

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
    reg bus_terminated;
    reg bus_error;
    reg bus_valid;
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
            .bus_in(bus_in)
        );
       
  initial
    begin
        cIn = 8'd9;
        start = 1'b0;
        valueA = 32'b0;
        valueB = 32'b0;

        bus_grants = 1'b0;
        bus_terminated = 1'b0;
        bus_error = 1'b0;
        bus_valid = 1'b0;
        bus_in = 32'b0;
        @(negedge reset);

        //write
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0001, 9'b000001000};
        valueB = 32'b1111;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0000, 9'b000001000};
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
        valueB = 32'h0010;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0100, 9'b000000000};
        repeat(2) @(negedge clock);

        //block size
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b0111, 9'b000000000};
        valueB = 32'd5;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0110, 9'b000000000};
        repeat(2) @(negedge clock);

        //burst size
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b1001, 9'b000000000};
        valueB = 32'd1;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b1000, 9'b000000000};
        repeat(2) @(negedge clock);

        //start dma
        start = 1'b1;
        valueA = {{19{1'b0}}, 4'b1011, 9'b000000000};
        valueB = 32'b1;
        @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b1010, 9'b000000000};
        repeat(5) @(negedge clock); //waiting for bus_grants
        
        bus_grants = 1'b1;
        repeat(5) @(negedge clock); //waiting for data_valid

        bus_valid = 1'b1;
        bus_in = 32'hFFFF;
        repeat(20) @(negedge clock);

        //read
        valueA = {{19{1'b0}}, 4'b0000, 9'h10};
        repeat(2) @(negedge clock);
                //read
        valueA = {{19{1'b0}}, 4'b0000, 9'h11};
        repeat(2) @(negedge clock);
                //read
        valueA = {{19{1'b0}}, 4'b0000, 9'h12};
        repeat(2) @(negedge clock);
                //read
        valueA = {{19{1'b0}}, 4'b0000, 9'h13};
        repeat(2) @(negedge clock);
                //read
        valueA = {{19{1'b0}}, 4'b0000, 9'h14};
        repeat(2) @(negedge clock);
        //read
        valueA = {{19{1'b0}}, 4'b0000, 9'h15};
        repeat(2) @(negedge clock);
        $finish;
    end
      
  
  initial
    begin
      $dumpfile("ramEx2.vcd");
      $dumpvars(1,dma);
    end
endmodule