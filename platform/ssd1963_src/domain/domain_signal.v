module domain_signal (
    clkA, SignalIn, 
    clkB, SignalOut);

// clkA domain signals
input clkA;
input SignalIn;

// clkB domain signals
input clkB;
output SignalOut;

// Now let's transfer SignalIn into the clkB clock domain
// We use a two-stages shift-register to synchronize the signal
reg [1:0] SyncA_clkB;
always @(posedge clkB) SyncA_clkB[0] <= SignalIn;      // notice that we use clkB
always @(posedge clkB) SyncA_clkB[1] <= SyncA_clkB[0]; // notice that we use clkB

assign SignalOut = SyncA_clkB[1];  // new signal synchronized to (=ready to be used in) clkB domain
endmodule