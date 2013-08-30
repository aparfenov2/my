#ifndef pins_defH
#define pins_defH

#include "types.h"


//typedef union {
//	u32 all;
//	struct {          // bits   description
//	   u16 GPIO0:1;           // 0      GPIO0
//	   u16 GPIO1:1;           // 1      GPIO1
//	   u16 GPIO2:1;           // 2      GPIO2
//	   u16 GPIO3:1;           // 3      GPIO3
//	   u16 GPIO4:1;           // 4      GPIO4
//	   u16 GPIO5:1;           // 5      GPIO5
//	   u16 GPIO6:1;           // 6      GPIO6
//	   u16 GPIO7:1;           // 7      GPIO7
//	   u16 GPIO8:1;           // 8      GPIO8
//	   u16 GPIO9:1;           // 9      GPIO9
//	   u16 GPIO10:1;           // 10     GPIO10
//	   u16 GPIO11:1;           // 11     GPIO11
//	   u16 GPIO12:1;           // 12     GPIO12
//	   u16 GPIO13:1;           // 13     GPIO13
//	   u16 GPIO14:1;           // 14     GPIO14
//	   u16 GPIO15:1;           // 15     GPIO15
//	   u16 GPIO16:1;           // 16     GPIO16
//	   u16 GPIO17:1;           // 17     GPIO17
//	   u16 GPIO18:1;           // 18     GPIO18
//	   u16 GPIO19:1;           // 19     GPIO19
//	   u16 GPIO20:1;           // 20     GPIO20
//	   u16 GPIO21:1;           // 21     GPIO21
//	   u16 GPIO22:1;           // 22     GPIO22
//	   u16 GPIO23:1;           // 23     GPIO23
//	   u16 GPIO24:1;           // 24     GPIO24
//	   u16 GPIO25:1;           // 25     GPIO25
//	   u16 GPIO26:1;           // 26     GPIO26
//	   u16 GPIO27:1;           // 27     GPIO27
//	   u16 GPIO28:1;           // 28     GPIO28
//	   u16 GPIO29:1;           // 29     GPIO29
//	   u16 GPIO30:1;           // 30     GPIO30
//	   u16 GPIO31:1;           // 31     GPIO31
//	} bits;
//} port_a_t;


typedef union {
	u32 all;
	struct {          // bits   description
	   u16 GPIO0:1;           // 0      GPIO0
	   u16 GPIO1:1;           // 1      GPIO1
	   u16 GPIO2:1;           // 2      GPIO2
	   u16 GPIO3:1;           // 3      GPIO3
	   u16 GPIO4:1;           // 4      GPIO4
	   u16 BL_E:1;           // 5      GPIO5
	   u16 GPIO6:1;           // 6      GPIO6
	   u16 GPIO7:1;           // 7      GPIO7
	   u16 GPIO8:1;           // 8      GPIO8
	   u16 GPIO9:1;           // 9      GPIO9
	   u16 GPIO10:1;           // 10     GPIO10
	   u16 GPIO11:1;           // 11     GPIO11
	   u16 GPIO12:1;           // 12     GPIO12
	   u16 GPIO13:1;           // 13     GPIO13
	   u16 GPIO14:1;           // 14     GPIO14
	   u16 CS:1;           // 15     GPIO15
	   u16 GPIO16:1;           // 16     GPIO16
	   u16 RST:1;           // 17     GPIO17
	   u16 DISP:1;           // 18     GPIO18
	   u16 GPIO19:1;           // 19     GPIO19
	   u16 GPIO20:1;           // 20     GPIO20
	   u16 GPIO21:1;           // 21     GPIO21
	   u16 WR:1;           // 22     GPIO22
	   u16 GPIO23:1;           // 23     GPIO23
	   u16 GPIO24:1;           // 24     GPIO24
	   u16 GPIO25:1;           // 25     GPIO25
	   u16 GPIO26:1;           // 26     GPIO26
	   u16 GPIO27:1;           // 27     GPIO27
	   u16 RD:1;           // 28     GPIO28
	   u16 GPIO29:1;           // 29     GPIO29
	   u16 DC:1;           // 30     GPIO30, RS
	   u16 GPIO31:1;           // 31     GPIO31
	} bits;
} port_a_t;


typedef union {
	u32 all;
	struct {          // bits   description
	   u16 D0:1;           // 0      GPIO32
	   u16 GPIO33:1;           // 1      GPIO33
	   u16 D1_3:3;           // 2      GPIO34
//	   u16 D2:1;           // 3      GPIO35
//	   u16 D3:1;           // 4      GPIO36
	   u16 GPIO37:1;           // 5      GPIO37
	   u16 GPIO38:1;           // 6      GPIO38
	   u16 GPIO39:1;           // 7      GPIO39
	   u16 GPIO40:1;           // 8      GPIO40
	   u16 GPIO41:1;           // 9      GPIO41
	   u16 GPIO42:1;           // 10     GPIO42
	   u16 GPIO43:1;           // 11     GPIO43
	   u16 GPIO44:1;           // 12     GPIO44
	   u16 GPIO45:1;           // 13     GPIO45
	   u16 GPIO46:1;           // 14     GPIO46
	   u16 GPIO47:1;           // 15     GPIO47
	   u16 D4_11:8;           // 16     GPIO48
//	   u16 D5:1;           // 17     GPIO49
//	   u16 D6:1;           // 18     GPIO50
//	   u16 D7:1;           // 19     GPIO51
//	   u16 D8:1;           // 20     GPIO52
//	   u16 D9:1;           // 21     GPIO53
//	   u16 D10:1;           // 22     GPIO54
//	   u16 D11:1;           // 23     GPIO55
	   u16 GPIO56:1;           // 24     GPIO56
	   u16 D12_15:4;           // 25     GPIO57
//	   u16 D13:1;           // 26     GPIO58
//	   u16 D14:1;           // 27     GPIO59
//	   u16 D15:1;           // 28     GPIO60
	   u16 GPIO61:1;           // 29     GPIO61
	   u16 GPIO62:1;           // 30     GPIO62
	   u16 GPIO63:1;           // 31     GPIO63
	} bits;
} port_b_t;

typedef union {
	u16 all;
	struct {          // bits   description
	   u16 D0:1;           // 0      GPIO32
	   u16 D1_3:3;           // 2      GPIO34
	   u16 D4_11:8;           // 16     GPIO48
	   u16 D12_15:4;           // 25     GPIO57
	} bits;
} bus_data_t;




// -------------------- ILS inherited -----------------

#define PIN_CMTR_CLK_O  GPIO0 // IOU_DDS
#define PIN_CMTR_DATA_O GPIO1 // Out1
#define PIN_CMTR_WR_O   GPIO2 // Out2
#define PIN_CMTR_DIR    GPIO3 // Out3
#define PIN_INP_CMTR    GPIO4 // Out5

//#define PIN_A0          GPIO5
//#define PIN_A1          GPIO6
//#define PIN_A2          GPIO7

#define PIN_LG          GPIO6
#define PIN__LG         GPIO8
#define PIN_REC_SYNC    GPIO10 // GPIOA15
#define PIN__DAC_LD     GPIO12 // GPIOB2
#define PIN_HET_STATE   GPIO13 // Het_OK
#define PIN_DDS_RST     GPIO14
#define PIN_M25_MOUT    GPIO24
#define PIN_M25_MIN     GPIO25
#define PIN_M25_CLK     GPIO26
#define PIN__M25_CS     GPIO27
#define PIN_GREEN       GPIO32
#define PIN_RED         GPIO33
#define PIN_PIN_NARR    GPIO49
#define PIN_PIN_WIDE    GPIO51
#define PIN_SPI_MO      GPIO16
#define PIN_SPI_MI      GPIO17
#define PIN_SPI_CLK     GPIO18
#define PIN_SPI_CS      GPIO19 // SPI_CS
#define PIN_SCIA_TX     GPIO11
#define PIN_SCIA_RX     GPIO9
#define PIN_SCIB_TX     GPIO62
#define PIN_SCIB_RX     GPIO63
#define PIN_MCLK        GPIO7
#define PIN_MFSR        GPIO5
#define PIN_MDR         GPIO21
#define PIN_SPI_D0      GPIO22
#define PIN_SPI_D1      GPIO23
#define PIN_SPI_D2      GPIO20


#endif

