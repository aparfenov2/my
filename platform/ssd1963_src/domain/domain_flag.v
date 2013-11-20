module domain_flag (
    clkA, FlagIn_clkA, 
    clkB, FlagOut_clkB);

// clkA domain signals
input clkA, FlagIn_clkA;

// clkB domain signals
input clkB;
output FlagOut_clkB;

reg FlagToggle_clkA;
reg [2:0] SyncA_clkB;

// this changes level when a flag is seen
always @(posedge clkA) if(FlagIn_clkA) FlagToggle_clkA <= ~FlagToggle_clkA;

// which can then be sync-ed to clkB
always @(posedge clkB) SyncA_clkB <= {SyncA_clkB[1:0], FlagToggle_clkA};

// and recreate the flag from the level change
assign FlagOut_clkB = (SyncA_clkB[2] ^ SyncA_clkB[1]);
endmodule