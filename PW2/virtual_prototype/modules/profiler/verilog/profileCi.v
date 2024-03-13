module profileCi #( parameter[7:0] customId = 8'h00 )
(
    input wire          start, 
                        clock, 
                        reset,
                        stall,
                        busIdle,
    input wire [31:0]   valueA, 
                        valueB,
    input wire [7:0]    ciN,
    output wire         done,
    output wire [31:0]  result
);

    parameter count_up = 1'b1;
    parameter ctr_size = 32;

    wire [ctr_size - 1:0] ctr0, ctr1, ctr2, ctr3;
    reg c0_on, c1_on, c2_on, c3_on;
    wire c0_en, c1_en, c2_en, c3_en;
    wire c0_r, c1_r, c2_r, c3_r;
    wire module_en;
    reg[ctr_size - 1:0] s_result;

    assign module_en = (start == 1'b1 & ciN == customId); 

    assign c0_en = c0_on;
    assign c1_en = c1_on & stall;
    assign c2_en = c2_on & busIdle;
    assign c3_en = c3_on;

    assign result = s_result;
    assign done = module_en;

    always @(module_en, valueA, ctr0, ctr1, ctr2, ctr3) begin
        if(module_en == 1'b1) begin
            case (valueA[1:0])
                2'b00: s_result = ctr0;
                2'b01: s_result = ctr1;
                2'b10: s_result = ctr2;
                2'b11: s_result = ctr3;
                default: s_result = {ctr_size{1'b0}};
            endcase
        end
        else s_result = {ctr_size{1'b0}};
    end
    counter #(.WIDTH(ctr_size))
        c0(
            .reset(c0_r),
            .clock(clock),
            .enable(c0_en),
            .direction(count_up),
            .counterValue(ctr0)
        );

    counter #(.WIDTH(ctr_size))
        c1(
            .reset(c1_r),
            .clock(clock),
            .enable(c1_en),
            .direction(count_up),
            .counterValue(ctr1)
        );

    counter #(.WIDTH(ctr_size))
        c2(
            .reset(c2_r),
            .clock(clock),
            .enable(c2_en),
            .direction(count_up),
            .counterValue(ctr2)
        );

    counter #(.WIDTH(ctr_size))
        c3(
            .reset(c3_r),
            .clock(clock),
            .enable(c3_en),
            .direction(count_up),
            .counterValue(ctr3)
        );

    assign c0_r = (valueB[8] & module_en)  | reset;
    assign c1_r = (valueB[9] & module_en)  | reset;
    assign c2_r = (valueB[10] & module_en) | reset;
    assign c3_r = (valueB[11] & module_en) | reset;
            

    always @(posedge clock) begin
        if(module_en == 1'b1) begin
            if (valueB[0]) c0_on = 1'b1;
            if (valueB[1]) c1_on = 1'b1;
            if (valueB[2]) c2_on = 1'b1;
            if (valueB[3]) c3_on = 1'b1;
            if (valueB[4]) c0_on = 1'b0;
            if (valueB[5]) c1_on = 1'b0;
            if (valueB[6]) c2_on = 1'b0;
            if (valueB[7]) c3_on = 1'b0;
        end
    end 
    
endmodule