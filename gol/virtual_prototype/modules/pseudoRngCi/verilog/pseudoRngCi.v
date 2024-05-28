module pseudoRngCi #(parameter [7:0] customInstructionId = 8'd0 )
                            (input wire         reset,       
                             input wire         start,
                             input wire         clock,
                             input wire [7:0]   cIn,
                             output wire        done,
                             output wire [31:0] result );
  wire s_isMyCi = (cIn == customInstructionId) ? start : 1'b0;

localparam[31:0] SEED = 32'd2024;

reg[31:0] current;
wire[31:0] next;
wire randomizedBit;

assign randomizedBit = current[31]^current[30]^current[10]^current[0];
assign next = {randomizedBit,current[31:1]};

always @(posedge clock)
    begin
        current <= (reset == 1'b1) ? SEED : next;
    end

assign done   = s_isMyCi;
  assign result = (s_isMyCi == 1'b1) ? current : 32'd0;

endmodule

// source: https://www.eevblog.com/forum/fpga/looking-for-good-32-bit-pseudo-rng-in-systemverilog/