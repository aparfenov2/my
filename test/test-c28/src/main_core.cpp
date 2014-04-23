
//#include <io.h>
//#include "system.h"
//#include "altera_avalon_pio_regs.h"
//#include "ssd1963.h"
#include "ssd1963drv.h"

#include "surface.h"
#include "assert_impl.h"
#include "bmp_math.h"
#include "app_events.h"
#include "screen_1.h"
#include "resources.h"
#include "widgets.h"

//#include "lena_320.h"
//#include "ARIALNI.h"

#include "ttcache.h"
//#include "ttcache_dat.h"

//#include "host_emu.h"
extern "C" {
#include "hdlc.h"
}
#include "packetizer.h"
#include "serializer.h"

#include "uart_drv.h"
#include "devices.h"
#include "Spi.h"

#include <stdio.h>




using namespace myvi;


extern "C" char* my_itoa(int i, char b[]);

class logger_impl_t : public logger_t {
public:
public:


    virtual logger_t& operator << (s32 v) OVERRIDE {
    	char ls[32];
    	my_itoa(v,ls);
    	printf(ls);
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl)
			printf("\n");
		else
			printf(v);
        return *this;
    }

};

logger_impl_t logger_impl;

logger_t *logger_t::instance = &logger_impl;


//#define COL_a 0
//#define COL_b 1
//#define COL_c 2
//#define COL_d 3
//#define COL_e 4

//key_t::key_t getAppKey() {
//	key_t::key_t ret = (key_t::key_t)0;
//	for (int row=0; row<4; row++) {
//		// write row
//		u32 led = 0; //IORD_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE);
//		u8 row_msk = (1 << row);
//		// clear other rows
//		u32 led_msk = (((row_msk) & 0x0f) << 8);
//		led &= ~led_msk;
//		// set selected row
//		led_msk = (((~row_msk) & 0x0f) << 8);
//		led |= led_msk;
////		IOWR_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE, led);
//		// read col
//		u32 key = 0; //~IORD_ALTERA_AVALON_PIO_DATA(PIO_KEY_BASE);
//		u8 col_res = (key >> 4);
//		int rowi = row+1;
//		if (rowi == 1 && (col_res & (1 << COL_a))) ret = key_t::K_F1;
//		if (rowi == 1 && (col_res & (1 << COL_b))) ret = key_t::K_F2;
//		if (rowi == 1 && (col_res & (1 << COL_c))) ret = key_t::K_F3;
//		if (rowi == 1 && (col_res & (1 << COL_d))) ret = key_t::K_F4;
//		if (rowi == 3 && (col_res & (1 << COL_b))) ret = key_t::K_LEFT;
//		if (rowi == 3 && (col_res & (1 << COL_c))) ret = key_t::K_DOWN;
//		if (rowi == 3 && (col_res & (1 << COL_d))) ret = key_t::K_RIGHT;
//		if (rowi == 4 && (col_res & (1 << COL_c))) ret = key_t::K_UP;
//		if (rowi == 4 && (col_res & (1 << COL_e))) ret = key_t::K_ENTER;
//		if (rowi == 4 && (col_res & (1 << COL_a))) ret = key_t::K_ESC;
//	}
//	return ret;
//}

//#define BW 320
//#define BH 240
//surface_24bpp_t bk_surface(BW,BH,BMP_GET_SIZE(BW,BH,24), lena_320);


//extern "C" char hw_mode;
extern resources_t res;

ssd1963drv_t drv1;
screen_1_t screen1;
//host_emu_t emu;
myvi::packetizer_impl_t pkz;
myvi::serializer_t ser;
uart_drv_t	uart;
Spi spi;
FRAM fram;
FlashDev flash;


void drawScene(surface_t &s1) {
	static key_t::key_t last_key = (key_t::key_t)0;
	key_t::key_t key = getAppKey();
	if (key != last_key) {
		last_key = key;
		globals::modal_overlay.key_event(key);
	}
	if (rasterizer_t::render(&globals::modal_overlay, s1))
		s1.copy_to(0,0,-1,-1,0,0,drv1);
}

void rs485_put_char(char data) {
	uart.write(data);
}

// к нам пришел пакет от удаленной системы
void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	pkz.receive((const u8*)buffer,bytes_received);
}

void my_hdlc_tx_frame(const u8 *buffer, u16 bytes_to_send) {
	hdlc_tx_frame((const u8_t*)buffer, bytes_to_send);
}

extern interrupt void Scib_Rx_Int (void);
extern interrupt void Scic_Rx_Int (void);

void init_pie_table() {
    EALLOW;
    PieVectTable.SCIRXINTB = (PINT) &Scib_Rx_Int;
//    PieVectTable.SCITXINTB = (PINT) &Scib_Tx_Int;
    PieVectTable.SCIRXINTC = (PINT) &Scic_Rx_Int;
//    PieVectTable.SCITXINTC = (PINT) &Scic_Tx_Int;
    EDIS;

    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;	// RXBINT
//    PieCtrlRegs.PIEIER9.bit.INTx4 = 1;	// TXBINT
    PieCtrlRegs.PIEIER8.bit.INTx5 = 1;	// RXCINT
//    PieCtrlRegs.PIEIER8.bit.INTx6 = 1;	// TXCINT

    IER = M_INT8 | M_INT9;

    EINT;
    // Enable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    // Enables PIE to drive a pulse into the CPU
    PieCtrlRegs.PIEACK.all = 0xFFFF;
}

#include "file_map.h"

void my_main() {


	s32 buf_sz = BMP_GET_SIZE_16(TFT_WIDTH,TFT_HEIGHT);

	u8 *buf0 = new u8[buf_sz];

	surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,buf_sz, buf0);

//	IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_1_BASE,0xFF);

//	alt_putstr("Hello from Nios II!\n");

    spi.Init();

    u32 ft_magic = 0;

    for (u32 ofs = 0; ofs < sizeof(ft_magic); ofs++) {
    	((u16*)&ft_magic)[ofs] = flash.ReadData(FILE_TABLE_ADDR + (ofs*2));
    }

    if (ft_magic == FILE_TABLE_MAGIC) {
        for (u32 ofs = 0; ofs < sizeof(file_table); ofs++) {
        	((u16*)&file_table)[ofs] = flash.ReadData(FILE_TABLE_ADDR + (ofs*2));
        }
    }

	file_rec_t *fr = find_file(TTCACHE_FILE_ID);
	_MY_ASSERT(fr,);
	u32 ttcache_sz = fr->cur_len;
	_MY_ASSERT(ttcache_sz,);

	u8 *ttcache_dat = new u8[ttcache_sz];
	flash.ReadData2(fr->offset,(u16 *)ttcache_dat, ttcache_sz);

	globals::ttcache.init((u8 *)ttcache_dat,ttcache_sz);

	res.init();
	screen1.init();
	screen1.dirty = true;

	globals::modal_overlay.w = TFT_WIDTH;
	globals::modal_overlay.h = TFT_HEIGHT;
	globals::modal_overlay.push_modal(&screen1);
	globals::modal_overlay.dirty = true;

	drv1.init();
	app_model_t::instance.init();
//	emu.set_target(&app_model_t::instance);
//	app_model_t::instance.subscribe_host(&emu);

    uart.init(&ScibRegs);
	init_pie_table();
    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
    pkz.init(&my_hdlc_tx_frame);
	ser.init(&pkz,&app_model_t::instance);
	app_model_t::instance.subscribe_host(&ser);

	while (1) {
//		emu.update();
		drawScene(s1);

		while (!uart.is_empty()) {

			u8 byte = uart.read();
			hdlc_on_rx_byte(byte);
		}
	}

//	_ASSERT(0, return 0);

//	return 0;
}
