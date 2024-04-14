module ramDmaCi   #(parameter[7:0] customId = 8'h00)
                (input wire start,
                clock,
                reset,
                input wire[31:0] valueA,
                valueB,
                input wire[7:0] ciN,
                output wire done,
                output reg[31:0] result);


wire module_en = (start == 1'b1 & ciN == customId); 

//write || second cycle of read
assign done = enable_a || read_current != IDLE;

reg [31:0]  r_bus_start;
reg [8:0]   r_mem_start;
reg [9:0]   r_block_size;
reg [7:0]   r_burst_size;

localparam MEMORY_LOCATION  = 3'b000;
localparam BUS_START_ADDR   = 3'b001;
localparam MEM_START_ADDR   = 3'b010;
localparam BLOCK_SIZE       = 3'b011;
localparam BURST_SIZE       = 3'b100;
localparam STATUS_CONTROL   = 3'b101;

/*
 * READ
 */
localparam IDLE             = 3'b111;
reg [3:0] read_current, read_next;

//FSM
always @(posedge clock)
begin
    if(reset) read_current <= IDLE;
    else read_current <= read_next;
end

//Delay cycle for reads
always @(read_current, valueA, module_en) begin
    if(read_current == IDLE && valueA[9] == 1'b0) read_next = valueA[12:10];
    else read_next = IDLE;    
end

//Write read result
always @(read_a, read_current, reset) begin
    case (read_current)
        MEMORY_LOCATION : result = read_a;
        BUS_START_ADDR  : result = r_bus_start;
        MEM_START_ADDR  : result = {24'b0, r_mem_start};
        BLOCK_SIZE      : result = {23'b0, r_block_size};
        BURST_SIZE      : result = {25'b0, r_burst_size};
        STATUS_CONTROL  : ;//TODO:
        default         : result = 32'b0; //Also acts as reset
    endcase
end

/*
 * WRITE
 */
wire enable_a = valueA[9];
wire [8:0] address_a = valueA[8:0];
wire [31:0] read_a;

always @(posedge clock) begin
    if(reset) begin
        r_bus_start     = 32'b0;
        r_mem_start     = 9'b0;
        r_block_size    = 10'b0;
        r_burst_size    = 8'b0;
    end
    else if(enable_a) begin
        case (valueA[12:10])
            BUS_START_ADDR  : r_bus_start   = valueB;
            MEM_START_ADDR  : r_mem_start   = valueB[8:0];
            BLOCK_SIZE      : r_block_size  = valueB[9:0];
            BURST_SIZE      : r_burst_size  = valueB[7:0];
            STATUS_CONTROL  :; //TODO:
            default         : ;//do nothing
        endcase
    end
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
        .dataOutA(read_a),
        .dataOutB()
    );
endmodule
            