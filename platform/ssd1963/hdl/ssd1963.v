module ssd1963 (
	clk,
	reset_n,
	
	avalon_slave_address,
	avalon_slave_readdata,
	avalon_slave_writedata,
	avalon_slave_write,
	avalon_slave_read,
	avalon_slave_chipselect,
	avalon_slave_byteenable,
	
	cs_n,
	wr_n,
	rd_n,
	d
	);


	
input clk;
input reset_n;
input [3:0] avalon_slave_address;
input [3:0] avalon_slave_byteenable;
input avalon_slave_chipselect;
input avalon_slave_write;

input [31:0] avalon_slave_writedata;
output [31:0] avalon_slave_readdata;
input avalon_slave_read;

output cs_n;
output wr_n;
output rd_n;
inout [7:0] d;

wire go = avalon_slave_chipselect && (avalon_slave_write || avalon_slave_read) && avalon_slave_byteenable[0];
wire go_read = go && avalon_slave_read;
wire go_write = go && avalon_slave_write;

assign cs_n = ~go;
assign wr_n = ~go_write;
assign rd_n = ~go_read;
assign d 	= go_write ? avalon_slave_writedata[7:0] : 8'bZ;
assign avalon_slave_readdata[31:0] = go_read ? {24'b0,d[7:0]} : 31'b0;

endmodule
