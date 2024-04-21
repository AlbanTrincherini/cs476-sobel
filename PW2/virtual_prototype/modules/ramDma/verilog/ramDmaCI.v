module ramDmaCi   #(parameter[7:0] customId = 8'h00)
                (input wire start,
                clock,
                reset,
                input wire[31:0] valueA,
                valueB,
                input wire[7:0] ciN,
                input wire[31:0] bus_in, 
                input wire bus_grants,
                input wire bus_error,
                input wire bus_valid,
                output wire bus_request,
                output reg bus_begin,
                output reg bus_read,
                output reg [3:0] bus_byte_enable,
                output reg bus_terminated,
                output reg [7:0] bus_burst_size, //has to be 0 when disabled
                output reg [31:0] bus_out,
                output wire done,
                output reg[31:0] result);


wire module_en = (start == 1'b1 & ciN == customId); 

//write || second cycle of read
assign done = enable_a || read_current != IDLE;

reg [31:0]  r_bus_start;
reg [8:0]   r_mem_start;
reg [9:0]   r_block_size;
reg [7:0]   r_burst_size;

// Possible values for A[12:10]
localparam MEMORY_LOCATION  = 3'b000;
localparam BUS_START_ADDR   = 3'b001;
localparam MEM_START_ADDR   = 3'b010;
localparam BLOCK_SIZE       = 3'b011;
localparam BURST_SIZE       = 3'b100;
localparam STATUS_CONTROL   = 3'b101;

/*
 * READ signals
 */
localparam IDLE             = 3'b111;
reg [3:0] read_current, read_next;

/*
 * WRITE signals
 */
wire enable_a = valueA[9];
wire [8:0] address_a = valueA[8:0];
wire [31:0] read_a;

//FSM
always @(posedge clock)
begin
    if(reset) read_current <= IDLE;
    else read_current <= read_next;
end

//Delay cycle for reads
always @(read_current, valueA, module_en) begin
    if(read_current == IDLE && valueA[9] == 1'b0 && module_en) read_next = valueA[12:10];
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
        STATUS_CONTROL  : result = {30'b0, r_err_current, dma_status};
        default         : result = 32'b0; //Also acts as reset
    endcase
end



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
            STATUS_CONTROL  :; //Nothing to do handled in the DMA logic (writing the control register)
            default         :; //do nothing
        endcase
    end
end

/*
 * DMA logic
 */
localparam DMA_IDLE     = 3'b000;
localparam DMA_REQUEST  = 3'b001;
localparam DMA_TRANSFER = 3'b010;
localparam DMA_SETUP    = 3'b011;
localparam DMA_END_TR   = 3'b100;
localparam DMA_END_OP   = 3'b101;

localparam STATUS_OK    = 1'b0;
localparam STATUS_ERR   = 1'b1;

reg [2:0]   dma_current, dma_next;
reg [9:0]   block_rem_c, block_rem_n;
reg [7:0]   burst_rem_c, burst_rem_n;
reg [31:0]  r_bus_start_c, r_bus_start_n;
reg [8:0]   r_mem_start_c, r_mem_start_n;
reg         r_err_current, r_err_next;

//Dff
always @(posedge clock) begin
    if(reset) begin
        dma_current         = DMA_IDLE;
        r_err_current       = STATUS_OK;
        block_rem_c         = 10'b0;
        burst_rem_c         = 8'b0;
        r_bus_start_c       = 32'b0;
        r_mem_start_c       = 9'b0;
    end
    else begin 
        dma_current     = dma_next;
        r_err_current   = r_err_next;
        block_rem_c     = block_rem_n;
        burst_rem_c     = burst_rem_n;
        r_bus_start_c   = r_bus_start_n;
        r_mem_start_c   = r_mem_start_n;
    end
end

always @(dma_current, r_err_current, bus_error, bus_grants, valueA, valueB, 
        bus_valid, block_rem_c, burst_rem_c, r_mem_start_c, r_bus_start_c) begin
    r_err_next = r_err_current;
    dma_next = dma_current;
    block_rem_n = block_rem_c;
    burst_rem_n = burst_rem_c;
    r_mem_start_n = r_mem_start_c;
    r_bus_start_n = r_bus_start_c;
    
    case (dma_current)
        DMA_IDLE: begin
            if(valueB[0] == 1 && valueA[12:9] == 4'b1011) begin
                dma_next = DMA_REQUEST;
                r_err_current = STATUS_OK;
                block_rem_n = r_block_size;
                burst_rem_n = r_burst_size + 1;
                r_mem_start_n = r_mem_start;
                r_bus_start_n = r_bus_start;
            end
        end
        DMA_REQUEST: begin
            if(bus_grants) dma_next = DMA_SETUP;
        end
        DMA_SETUP: dma_next = DMA_TRANSFER;
        DMA_TRANSFER: begin
            if(bus_error) begin 
                r_err_next = STATUS_ERR; //there was an error stop transfer
                dma_next = DMA_END_OP;
            end
            else if(bus_valid) begin
                block_rem_n = block_rem_c - 1;
                burst_rem_n = burst_rem_c - 1;
                r_bus_start_n = r_bus_start_c + 4;
                r_mem_start_n = r_mem_start_c + 1;
                if(block_rem_n == 10'b0) dma_next = DMA_END_OP;
                if(burst_rem_n == 8'b0) begin
                    burst_rem_n = r_burst_size + 1;
                    dma_next    = DMA_END_TR; //we will restart a new transaction
                end
            end
        end
        DMA_END_TR: dma_next = DMA_REQUEST;
        DMA_END_OP: dma_next = DMA_IDLE;
        default: ;
    endcase
end

//output logic
always @(dma_current, reset) begin
    if(dma_current == DMA_SETUP) begin
        bus_read        = 1'b1;
        bus_byte_enable = 4'b1111;
        bus_begin       = 1'b1;
        bus_burst_size  = r_burst_size;
        bus_out         = r_bus_start_c;
    end 
    else if(dma_current == DMA_END_TR || dma_current == DMA_END_OP) begin
        bus_terminated = 1'b1;
    end
    else begin
        bus_read        = 1'b0;
        bus_byte_enable = 4'b0000;
        bus_begin       = 1'b0;
        bus_burst_size  = 8'b0;
        bus_out         = 32'b0;

        bus_terminated = 1'b0;
    end
    
end

assign bus_request = dma_current == DMA_REQUEST;

//Control signals
wire dma_status = dma_current != DMA_IDLE;
wire dma_on     = dma_current == DMA_TRANSFER && bus_valid;

dualPortSSRAM #(.bitwidth(32), .nrOfEntries(512))
    ram(
        .clockA(clock),
        .clockB(~clock),
        .writeEnableA(enable_a),
        .writeEnableB(dma_on),
        .addressA(address_a),
        .addressB(r_mem_start_c),
        .dataInA(valueB),
        .dataInB(bus_in),
        .dataOutA(read_a),
        .dataOutB()
    );
endmodule
            