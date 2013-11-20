#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
//#include "DSP2833x_CpuTimers.h"
//#include "DSP2833x_Examples.h"
//#include "DSP2833x_SysCtrl.h"
//#include "cpy_tbl.h"
//#include <new.h>
#include "some_funcs.h"
#include "pins_def.h"
#include "types.h"

extern "C" {
#include "hdlc/hdlc.h"
}

#include "spi_drv_master.h"
#include "packetizer.h"
#include "exported_stub.h"

spi_master_t	spi;
packetizer_impl_t pkz;
myvi::serializer_t ser;
exported_stub_t exported_stub;

extern "C" {
void my_assert (const char *msg, const char *file, int line)
{
//	std::cout << msg << " in " << file << " at " << line << std::endl;
}
void my_trace (const char *msg, const char *file, int line)
{
//	std::cout << msg << " in " << file << " at " << line << std::endl;
}
}

void rs485_put_char(char data) {
	spi.write(data);
}

// к нам пришел пакет от удаленной системы
void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	pkz.receive((const u8*)buffer,bytes_received);
}

void my_hdlc_tx_frame(const u8 *buffer, u16 bytes_to_send) {
	hdlc_tx_frame((const u8_t*)buffer, bytes_to_send);
}

void testPins();

extern "C" void SSD1963_InitHW();
bool isExtRamOK2(u16 *ext_ram_mas, s32 sz);


int main(void)
{	
    InitSysCtrl();

    init_zone7();

    #define ALLOC_SZ 5
	u16 *buf = new u16[ALLOC_SZ];
	if(!isExtRamOK2(buf,ALLOC_SZ))
		asm(" ESTOP0");

    spi.init();
    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
    pkz.init(&my_hdlc_tx_frame);
    ser.init(&pkz,&exported_stub);


    u8 new_frame = 0;
    u16 cnt = 0;

    while (1) {
    	cnt++;
    	if (cnt > 1000)
    		cnt = 0;
    	new_frame = cnt == 500;

		if (new_frame) {
			ser.ChannelDMEChanged(35,myvi::msg::tSuffix::Y);
		}

		while (!spi.is_empty())
		{
			u8 byte = spi.read();
			hdlc_on_rx_byte(byte);
		}
		spi.write(0);
    }

//    while(1) asm(" ESTOP0");
}
