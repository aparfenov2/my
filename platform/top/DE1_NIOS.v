
module DE1_NIOS
	(
		////////////////////	Clock Input	 	////////////////////	 
		CLOCK_24,						//	24 MHz
		CLOCK_27,						//	27 MHz
		CLOCK_50,						//	50 MHz
		EXT_CLOCK,						//	External Clock
		////////////////////	Push Button		////////////////////
		KEY,							//	Pushbutton[3:0]
		////////////////////	DPDT Switch		////////////////////
		SW,								//	Toggle Switch[9:0]
		////////////////////	7-SEG Dispaly	////////////////////
		HEX0,							//	Seven Segment Digit 0
		HEX1,							//	Seven Segment Digit 1
		HEX2,							//	Seven Segment Digit 2
		HEX3,							//	Seven Segment Digit 3
		////////////////////////	LED		////////////////////////
		LEDG,							//	LED Green[7:0]
		LEDR,							//	LED Red[9:0]
		////////////////////////	UART	////////////////////////
		UART_TXD,						//	UART Transmitter
		UART_RXD,						//	UART Receiver
		/////////////////////	SDRAM Interface		////////////////
		DRAM_DQ,						//	SDRAM Data bus 16 Bits
		DRAM_ADDR,						//	SDRAM Address bus 12 Bits
		DRAM_LDQM,						//	SDRAM Low-byte Data Mask 
		DRAM_UDQM,						//	SDRAM High-byte Data Mask
		DRAM_WE_N,						//	SDRAM Write Enable
		DRAM_CAS_N,						//	SDRAM Column Address Strobe
		DRAM_RAS_N,						//	SDRAM Row Address Strobe
		DRAM_CS_N,						//	SDRAM Chip Select
		DRAM_BA_0,						//	SDRAM Bank Address 0
		DRAM_BA_1,						//	SDRAM Bank Address 0
		DRAM_CLK,						//	SDRAM Clock
		DRAM_CKE,						//	SDRAM Clock Enable
		////////////////////	Flash Interface		////////////////
		FL_DQ,							//	FLASH Data bus 8 Bits
		FL_ADDR,						//	FLASH Address bus 22 Bits
		FL_WE_N,						//	FLASH Write Enable
		FL_RST_N,						//	FLASH Reset
		FL_OE_N,						//	FLASH Output Enable
		FL_CE_N,						//	FLASH Chip Enable
		////////////////////	SRAM Interface		////////////////
		SRAM_DQ,						//	SRAM Data bus 16 Bits
		SRAM_ADDR,						//	SRAM Address bus 18 Bits
		SRAM_UB_N,						//	SRAM High-byte Data Mask 
		SRAM_LB_N,						//	SRAM Low-byte Data Mask 
		SRAM_WE_N,						//	SRAM Write Enable
		SRAM_CE_N,						//	SRAM Chip Enable
		SRAM_OE_N,						//	SRAM Output Enable
		////////////////////	SD_Card Interface	////////////////
		SD_DAT,							//	SD Card Data
		SD_DAT3,						//	SD Card Data 3
		SD_CMD,							//	SD Card Command Signal
		SD_CLK,							//	SD Card Clock
		////////////////////	USB JTAG link	////////////////////
		TDI,  							// CPLD -> FPGA (data in)
		TCK,  							// CPLD -> FPGA (clk)
		TCS,  							// CPLD -> FPGA (CS)
	    TDO,  							// FPGA -> CPLD (data out)
		////////////////////	I2C		////////////////////////////
		I2C_SDAT,						//	I2C Data
		I2C_SCLK,						//	I2C Clock
		////////////////////	PS2		////////////////////////////
		PS2_DAT,						//	PS2 Data
		PS2_CLK,						//	PS2 Clock
		////////////////////	VGA		////////////////////////////
		VGA_HS,							//	VGA H_SYNC
		VGA_VS,							//	VGA V_SYNC
		VGA_R,   						//	VGA Red[3:0]
		VGA_G,	 						//	VGA Green[3:0]
		VGA_B,  						//	VGA Blue[3:0]
		////////////////	Audio CODEC		////////////////////////
		AUD_ADCLRCK,					//	Audio CODEC ADC LR Clock
		AUD_ADCDAT,						//	Audio CODEC ADC Data
		AUD_DACLRCK,					//	Audio CODEC DAC LR Clock
		AUD_DACDAT,						//	Audio CODEC DAC Data
		AUD_BCLK,						//	Audio CODEC Bit-Stream Clock
		AUD_XCK,						//	Audio CODEC Chip Clock
		////////////////////	GPIO	////////////////////////////
		GPIO_0,							//	GPIO Connection 0
		GPIO_1							//	GPIO Connection 1
	);

////////////////////////	Clock Input	 	////////////////////////
input	[1:0]	CLOCK_24;				//	24 MHz
input	[1:0]	CLOCK_27;				//	27 MHz
input			CLOCK_50;				//	50 MHz
input			EXT_CLOCK;				//	External Clock
////////////////////////	Push Button		////////////////////////
input	[3:0]	KEY;					//	Pushbutton[3:0]
////////////////////////	DPDT Switch		////////////////////////
input	[9:0]	SW;						//	Toggle Switch[9:0]
////////////////////////	7-SEG Dispaly	////////////////////////
output	[6:0]	HEX0;					//	Seven Segment Digit 0
output	[6:0]	HEX1;					//	Seven Segment Digit 1
output	[6:0]	HEX2;					//	Seven Segment Digit 2
output	[6:0]	HEX3;					//	Seven Segment Digit 3
////////////////////////////	LED		////////////////////////////
output	[7:0]	LEDG;					//	LED Green[7:0]
output	[9:0]	LEDR;					//	LED Red[9:0]
////////////////////////////	UART	////////////////////////////
output			UART_TXD;				//	UART Transmitter
input			UART_RXD;				//	UART Receiver
///////////////////////		SDRAM Interface	////////////////////////
inout	[15:0]	DRAM_DQ;				//	SDRAM Data bus 16 Bits
output	[11:0]	DRAM_ADDR;				//	SDRAM Address bus 12 Bits
output			DRAM_LDQM;				//	SDRAM Low-byte Data Mask 
output			DRAM_UDQM;				//	SDRAM High-byte Data Mask
output			DRAM_WE_N;				//	SDRAM Write Enable
output			DRAM_CAS_N;				//	SDRAM Column Address Strobe
output			DRAM_RAS_N;				//	SDRAM Row Address Strobe
output			DRAM_CS_N;				//	SDRAM Chip Select
output			DRAM_BA_0;				//	SDRAM Bank Address 0
output			DRAM_BA_1;				//	SDRAM Bank Address 0
output			DRAM_CLK;				//	SDRAM Clock
output			DRAM_CKE;				//	SDRAM Clock Enable
////////////////////////	Flash Interface	////////////////////////
inout	[7:0]	FL_DQ;					//	FLASH Data bus 8 Bits
output	[21:0]	FL_ADDR;				//	FLASH Address bus 22 Bits
output			FL_WE_N;				//	FLASH Write Enable
output			FL_RST_N;				//	FLASH Reset
output			FL_OE_N;				//	FLASH Output Enable
output			FL_CE_N;				//	FLASH Chip Enable
////////////////////////	SRAM Interface	////////////////////////
inout	[15:0]	SRAM_DQ;				//	SRAM Data bus 16 Bits
output	[17:0]	SRAM_ADDR;				//	SRAM Address bus 18 Bits
output			SRAM_UB_N;				//	SRAM High-byte Data Mask 
output			SRAM_LB_N;				//	SRAM Low-byte Data Mask 
output			SRAM_WE_N;				//	SRAM Write Enable
output			SRAM_CE_N;				//	SRAM Chip Enable
output			SRAM_OE_N;				//	SRAM Output Enable
////////////////////	SD Card Interface	////////////////////////
inout			SD_DAT;					//	SD Card Data
inout			SD_DAT3;				//	SD Card Data 3
inout			SD_CMD;					//	SD Card Command Signal
output			SD_CLK;					//	SD Card Clock
////////////////////////	I2C		////////////////////////////////
inout			I2C_SDAT;				//	I2C Data
output			I2C_SCLK;				//	I2C Clock
////////////////////////	PS2		////////////////////////////////
input		 	PS2_DAT;				//	PS2 Data
input			PS2_CLK;				//	PS2 Clock
////////////////////	USB JTAG link	////////////////////////////
input  			TDI;					// CPLD -> FPGA (data in)
input  			TCK;					// CPLD -> FPGA (clk)
input  			TCS;					// CPLD -> FPGA (CS)
output 			TDO;					// FPGA -> CPLD (data out)
////////////////////////	VGA			////////////////////////////
output			VGA_HS;					//	VGA H_SYNC
output			VGA_VS;					//	VGA V_SYNC
output	[3:0]	VGA_R;   				//	VGA Red[3:0]
output	[3:0]	VGA_G;	 				//	VGA Green[3:0]
output	[3:0]	VGA_B;   				//	VGA Blue[3:0]
////////////////////	Audio CODEC		////////////////////////////
inout			AUD_ADCLRCK;			//	Audio CODEC ADC LR Clock
input			AUD_ADCDAT;				//	Audio CODEC ADC Data
inout			AUD_DACLRCK;			//	Audio CODEC DAC LR Clock
output			AUD_DACDAT;				//	Audio CODEC DAC Data
inout			AUD_BCLK;				//	Audio CODEC Bit-Stream Clock
output			AUD_XCK;				//	Audio CODEC Chip Clock
////////////////////////	GPIO	////////////////////////////////
inout	[35:0]	GPIO_0;					//	GPIO Connection 0
inout	[35:0]	GPIO_1;					//	GPIO Connection 1

// not connected
//assign   LEDG[7:0]   = 8'b0;
assign   LEDR[9:0]   = 10'b0;
assign   HEX0[6:0]   = 7'b0;
assign   HEX1[6:0]   = 7'b0;
assign   HEX2[6:0]   = 7'b0;
assign   HEX3[6:0]   = 7'b0;
assign	FL_RST_N	   = 1'b1;
assign   FL_CE_N     = 1'b1;
assign   FL_ADDR     = 22'b0;
assign   FL_DQ       = 8'bz;
assign   FL_OE_N     = 1'b1;
assign   FL_WE_N     = 1'b1;
assign   UART_TXD    = 1'b1;
assign   SRAM_ADDR   = 18'b0;
assign   SRAM_CE_N   = 1'b1;
assign   SRAM_DQ     = 16'bz;
assign   SRAM_LB_N   = 1'b1;
assign   SRAM_OE_N   = 1'b1;
assign   SRAM_UB_N   = 1'b1;
assign   SRAM_WE_N   = 1'b1;
assign   VGA_R[3:0]  = 4'b0;
assign   VGA_G[3:0]  = 4'b0;
assign   VGA_B[3:0]  = 4'b0;
assign   TDO         = 1'b0;
assign   VGA_HS      = 1'b0;
assign   VGA_VS      = 1'b0;
assign	SD_DAT		= 1'bz;
assign   SD_CLK      = 1'b0;
assign   I2C_SCLK    = 1'b0;
assign	I2C_SDAT	   = 1'bz;
assign	AUD_ADCLRCK	= 1'bz;
assign	AUD_DACLRCK	= 1'bz;
assign	AUD_BCLK	   = 1'bz;
assign	AUD_DACDAT	= 1'b0;
assign	AUD_XCK	   = 1'b0;

// ssd1963
wire ssd1963_cs_n;
wire [7:0] ssd1963_d;
wire ssd1963_rd_n;
wire ssd1963_wr_n;

wire _ssd1963_cs_n;
wire [7:0] _ssd1963_d;
wire _ssd1963_rd_n;
wire _ssd1963_wr_n;

wire ssd1963_dc_n;
wire ssd1963_rst_n;
wire ssd1963_rl;
wire ssd1963_ud;
wire ssd1963_mode;

wire [3:0] keypad_row;
wire [4:0] keypad_col;

wire [7:0] pio_0_out;
wire [7:0] pio_1_out;
wire [15:0] pio_key_in;
wire [31:0] pio_led_out;
// common ctl
assign ssd1963_dc_n = pio_0_out[0];
assign ssd1963_rst_n = pio_0_out[1];
assign ssd1963_rl = pio_0_out[2];
assign ssd1963_ud = pio_0_out[3];

// keypad
assign pio_key_in[3:0] = 4'b0;
assign pio_key_in[8:4] = keypad_col[4:0];
assign pio_key_in[12:9] = KEY[3:0];
assign pio_key_in[15:13] = 3'bz;

assign keypad_row[3:0] = pio_led_out[11:8];
// leds
assign LEDG[6:0] = pio_led_out[6:0];
assign LEDG[7] = ssd1963_mode;

// PIO mode
assign ssd1963_wr_n = ssd1963_mode ? _ssd1963_wr_n : pio_0_out[4];
assign ssd1963_rd_n = ssd1963_mode ? _ssd1963_rd_n : pio_0_out[5];
assign ssd1963_cs_n = ssd1963_mode ? _ssd1963_cs_n : pio_0_out[6];
assign ssd1963_mode = pio_0_out[7];

assign ssd1963_d[7:0] = ssd1963_mode ? _ssd1963_d[7:0] : pio_1_out[7:0];

// HW mode
/*
assign ssd1963_wr_n = _ssd1963_wr_n;
assign ssd1963_rd_n = _ssd1963_rd_n;
assign ssd1963_cs_n = _ssd1963_cs_n;
assign ssd1963_d[7:0] = _ssd1963_d[7:0];
*/
wire [35:0] DISP_GPIO;
wire [35:0] DISP_GPIO_IN;
// GPIO assignment
assign keypad_col[4:0] = DISP_GPIO_IN[35:31];
assign DISP_GPIO[35:31] = 5'bz;
assign DISP_GPIO[30:27] = keypad_row[3:0];
assign DISP_GPIO[26] = 1'bz;
assign DISP_GPIO[25] = ssd1963_dc_n;
assign DISP_GPIO[24] = ssd1963_wr_n;
assign DISP_GPIO[23] = ssd1963_rd_n;
assign DISP_GPIO[22] = ssd1963_d[0];
assign DISP_GPIO[21] = ssd1963_d[1];
assign DISP_GPIO[20] = ssd1963_d[2];
assign DISP_GPIO[19] = ssd1963_d[3];
assign DISP_GPIO[18] = ssd1963_d[4];
assign DISP_GPIO[17] = ssd1963_d[5];
assign DISP_GPIO[16] = ssd1963_d[6];
assign DISP_GPIO[15] = ssd1963_d[7];
assign DISP_GPIO[14] = ssd1963_cs_n;
assign DISP_GPIO[13] = ssd1963_rst_n;
assign DISP_GPIO[12] = ssd1963_rl;
assign DISP_GPIO[11] = ssd1963_ud;
assign DISP_GPIO[10:0] = 11'bz;

//	All inout port turn to tri-state

assign	GPIO_0[35:0] =	DISP_GPIO[35:0];
assign   DISP_GPIO_IN[35:0] = GPIO_0[35:0];
assign	GPIO_1[35:0] =	DISP_GPIO[35:0];

wire	CPU_CLK;
wire	CPU_RESETn;
wire  _CPU_RESETn;

Reset_Delay	delay1	(.iRST(1'b1),.iCLK(CLOCK_50),.oRESET(_CPU_RESETn));

assign CPU_RESETn = ~((~_CPU_RESETn) | (~KEY[2]));

SDRAM_PLL 	PLL1	(.inclk0(CLOCK_50),.c0(DRAM_CLK),.c1(CPU_CLK));

my_sopc 	u0	(
                   .clk_0(CPU_CLK),
                   .reset_n(CPU_RESETn),
						 
                  // the_sdram_0
                   .zs_addr_from_the_sdram_0(DRAM_ADDR),
                   .zs_ba_from_the_sdram_0({DRAM_BA_1,DRAM_BA_0}),
                   .zs_cas_n_from_the_sdram_0(DRAM_CAS_N),
                   .zs_cke_from_the_sdram_0(DRAM_CKE),
                   .zs_cs_n_from_the_sdram_0(DRAM_CS_N),
                   .zs_dq_to_and_from_the_sdram_0(DRAM_DQ),
                   .zs_dqm_from_the_sdram_0({DRAM_UDQM,DRAM_LDQM}),
                   .zs_ras_n_from_the_sdram_0(DRAM_RAS_N),
                   .zs_we_n_from_the_sdram_0(DRAM_WE_N),
						 
                  // the_ssd1963_0
                   .cs_n_from_the_ssd1963_0(_ssd1963_cs_n),
                   .d_to_and_from_the_ssd1963_0(_ssd1963_d),
                   .rd_n_from_the_ssd1963_0(_ssd1963_rd_n),
                   .wr_n_from_the_ssd1963_0(_ssd1963_wr_n),
						 
						 // pio_0
						 .out_port_from_the_pio_0(pio_0_out),
						 // pio_1
						 .bidir_port_to_and_from_the_pio_1(pio_1_out),
						 // pio_2
						 .in_port_to_the_pio_key(pio_key_in),
						 // pio_2
						 .out_port_from_the_pio_led(pio_led_out) 
                
/*
                  // the_KEY
                   .in_port_to_the_KEY(KEY),

                  // the_LEDG
                   .out_port_from_the_LEDG(LEDG),

                  // the_LEDR
                   .out_port_from_the_LEDR(LEDR),

                  // the_SEG7
                   .oSEG0_from_the_SEG7(HEX0),
                   .oSEG1_from_the_SEG7(HEX1),
                   .oSEG2_from_the_SEG7(HEX2),
                   .oSEG3_from_the_SEG7(HEX3),

                  // the_Switch
                   .in_port_to_the_Switch(SW),
						 
                  // the_tri_state_bridge_0_avalon_slave
                   .select_n_to_the_cfi_flash_0(FL_CE_N),
                   .tri_state_bridge_0_address(FL_ADDR),
                   .tri_state_bridge_0_data(FL_DQ),
                   .tri_state_bridge_0_readn(FL_OE_N),
                   .write_n_to_the_cfi_flash_0(FL_WE_N),

                  // the_uart_0
                   .rxd_to_the_uart_0(UART_RXD),
                   .txd_from_the_uart_0(UART_TXD),

                    // the_sram_0
                   .SRAM_ADDR_from_the_sram_0(SRAM_ADDR),
                   .SRAM_CE_N_from_the_sram_0(SRAM_CE_N),
                   .SRAM_DQ_to_and_from_the_sram_0(SRAM_DQ),
                   .SRAM_LB_N_from_the_sram_0(SRAM_LB_N),
                   .SRAM_OE_N_from_the_sram_0(SRAM_OE_N),
                   .SRAM_UB_N_from_the_sram_0(SRAM_UB_N),
                   .SRAM_WE_N_from_the_sram_0(SRAM_WE_N),
*/						 
				);

endmodule