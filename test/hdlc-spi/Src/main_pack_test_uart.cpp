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
#include "hdlc.h"
}

#include "uart_drv.h"
#include "packetizer.h"
#include "serializer.h"
#include "exported_stub.h"

uart_drv_t	uart;
myvi::packetizer_impl_t pkz;
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
	uart.write(data);
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

int main(void)
{	
    InitSysCtrl();

    init_zone7();

    #define ALLOC_SZ 512
	u16 *buf = new u16[ALLOC_SZ];
	if(!isExtRamOK2(buf,ALLOC_SZ))
		asm(" ESTOP0");
	delete[] buf;

    uart.init(&ScicRegs);
	init_pie_table();
    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
    pkz.init(&my_hdlc_tx_frame);
    ser.init(&pkz,&exported_stub);


    u8 new_frame = 0;
    u32 cnt = 0;

    while (1) {
    	cnt++;
    	if (cnt > 1000000)
    		cnt = 0;
    	new_frame = cnt == 500000;

		if (new_frame) {
			ser.ChannelDMEChanged(35,myvi::msg::tSuffix::Y);
//			uart.write(cnt);
		}

		while (!uart.is_empty())
		{
			u8 byte = uart.read();
			hdlc_on_rx_byte(byte);
		}
    }

//    while(1) asm(" ESTOP0");
}
