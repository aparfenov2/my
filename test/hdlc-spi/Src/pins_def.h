#ifndef pins_defH
#define pins_defH

#include "types.h"

// all port B except PIN_RL
// port B 32 bit GPIO32..GPIO63

#define PIN_DC GPIO57 // bit 25
#define PIN_WR GPIO58 // 26
#define PIN_RD GPIO59 // 27
#define PIN_DB0 GPIO48 // 16
#define PIN_DB1 GPIO49 // 17
#define PIN_DB2 GPIO50 // 18
#define PIN_DB3 GPIO51 // 19
#define PIN_DB4 GPIO52 // 20
#define PIN_DB5 GPIO53 // 21
#define PIN_DB6 GPIO54 // 22
#define PIN_DB7 GPIO55 // 23
#define PIN_CS GPIO60  // 28
#define PIN_RST GPIO61 // 29
#define PIN_UD GPIO36 // 4

#define PIN_RL GPIO30 // port A


typedef union {
	u32 all;
	struct {          // bits   description
	   u16 GPIO32:1;           // 0      GPIO32
	   u16 GPIO33:1;           // 1      GPIO33
	   u16 GPIO34:1;           // 2      GPIO34
	   u16 GPIO35:1;           // 3      GPIO35
	   u16 GPIO36:1;           // 4      GPIO36
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
	   u16 dat:8;           // 16     GPIO48
	   u16 GPIO56:1;           // 24     GPIO56
	   u16 GPIO57:1;           // 25     GPIO57
	   u16 GPIO58:1;           // 26     GPIO58
	   u16 GPIO59:1;           // 27     GPIO59
	   u16 GPIO60:1;           // 28     GPIO60
	   u16 GPIO61:1;           // 29     GPIO61
	   u16 GPIO62:1;           // 30     GPIO62
	   u16 GPIO63:1;           // 31     GPIO63
	} bits;
} port_b_t;




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

