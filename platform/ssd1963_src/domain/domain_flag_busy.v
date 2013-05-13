module domain_flag_busy (
    clkA, FlagIn_clkA, Busy_clkA, 
    clkB, FlagOut_clkB);

// clkA domain signals
input clkA, FlagIn_clkA;
output Busy_clkA;

// clkB domain signals
input clkB;
output FlagOut_clkB;

reg FlagToggle_clkA;
reg [2:0] SyncA_clkB;
reg [1:0] SyncB_clkA;

always @(posedge clkA) if(FlagIn_clkA & ~Busy_clkA) FlagToggle_clkA <= ~FlagToggle_clkA;
always @(posedge clkB) SyncA_clkB <= {SyncA_clkB[1:0], FlagToggle_clkA};
always @(posedge clkA) SyncB_clkA <= {SyncB_clkA[0], SyncA_clkB[2]};

assign FlagOut_clkB = (SyncA_clkB[2] ^ SyncA_clkB[1]);
assign Busy_clkA = FlagToggle_clkA ^ SyncB_clkA[1];
endmodule