/*
 * exported2_impl.h
 *
 *  Created on: 26.04.2014
 *      Author: Администратор
 */

#ifndef EXPORTED2_IMPL_H_
#define EXPORTED2_IMPL_H_

#include "exported.h"

namespace myvi {
class exported_interface2_impl_t : public msg::exported_interface2_t  {
public:
	msg::host_interface2_t *host;
public:
	exported_interface2_impl_t(msg::host_interface2_t *ahost) {
		host = ahost;
	}
	void key_event(key_t::key_t key) OVERRIDE;
	void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE;
	void download_file(u32 file_id, u32 offset, u32 length) OVERRIDE;
	void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) OVERRIDE;
	void read_file_info(u32 file_id) OVERRIDE;

}; // class

} // ns myvi

bool read_file_table();
bool allocate_and_read_font_cache(u8 *&buf, u32 &sz);


#endif /* EXPORTED2_IMPL_H_ */
