module fifo #(
    parameter nrOfEntries = 16,
    parameter bitWidth = 32
) (
    input wire clock, reset, push, pop,
    input wire [bitWidth - 1:0] pushData,
    output wire full, empty,
    output wire [bitWidth - 1:0] popData
);

    wire enable_top, enable_bot;
    wire [$clog2(nrOfEntries) - 1: 0] counter_top, counter_bot;
    parameter count_up = 1'b1;

    counter #(.WIDTH($clog2(nrOfEntries))) 
        top(
        .reset(reset),
        .clock(clock),
        .enable(enable_top),
        .direction(count_up),
        .counterValue(counter_top)
        );

    counter #(.WIDTH($clog2(nrOfEntries)))
        bot(
        .reset(reset),
        .clock(clock),
        .enable(enable_bot),
        .direction(count_up),
        .counterValue(counter_bot)
        );

    semiDualPortSSRAM #(.bitwidth(bitWidth),
                    .nrOfEntries(nrOfEntries),
                    .readAfterWrite(0))
        queue(
        .clockA(clock),
        .clockB(clock),
        .writeEnable(enable_top),
        .addressA(counter_top),
        .addressB(counter_bot),
        .dataIn(pushData),
        .dataOutA(),
        .dataOutB(popData)
        ); 

    assign enable_top = push;
    assign enable_bot = pop;

    assign full = (counter_top == counter_bot - 1);
    assign empty = (counter_top == counter_bot);

endmodule

