#include <string.h>

#include "assert_impl.h"

#include "file_map.h"
#include "file_system_impl.h"
#include "ttcache.h"

using namespace hw;


file_system_impl_t fs;

Spi spi;
FRAM fram;
FlashDev flash;


void my_main() {


    spi.Init();
	fram.init(&spi);
	flash.init(&spi);

	_WEAK_ASSERT(fs.init(&fram, &flash), 0);


    bool ttcache_loaded = false;
	do {
		u32 ttcache_file_id = TTCACHE_FILE_ID;
		u32 ttcache_len, ttcache_max_len;
		_WEAK_ASSERT(fs.get_info(ttcache_file_id, ttcache_len, ttcache_max_len), break);
		if (!ttcache_len) break;

		u8 *ttcache_dat = new u8[ttcache_len + 0x0f];
		_WEAK_ASSERT(ttcache_dat, break);

		_WEAK_ASSERT(fs.read_file(ttcache_file_id,0,ttcache_len, ttcache_dat), break);

		myvi::globals::ttcache.init((u8 *)ttcache_dat,ttcache_len);
		ttcache_loaded = true;

	} while(false);


	while (1) {
	}

}
