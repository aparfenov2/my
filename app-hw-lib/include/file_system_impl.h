/*
 *
 *  Created on: 26.04.2014
 *      Author: Администратор
 */

#ifndef EXPORTED2_IMPL_H_
#define EXPORTED2_IMPL_H_

#include "file_system.h"

#include "devices.h"

namespace hw {
	class file_system_impl_t : public app::file_system_t  {
	public:
		FRAM *fram;
		FlashDev *flash;
	public:
		file_system_impl_t() {
			fram = 0;
			flash = 0;
		}

		bool init(FRAM *_fram, FlashDev *_flash) {
			fram = _fram;
			flash = _flash;
			return read_file_table();
		}

		bool allocate_and_read_font_cache(u8 *&buf, u32 &sz);

		virtual bool read_file(u32 file_id, u32 offset, u32 len, u8 *data, u32 &read ) OVERRIDE;
		virtual bool write_file(u32 file_id, u32 offset, u32 len, u8 *data, u32 &written ) OVERRIDE;
		virtual bool get_info(u32 file_id, u32 &len, u32 &max_len) OVERRIDE;
		virtual bool set_info(u32 file_id, u32 len) OVERRIDE;

	private:
		bool read_file_table();
	}; // class


} // ns hw



#endif /* EXPORTED2_IMPL_H_ */
