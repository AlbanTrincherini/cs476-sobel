module rgb565grayscaleIse #(
    parameter [7:0] customInstructionId = 8'd0
) ( input wire          start,
    input wire[31:0]    valueA,
    input wire[7:0]     iseId,
    output wire         done,
    output reg [31:0]  result
    );
    
wire module_en;
assign module_en = (start == 1'b1 & iseId == customInstructionId); 
assign done = module_en;

wire [7:0] red, green, blue;

assign red = {valueA[15:11], 3'b0};
assign green = {valueA[10:5], 2'b0};
assign blue = {valueA[4:0], 3'b0};

wire [15:0] red32, red16, red4, red2;
wire [15:0] green128, green32, green16, green4, green2, green1;
wire [15:0] blue16, blue2, blue1;

assign red32 = {3'b0, red, 5'b0};
assign red16 = {4'b0, red, 4'b0};
assign red4 = {6'b0, red, 2'b0};
assign red2 = {7'b0, red, 1'b0};

assign green128 = {1'b0 , green, 7'b0};
assign green32 = {3'b0 , green, 5'b0};
assign green16 = {4'b0, green, 4'b0};
assign green4 = {6'b0, green, 2'b0};
assign green2 = {7'b0, green, 1'b0};
assign green1 = {8'b0, green};

assign blue16 = {4'b0, blue, 4'b0};
assign blue2 = {7'b0, blue, 1'b0};
assign blue1 = {8'b0, blue};

wire [15:0] sum, sumRed, sumBlue, sumGreen;
assign sumRed = red32 + red16 + red4 + red2;
assign sumGreen = green128 + green32 + green16 + green4 + green2 + green1;
assign sumBlue = blue16 + blue2 + blue1;
assign sum = sumBlue + sumRed + sumGreen;

wire [31:0] grayscale;
assign grayscale = {24'b0, sum[15:8]};

always @* begin
if (module_en == 1'b0) result <= 32'd0;
else result <= grayscale;
end

endmodule