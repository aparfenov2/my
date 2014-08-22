#include <string.h>

#include "assert_impl.h"

#include "file_map.h"
#include "file_system_impl.h"


using namespace hw;


file_system_impl_t fs;

Spi spi;
FRAM fram;
FlashDev flash;

extern const char *data;


void my_main() {


    spi.Init();
	fram.init(&spi);
	flash.init(&spi);

	_WEAK_ASSERT(fs.init(&fram, &flash), 0);

	u32 file_id = SCHEMA_FILE_ID;
	u32 len = std::strlen(data);
	u32 rlen, max_len;
	_MY_ASSERT(fs.get_info(file_id,rlen, max_len), return);
	_MY_ASSERT(len < max_len, return);

	_MY_ASSERT(fs.write_file(file_id, 0, len, (u8 *)data), return);

	_MY_ASSERT(fs.set_info(file_id,len), return);

	_MY_ASSERT(fs.get_info(file_id,rlen, max_len), return);
	_MY_ASSERT(len == rlen, return);

	u8 *rdata = new u8[len + 100];


	u32 offset = 0;
	_MY_ASSERT(fs.read_file(file_id,offset,len,rdata), return);
	_MY_ASSERT(std::strcmp(data, (const char *)rdata) == 0, return);


	while (1) {
	}

}
