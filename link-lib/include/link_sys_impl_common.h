#ifndef _LINK_SYS_IMPL_COMMON
#define _LINK_SYS_IMPL_COMMON

#include "types.h"
#include "link.h"

// Ё“ќ ј¬“ќ ќѕ»я !!!

namespace link {

class packet_sender_t {
public:
	virtual void send_packet(void *packet, u32 sizeof_packet) = 0;
};


class _internal_frame_receiver_t {
public:
	virtual void receive_frame(u8 *data, u32 len) = 0;
};


class framer_t : public serial_data_receiver_t {
public:
	serial_interface_t *sintf;
	_internal_frame_receiver_t *frame_receiver;
public:
	framer_t() {
		sintf = 0;
		frame_receiver = 0;
	}

	void init(serial_interface_t *_sintf, _internal_frame_receiver_t *_frame_receiver) {
		sintf = _sintf;
		frame_receiver = _frame_receiver;
		sintf->subscribe(this);
		sys_init();
	}

	void send_frame(u8 *data, u32 len);

	// прием блока данных из интерфеса UART
	void receive(u8 *data, u32 len) OVERRIDE;

private:
	void sys_init(); 
};



}
#endif