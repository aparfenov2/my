
#include "file_system_impl.h"
#include "assert_impl.h"
#include "file_map.h"
extern "C" {
#include "crc16_ccitt.h"
}

using namespace hw;

extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);

#define CRC_SEED 0xabba

#define FILE_TABLE_MAGIC 0xabba
#define FILE_TABLE_ADDR 0x000000


namespace file_source_t {
	typedef enum {
		FLASH,
		FRAM
	} file_source_t;
}

typedef struct {
	u32 file_id;
	file_source_t::file_source_t file_source;
	u32 offset;
	u32 cur_len;
	u32 max_len;
	u32 crc;
} file_rec_t;


#define FILE_TABLE_SIZE 6

static file_rec_t file_table[FILE_TABLE_SIZE] = {
		{FILE_TABLE_MAGIC,file_source_t::FLASH,FILE_TABLE_ADDR,0,0x010000,0}, // file table
		{TTCACHE_FILE_ID,file_source_t::FLASH,0x010000,0,0x040000,0}, // font cache
		{LOGO_FILE_ID,file_source_t::FLASH,0x050000,0,0x060000,0}, // startup bitmap
		{SCHEMA_FILE_ID,file_source_t::FLASH,0x0B0000,0,0x010000,0}, // schema rus
		{SCHEMA_EN_FILE_ID,file_source_t::FLASH,0x0C0000,0,0x010000,0}, // schema en
		{0x00,file_source_t::FLASH,0,0,0,0}
};


static file_rec_t * find_file(u32 id) {
	file_rec_t *p = &file_table[0];
	while (p->file_id) {
		if (p->file_id == id) {
			return p;
		}
		p++;
	}
	return 0;
}



#define _FLASH_BLOCK_SIZE 255

bool file_system_impl_t::read_file(u32 file_id, u32 offset, u32 length, u8 *buf ) {

	file_rec_t * fr = find_file(file_id);
	if (!fr) return false;

	if (offset + length > fr->max_len) return false;

	u32 remain = length;
	u32 buf_ofs = 0;
	do {
		u32 pkt_len = _MY_MIN(remain,_FLASH_BLOCK_SIZE);
		remain -= pkt_len;


		flash->ReadData2(fr->offset + offset,(u16*)buf + buf_ofs, pkt_len);


		offset += _FLASH_BLOCK_SIZE;
		buf_ofs += _FLASH_BLOCK_SIZE;


	} while (remain);
	return true;
}



bool file_system_impl_t::write_file(u32 file_id, u32 offset, u32 len, u8 *data) {

	file_rec_t * fr = find_file(file_id);
	if (!fr) return false;

	if (offset + len >= fr->max_len) {
		return false;
	}


	static u32 last_sector = 0xffff;

	if (!offset) {
		last_sector = 0xffff;
	}
	u32 ofs = 0;
	// ofs - смещение в data
	// offset - смещение от начала файла
	u32 flash_addr = fr->offset + offset;

	for (; ofs < len;) {

		u32 sector = (flash_addr) >> 16;

		if (last_sector != sector) {
			last_sector = sector;
			flash->SectorErase(flash_addr);
		}
		flash->PageProgram2(flash_addr, data[ofs]);
		flash_addr++;
		ofs++;
	}
	return true;
}

bool file_system_impl_t::get_info(u32 file_id, u32 &len, u32 &max_len) {
	file_rec_t * fr = find_file(file_id);
	len = 0, max_len = 0;
	if (fr) {
		len = fr->cur_len, max_len = fr->max_len;
	} else {
		return false;
	}
	return true;
}

bool file_system_impl_t::set_info(u32 file_id, u32 len) {

	file_rec_t * fr = find_file(file_id);
	if (fr) {
		fr->cur_len = len;

		flash->SectorErase(FILE_TABLE_ADDR);

		for (u32 ofs = 0; ofs < sizeof(file_table); ofs++) {
			flash->PageProgram(FILE_TABLE_ADDR + (ofs * 2), ((u16*)file_table)[ofs]);
		}
	} else {
		return false;
	}
	return true;
}

bool file_system_impl_t::read_file_table() {
    u32 ft_magic = 0;

    for (u32 ofs = 0; ofs < sizeof(ft_magic); ofs++) {
    	((u16*)&ft_magic)[ofs] = flash->ReadData(FILE_TABLE_ADDR + (ofs*2));
    }

    if (ft_magic == FILE_TABLE_MAGIC) {
    	file_rec_t *temp_table = new file_rec_t[sizeof(file_table)/sizeof(file_rec_t)];
        for (u32 ofs = 0; ofs < sizeof(file_table); ofs++) {
        	((u16*)temp_table)[ofs] = flash->ReadData(FILE_TABLE_ADDR + (ofs*2));
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


/*
#define ALIGNED(ptr) (!(0x03 & (u32)(ptr)))

bool file_system_impl_t::allocate_and_read_font_cache(u8 *&ttcache_dat, u32 &ttcache_sz) {

	u32 lmax;
	_MY_ASSERT(this->get_info(TTCACHE_FILE_ID, ttcache_sz, lmax), return false);
	_WEAK_ASSERT(ttcache_sz, return false);

	ttcache_dat = new u8[ttcache_sz + 0x0f];
	_WEAK_ASSERT(ttcache_dat, return false);

	while (!ALIGNED(ttcache_dat)) {
		ttcache_dat++;
	}
	_WEAK_ASSERT(this->read_file(TTCACHE_FILE_ID,0,ttcache_sz,ttcache_dat), return false);
	return true;
}
*/
