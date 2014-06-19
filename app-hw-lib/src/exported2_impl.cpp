
#include "exported2_impl.h"
#include "widgets.h"
#include "devices.h"
#include "file_map.h"
#include "uart_drv.h"
extern "C" {
#include "crc16_ccitt.h"
}

extern FRAM fram;
extern FlashDev flash;
extern uart_drv_t uart;

extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);

static u32 last_sector = 0xffff;
#define CRC_SEED 0xabba

using namespace myvi;

void exported_interface2_impl_t::key_event(key_t::key_t key) {
//	globals::modal_overlay.key_event(key);
}

void exported_interface2_impl_t::upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) {
	file_rec_t * fr = find_file(file_id);
	if (fr) {
		if (first) {
			last_sector = 0xffff;
		}
		u32 ofs = 0;
		for (u32 addr = offset; ofs < len;) {
			if (addr >= fr->max_len) {
				break;
			}
			u32 sector = (fr->offset + addr) >> 16;
			if (last_sector != sector) {
				last_sector = sector;
				flash.SectorErase(fr->offset + addr);
			}
			flash.PageProgram2(fr->offset + addr, data[ofs]);
			addr++;
			ofs++;
		}
	}
	uart.write(0x55); // send ACK
}

void exported_interface2_impl_t::download_file(u32 file_id, u32 offset, u32 length) {
	file_rec_t * fr = find_file(file_id);
	if (fr) {
		if (offset + length <= fr->max_len && length <= 255) {
			u8 buf[255];
			flash.ReadData2(fr->offset + offset,(u16*)buf, length);
			u16 crc = crc16_ccitt_calc_data(CRC_SEED,buf, length);
			host->download_response(file_id,offset,crc,false,buf,length);
		} else {
			host->error(0x01);
		}
	}
}

void exported_interface2_impl_t::update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) {
	file_rec_t * fr = find_file(file_id);
	if (fr) {
		fr->cur_len = cur_len;

		flash.SectorErase(FILE_TABLE_ADDR);

		for (u32 ofs = 0; ofs < sizeof(file_table); ofs++) {
			flash.PageProgram(FILE_TABLE_ADDR + (ofs * 2), ((u16*)file_table)[ofs]);
		}
	}

	uart.write(0x55); // send ACK
}

void exported_interface2_impl_t::read_file_info(u32 file_id) {
	file_rec_t * fr = find_file(file_id);
	if (fr) {
		host->file_info_response(file_id, fr->cur_len, fr->max_len, fr->crc);
	}
}

bool read_file_table() {
    u32 ft_magic = 0;

    for (u32 ofs = 0; ofs < sizeof(ft_magic); ofs++) {
    	((u16*)&ft_magic)[ofs] = flash.ReadData(FILE_TABLE_ADDR + (ofs*2));
    }

    if (ft_magic == FILE_TABLE_MAGIC) {
    	file_rec_t *temp_table = new file_rec_t[sizeof(file_table)/sizeof(file_rec_t)];
        for (u32 ofs = 0; ofs < sizeof(file_table); ofs++) {
        	((u16*)temp_table)[ofs] = flash.ReadData(FILE_TABLE_ADDR + (ofs*2));
        }
    	file_rec_t *p = &temp_table[0];
    	while (p->file_id) {
    		file_rec_t *fr = find_file(p->file_id);
    		if (fr) {
    			fr->cur_len = p->cur_len;
    			fr->crc = p->crc;
    		}
    		p++;
    	}
        delete temp_table;
    }
    return (ft_magic == FILE_TABLE_MAGIC);
}


#define ALIGNED(ptr) (!(0x03 & (u32)(ptr)))

bool allocate_and_read_font_cache(u8 *&ttcache_dat, u32 &ttcache_sz) {

	file_rec_t *fr = find_file(TTCACHE_FILE_ID);
	_MY_ASSERT(fr, return false);
	ttcache_sz = fr->cur_len;
	_WEAK_ASSERT(ttcache_sz, return false);

	ttcache_dat = new u8[ttcache_sz + 0x0f];

	while (!ALIGNED(ttcache_dat)) {
		ttcache_dat++;
	}
	flash.ReadData2(fr->offset,(u16 *)ttcache_dat, ttcache_sz);
	return true;
}
