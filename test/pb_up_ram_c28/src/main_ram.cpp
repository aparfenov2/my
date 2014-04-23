#include <stdlib.h>
#include <stdio.h>

#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "DSP2833x_CpuTimers.h"
#include "DSP2833x_Examples.h"
#include "DSP2833x_SysCtrl.h"
#include "cpy_tbl.h"
#include <new.h>
#include "some_funcs.h"
#include "c28/pins_def.h"

#include "types.h"
#include "assert_impl.h"

#include "ssd1963drv.h"
#include "surface.h"
#include "bmp_math.h"

#include "app_events.h"

#include "devices.h"
#include "Spi.h"
#include "uart_drv.h"

extern "C" {
#include "hdlc.h"
}

#include <pb_encode.h>
#include <pb_decode.h>

#include "myvi.pb.h"

//void testPins();

extern "C" void SSD1963_InitHW();

using namespace myvi;

ssd1963drv_t drv1;

Spi spi;
FRAM fram;
FlashDev flash;
uart_drv_t	uart;

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

void rs485_put_char(char data) {
	uart.write(data);
}

extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);
#define CRC_SEED 0xabba

u16 frames_count = 0;
u32 last_sector = 0xffff;


#include "file_map.h"

// к нам пришел пакет от удаленной системы
void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {

	myvi_proto_exported_interface_t message;
	memset(&message,0, sizeof(message));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)buffer, bytes_received);
    bool status = pb_decode(&istream, myvi_proto_exported_interface_t_fields, &message);
    if (status) {
		if (message.has_read_file_info) {
			file_rec_t * fr = find_file(message.read_file_info);
			if (fr) {
				myvi_proto_host_interface_t answ;
				memset(&answ,0,sizeof(answ));

				answ.has_file_info_response = true;
				answ.file_info_response.file_id = fr->file_id;
				answ.file_info_response.cur_len = fr->cur_len;
				answ.file_info_response.max_len = fr->max_len;
				answ.file_info_response.has_max_len = true;

				u8 out_buf[255];
				pb_ostream_t ostream = pb_ostream_from_buffer(out_buf, sizeof(out_buf));

				status = pb_encode(&ostream, myvi_proto_host_interface_t_fields, &answ);
				_MY_ASSERT(status,);
				hdlc_tx_frame((const u8_t*)out_buf, ostream.bytes_written);
			}
		} else if (message.has_update_file_info) {
			file_rec_t * fr = find_file(message.update_file_info.file_id);
			if (fr) {
				fr->cur_len = message.update_file_info.cur_len;

				flash.SectorErase(FILE_TABLE_ADDR);

				for (u32 ofs = 0; ofs < sizeof(file_table); ofs++) {
					flash.PageProgram(FILE_TABLE_ADDR + (ofs * 2), ((u16*)file_table)[ofs]);
				}

//			    u32 ft_magic = 0;
//
//			    for (u32 ofs = 0; ofs < sizeof(ft_magic); ofs++) {
//			    	((u16*)&ft_magic)[ofs] = flash.ReadData(FILE_TABLE_ADDR + (ofs*2));
//			    }


			}

		} else if (message.has_upload_file) {
//			fram.Write2((u16)message.upload_font.addr, (s16*)message.upload_font.data.bytes, (u16)message.upload_font.data.size);
			file_rec_t * fr = find_file(message.upload_file.file_id);
			if (fr) {
				if (message.upload_file.first) {
					last_sector = 0xffff;
	//				flash.BulkErase();
				}
				u32 ofs = 0;
				for (u32 addr = message.upload_file.offset; ofs < message.upload_file.data.size;) {
					if (addr >= fr->max_len) {
						break;
					}
					u32 sector = (fr->offset + addr) >> 16;
					if (last_sector != sector) {
						last_sector = sector;
						flash.SectorErase(fr->offset + addr);
					}
					flash.PageProgram2(fr->offset + addr, message.upload_file.data.bytes[ofs]);
					addr++;
					ofs++;
				}
			}

			//flash.PageProgram2(message.upload_font.addr, (u16*)message.upload_font.data.bytes, message.upload_font.data.size );

		} else if (message.has_download_file) {

			file_rec_t * fr = find_file(message.download_file.file_id);
			if (fr) {
				myvi_proto_host_interface_t answ;
				memset(&answ,0,sizeof(answ));

				u8 out_buf[255];

				pb_ostream_t ostream = pb_ostream_from_buffer(out_buf, sizeof(out_buf));

				answ.has_download_response = true;
				answ.download_response.offset = message.download_file.offset;
				answ.download_response.data.size = message.download_file.length;

	//			fram.Read2((u16)message.download_font_request.addr, (s16*)answ.download_font_response.data.bytes, (u16)message.download_font_request.length);

				if (message.download_file.offset + message.download_file.length <= fr->max_len) {

					flash.ReadData2(fr->offset + message.download_file.offset,(u16*)answ.download_response.data.bytes, message.download_file.length);
					answ.download_response.crc = crc16_ccitt_calc_data(CRC_SEED,answ.download_response.data.bytes, message.download_file.length);
				} else {
					answ.has_download_response = false;
					answ.has_error = true;
					answ.error = 0x01;
				}
				status = pb_encode(&ostream, myvi_proto_host_interface_t_fields, &answ);
				_MY_ASSERT(status,);
				hdlc_tx_frame((const u8_t*)out_buf, ostream.bytes_written);
			}
		}
    }
    frames_count++;
// send ACK
    uart.write(0x55);
}

//void my_hdlc_tx_frame(const u8 *buffer, u16 bytes_to_send) {
//	hdlc_tx_frame((const u8_t*)buffer, bytes_to_send);
//}

int main(void)
{
    InitSysCtrl();

    init_zone7();

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


    uart.init(&ScibRegs);
	init_pie_table();

    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);

	while (1) {
		while (!uart.is_empty()) {

			u8 byte = uart.read();
			hdlc_on_rx_byte(byte);
		}
	}
}
