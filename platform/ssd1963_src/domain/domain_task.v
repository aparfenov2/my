module domain_task (
    clkA, TaskStart_clkA, TaskBusy_clkA, TaskDone_clkA, 
    clkB, TaskStart_clkB, TaskBusy_clkB, TaskDone_clkB);

// clkA domain signals
input clkA;
input TaskStart_clkA;
output TaskBusy_clkA, TaskDone_clkA;

// clkB domain signals
input clkB;
output TaskBusy_clkB, TaskStart_clkB;
input TaskDone_clkB;

reg FlagToggle_clkA, FlagToggle_clkB, Busyhold_clkB;
reg [2:0] SyncA_clkB, SyncB_clkA;

always @(posedge clkA) if(TaskStart_clkA & ~TaskBusy_clkA) FlagToggle_clkA <= ~FlagToggle_clkA;

always @(posedge clkB) SyncA_clkB <= {SyncA_clkB[1:0], FlagToggle_clkA};
assign TaskStart_clkB = (SyncA_clkB[2] ^ SyncA_clkB[1]);
assign TaskBusy_clkB = TaskStart_clkB | Busyhold_clkB;
always @(posedge clkB) Busyhold_clkB <= ~TaskDone_clkB & TaskBusy_clkB;
always @(posedge clkB) if(TaskBusy_clkB & TaskDone_clkB) FlagToggle_clkB <= FlagToggle_clkA;

always @(posedge clkA) SyncB_clkA <= {SyncB_clkA[1:0], FlagToggle_clkB};
assign TaskBusy_clkA = FlagToggle_clkA ^ SyncB_clkA[2];
assign TaskDone_clkA = SyncB_clkA[2] ^ SyncB_clkA[1];
endmodule
