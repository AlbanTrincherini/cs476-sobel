module edgecaptureCi #(parameter [7:0] customInstructionId = 8'd0 )
                            (input wire         reset,       
                             input wire         start,
                             input wire         clock,
                             input wire [7:0]   cIn,
                             input wire [4:0]   buttons,
                             input wire [31:0]  valueA,
                             output wire        done,
                             output wire [31:0] result );

wire s_isMyCi = (cIn == customInstructionId) ? start : 1'b0;

reg [4:0] edgecapture;

always @(posedge clock)
    begin
        if(reset == 1'b1) 
            edgecapture <= 5'b0;
        else if(valueA[0] == 1'b1) //Clear the edgecapture
            edgecapture <= 5'b0;
        else
            edgecapture <= edgecapture | buttons;
    end

assign done   = s_isMyCi;
assign result = (s_isMyCi == 1'b1 && valueA[0] == 1'b0) ? {27'b0, edgecapture} : 32'd0;

endmodule