/*
 * flash_intf_impl.h
 *
 *  Created on: 29.08.2014
 *      Author: user
 */

#ifndef FLASH_INTF_IMPL_H_
#define FLASH_INTF_IMPL_H_

#include "exported_sys.h"

namespace hw {

class flash_intf_impl_t :
	public link::exported_system_interface_t,
	public link::exported_flash_interface_t {
public:
	link::host_flash_interface_t *host_intf;
public:
	flash_intf_impl_t() {
		host_intf = 0;
	}
	void init(link::host_flash_interface_t *_host_intf) {
		host_intf = _host_intf;
	}
	virtual void define_section (u32 key, u32 len, u32 flash_addr, u32 crc) OVERRIDE;

	virtual void post_sec_data (u32 key, u32 len, u32 remain, u8 * data) OVERRIDE;

	virtual void transfer_complete (u32 transfer_complete) OVERRIDE;

};

} // ns
#endif /* FLASH_INTF_IMPL_H_ */
