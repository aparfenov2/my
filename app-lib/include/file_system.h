/*
 * file_system.h
 *
 *  Created on: 15.08.2014
 *      Author: user
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#include "types.h"

namespace app {
	class file_system_t {
	public:
		virtual bool read_file(u32 file_id, u32 offset, u32 len, u8 *data, u32 &read ) = 0;
		virtual bool write_file(u32 file_id, u32 offset, u32 len, u8 *data, u32 &written ) = 0;
		virtual bool get_info(u32 file_id, u32 &len, u32 &max_len) = 0;
		virtual bool set_info(u32 file_id, u32 len) = 0;
	};
}

#endif /* FILE_SYSTEM_H_ */
