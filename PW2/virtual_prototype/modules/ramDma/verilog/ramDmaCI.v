module ramDmaCi   #(parameter[7:0] customId = 8'h00)
                (input wire start,
                clock,
                reset,
                input wire[31:0] valueA,
                valueB,
                input wire[7:0] ciN,
                output wire done,
                output reg[31:0] result);

localparam IDLE = 1'b0;
localparam READ = 1'b1;

localparam MEMORY_LOCATION = 3'b000;
localparam BUS_START_ADDR = 3'b001;
localparam MEM_START_ADDR = 3'b010;
localparam BLOCK_SIZE = 3'b011;
localparam BURST_SIZE = 3'b100;
localparam STATUS_CONTROL = 3'b101;

reg current_state, next_state;

wire module_en;
assign module_en = (start == 1'b1 & ciN == customId); 

always @(posedge clock)
begin
    if(reset) current_state <= IDLE;
    else current_state <= next_state;
end


//Delay cycle for reads
always @(current_state, valueA) begin
    case (current_state)
        IDLE:
        begin
            if(module_en == 1'b1) begin
                case(valueA[12:9])
                    4'b0000: next_state = READ; 
                    default: next_state = IDLE;
                endcase  
            end
        end
        READ: next_state = IDLE;
        default: next_state = IDLE;
    endcase 
end




//write || second cycle of read
assign done = enable_a || current_state == READ;

wire[8:0] address_a;
wire enable_a;

assign enable_a = valueA[9];
assign address_a = valueA[8:0];
wire [31:0] read;

always @(read, current_state, reset) begin
    if(current_state == READ) result = read;
    else result = 32'b0;
end

dualPortSSRAM #(.bitwidth(32), .nrOfEntries(512))
    ram(
        .clockA(clock),
        .clockB(~clock),
        .writeEnableA(enable_a),
        .writeEnableB(),
        .addressA(address_a),
        .addressB(),
        .dataInA(valueB),
        .dataInB(),
        .dataOutA(read),
        .dataOutB()
    );
endmodule
            