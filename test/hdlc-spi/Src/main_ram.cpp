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

#include "spi_async.h"

spi_async_t	spi;

extern "C" {
void my_assert (const char *msg, const char *file, int line)
{
//	std::cout << msg << " in " << file << " at " << line << std::endl;
}
}

void rs485_put_char(char data)
{
	spi.write(data);
}

// к нам пришел пакет от удаленной системы
void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received)
{
//	std::cout << "frame[" << bytes_received << "]: " << std::endl;
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

	u8_t data[] = {0xab, 0xcd, 0x7e, 0x34};
    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);

    u8 new_frame = 0;
    u16 cnt = 0;

    while (1) {
    	cnt++;
    	if (cnt > 1000)
    		cnt = 0;
    	new_frame = cnt == 500;

		if (new_frame)
			hdlc_tx_frame(data,sizeof(data));

		while (!spi.is_empty())
		{
			u8 byte = spi.read();
			hdlc_on_rx_byte(byte);
		}
		spi.write(0);
    }

//    while(1) asm(" ESTOP0");
}
