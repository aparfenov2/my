module ssd1963_sm (
	clk,
	disp_clk,
	reset_n,
	
// avalon slave	interface
	avalon_slave_address,
	avalon_slave_readdata,
	avalon_slave_writedata,
	avalon_slave_write,
	avalon_slave_read,
	avalon_slave_chipselect,
	avalon_slave_byteenable,
	
// avalon master interface	
	master_address,
	master_write,
	master_byteenable,
	master_writedata,
	master_waitrequest,
	
// display interface	
	disp_cs_n,
	disp_dc_n,
	disp_wr_n,
	disp_rd_n,
	disp_rst_n,
	disp_ud,
	disp_d
	);


	
input clk;
input reset_n;
// avalon slave	interface
input [3:0] avalon_slave_address;
input [3:0] avalon_slave_byteenable;
input avalon_slave_chipselect;
input avalon_slave_write;

input [31:0] avalon_slave_writedata;
output [31:0] avalon_slave_readdata;
input avalon_slave_read;

// display interface	
output disp_cs_n;
output disp_dc_n;
output disp_wr_n;
output disp_rd_n;
output disp_rst_n;
output disp_ud;
output [7:0] disp_d;

// write master interface
wire [31:0] control_write_base;
wire [31:0] control_write_length;
wire control_go;
wire control_done;
wire [31:0] user_buffer_data;
wire user_buffer_full;
wire user_write_buffer;

write_master wrm0 (
	.clk(clk),
	.reset(~reset_n),
	.control_fixed_location(1'b0),
	.control_write_base(control_write_base),
	.control_write_length(control_write_length),
	.control_go(control_go),
	.control_done(control_done),
	.user_buffer_data(user_buffer_data),
	.user_buffer_full(user_buffer_full),
	.user_write_buffer(user_write_buffer),
   
	.master_address(master_address),
	.master_write(master_write),
	.master_byteenable(master_byteenable),
	.master_writedata(master_writedata),
	.master_waitrequest(master_waitrequest)
   
);
// ----------------------------- Avalon MM registers -------------------------
reg [7:0] ctl_r; // управляющий
reg [7:0] flags_r; // флаги
reg [7:0] cmd_dat_r; // байт отсылаемых данных в режиме одиночной посылки
reg [15:0] sx1_r; // координаты области - источника
reg [15:0] sy1_r; 
reg [15:0] sw_r; // размер области - источника
reg [15:0] sh_r;
reg [15:0] w_r; // размеры холста, с которого считываются данныне
reg [15:0] h_r;
reg [31:0] src_start_r; // область памяти холста, с которого считываются данныне 

`define ADR_CTL 0
`define ADR_FLAGS 1
`define ADR_CMDDAT 2
`define ADR_SX1 3
`define ADR_SY1 4
`define ADR_SW 5
`define ADR_SH 6
`define ADR_W 7
`define ADR_H 8
`define ADR_SRC 9

// avalon write
always @(posedge clk or negedge reset_n)
	if (!reset_n)
		begin
//		ctl_r <= 8'b0;
//		flags_r <= 8'b0;
		cmd_dat_r <= 8'b0;
		sx1_r <= 16'b0;
		sy1_r <= 16'b0;
		sw_r <= 16'b0;
		sh_r <= 16'b0;
		w_r <= 16'b0;
		h_r <= 16'b0;
		src_start_r <= 32'b0;
		end
	else if (avalon_slave_chipselect && avalon_slave_write)
	case (avalon_slave_address)
//		`ADR_CTL	: if (avalon_slave_byteenable[0]) ctl_r	<= avalon_slave_writedata[7:0];
//		`ADR_FLAGS	: if (avalon_slave_byteenable[0]) flags_r	<= avalon_slave_writedata[7:0]; - флаги только на чтение
		`ADR_CMDDAT	: if (avalon_slave_byteenable[0]) cmd_dat_r	<= avalon_slave_writedata[7:0];
		`ADR_SX1: 	begin
					if (avalon_slave_byteenable[0]) sx1_r[7:0]	<= avalon_slave_writedata[7:0];
					if (avalon_slave_byteenable[1]) sx1_r[15:8]	<= avalon_slave_writedata[15:8];
					end
		`ADR_SY1: 	begin
					if (avalon_slave_byteenable[0]) sy1_r[7:0]	<= avalon_slave_writedata[7:0];
					if (avalon_slave_byteenable[1]) sy1_r[15:8]	<= avalon_slave_writedata[15:8];
					end
		`ADR_SW	: 	begin
					if (avalon_slave_byteenable[0]) sw_r[7:0]	<= avalon_slave_writedata[7:0];
					if (avalon_slave_byteenable[1]) sw_r[15:8]	<= avalon_slave_writedata[15:8];
					end
		`ADR_SH	: 	begin
					if (avalon_slave_byteenable[0]) sh_r[7:0]	<= avalon_slave_writedata[7:0];
					if (avalon_slave_byteenable[1]) sh_r[15:8]	<= avalon_slave_writedata[15:8];
					end
		`ADR_W	: 	begin
					if (avalon_slave_byteenable[0]) w_r[7:0]	<= avalon_slave_writedata[7:0];
					if (avalon_slave_byteenable[1]) w_r[15:8]	<= avalon_slave_writedata[15:8];
					end
		`ADR_H	: 	begin
					if (avalon_slave_byteenable[0]) h_r[7:0]	<= avalon_slave_writedata[7:0];
					if (avalon_slave_byteenable[1]) h_r[15:8]	<= avalon_slave_writedata[15:8];
					end
		`ADR_SRC: 	begin
					if (avalon_slave_byteenable[0]) src_start_r[7:0]	<= avalon_slave_writedata[7:0];
					if (avalon_slave_byteenable[1]) src_start_r[15:8]	<= avalon_slave_writedata[15:8];
					if (avalon_slave_byteenable[2]) src_start_r[23:16]	<= avalon_slave_writedata[23:16];
					if (avalon_slave_byteenable[3]) src_start_r[31:24]	<= avalon_slave_writedata[31:24];
					end
	endcase

// avalon read
always @ (*)
	case (avalon_slave_address)
		`ADR_CTL	: avalon_slave_readdata = {24'b0,ctl_r};
		`ADR_FLAGS	: avalon_slave_readdata = {24'b0,flags_r};
		`ADR_CMDDAT	: avalon_slave_readdata = {24'b0,cmd_dat_r};
		`ADR_SX1	: avalon_slave_readdata = {16'b0,sx1_r};
		`ADR_SY1	: avalon_slave_readdata = {16'b0,sy1_r};
		`ADR_SW		: avalon_slave_readdata = {16'b0,sw_r};
		`ADR_SH		: avalon_slave_readdata = {16'b0,sh_r};
		`ADR_W		: avalon_slave_readdata = {16'b0,w_r};
		`ADR_H		: avalon_slave_readdata = {16'b0,h_r};
		`ADR_SRC	: avalon_slave_readdata = {src_start_r};
		default: avalon_slave_readdata = 32'b0;
	endcase

// =============================== main ======================================

// ctl
`define CTL_START 0
`define CTL_DMA_MODE 1 // 0 - single, 1 - dma
`define CTL_RST 2
`define CTL_DC 3
`define CTL_UD 4


assign ctl_start = ctl_r[`CTL_START];
assign ctl_dma_mode = ctl_r[`CTL_DMA_MODE];
assign ctl_dc = ctl_r[`CTL_DC];
assign ctl_ud = ctl_r[`CTL_UD];
assign ctl_rst = ctl_r[`CTL_RST];

//assign f_error = flags_r[`F_ERROR];
//assign f_single_complete = flags_r[`F_SINGLE_COMPLETE];

always @(posedge clk or negedge reset_n)
	if (!reset_n)
		ctl_r <= 8'b0;
	else if (avalon_slave_chipselect && avalon_slave_write && avalon_slave_address == `ADR_CTL && avalon_slave_byteenable[0])
		ctl_r	<= avalon_slave_writedata[7:0];
	else
		if (ctl_start) 
			ctl_r[`CTL_START] <= 1'b0;

// flags
`define F_COMPLETE 0
`define F_ERROR 1

always @(posedge clk or negedge reset_n)
	if (!reset_n)
		flags_r <= 8'b0;
	else
		if (ctl_start) 
			flags_r <= 8'b0;

// ======================== display interface ================================

// domain crossing fifo (clk -> disp_clk) 
wire [7:0] disp_fifo_q;
wire disp_fifo_empty;
wire disp_fifo_readrq;

dc_fifo_8x8 disp_fifo (
	.aclr(~reset_n),
	.data,
	.rdclk(disp_clk),
	.rdreq(disp_fifo_readrq),
	.wrclk(clk),
	.wrreq,
	.q(disp_fifo_q),
	.rdempty(disp_fifo_empty),
	.wrfull);

assign disp_fifo_readrq = ~disp_fifo_empty;
assign disp_go = ~disp_clk & ~disp_fifo_empty;

assign disp_cs_n = ~disp_go;
assign disp_dc_n = ~ctl_dc; // need signal ?
assign disp_wr_n = ~disp_go;
assign disp_rd_n = 1'b1; // never read
assign disp_ud   = ctl_ud; // need signal ?
assign disp_rst_n = ctl_rst; // need signal ?
assign disp_d    = disp_fifo_q;

endmodule
