module rgb565GrayscaleIse #(parameter [7:0] customInstructionId = 8'd0 )
                           ( input wire         start,
                             input wire [31:0]  valueA,
                                                valueB,
                             input wire [7:0]   cIn,
                             output wire        done,
                             output wire [31:0] result );
  wire s_isMyCi = (cIn == customInstructionId) ? start : 1'b0;

reg[15:0] current;
wire[15:0]

endmodule