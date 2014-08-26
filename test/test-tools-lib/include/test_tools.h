#ifndef _TTOLS_H
#define _TTOLS_H

#include "surface.h"
#include "app_events.h"
#include "disp_def.h"
#include "link.h"

namespace test {

class test_drawer_t {
public:
	u32 w;
	u32 h;

public:
	test_drawer_t() {
		w = TFT_WIDTH, h = TFT_HEIGHT;
	}
	// return : if true - update
	virtual bool callback(myvi::key_t::key_t key, s32 mx, s32 my, myvi::mkey_t::mkey_t mkey) {
		return false;
	}
	bool cycle(myvi::surface_t &s);
	void create_window(myvi::surface_t &s, const char *title);
protected:
	void do_update(myvi::surface_t &s);
};


class serial_port_t {
public:
	void * port_name;
public:
	serial_port_t(void * _port_name) {
		port_name = _port_name;
	}
};

class serial_interface_impl_t : public link::serial_interface_t {
public:
	link::serial_data_receiver_t *receiver;
	void *serial;
public:
	serial_interface_impl_t() {
		receiver = 0;
		serial = 0;
	}

	bool init(serial_port_t port);

	virtual void send(u8 *data, u32 len) OVERRIDE;

	virtual void subscribe(link::serial_data_receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	void cycle();
};


} // ns
#endif
