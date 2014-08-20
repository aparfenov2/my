/*
 * ќписание реализации интерфейса св€зи системного уровн€
 * Ё“ќ ј¬“ќ ќѕ»я !!! *
 *
 *  Created on: 12.03.2013
 *      Author: gordeev
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include "link.h"
#include "exported_sys.h"
#include "link_sys_impl_common.h"
#include "link_sys_impl_gen.h"

namespace link {

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



class serializer_t :
	public packet_sender_t,
	public _internal_frame_receiver_t {

public:
	host_model_interface_t_serializer_t host_model_interface_t_serializer;
	host_file_interface_t_serializer_t host_file_interface_t_serializer;
	host_debug_interface_t_serializer_t host_debug_interface_t_serializer;
	framer_t framer;
public:

	void add_implementation(exported_system_interface_t *impl) {
		exported_model_interface_t *exported_model_interface = dynamic_cast<exported_model_interface_t*>(impl);
		if (exported_model_interface) {
			host_model_interface_t_serializer.set_implementation(exported_model_interface);
		}
		exported_file_interface_t *exported_file_interface = dynamic_cast<exported_file_interface_t*>(impl);
		if (exported_file_interface) {
			host_file_interface_t_serializer.set_implementation(exported_file_interface);
		}
		exported_debug_interface_t *exported_debug_interface = dynamic_cast<exported_debug_interface_t*>(impl);
		if (exported_debug_interface) {
			host_debug_interface_t_serializer.set_implementation(exported_debug_interface);
		}
	}

	void init(serial_interface_t *asintf ) {
		framer.init(asintf, this);
		host_model_interface_t_serializer.init(this);
		host_file_interface_t_serializer.init(this);
		host_debug_interface_t_serializer.init(this);
	}
	

	void send_packet(void *packet, u32 sizeof_packet) OVERRIDE;

	host_model_interface_t * get_host_model_interface()  {
		return & host_model_interface_t_serializer;
	}
	host_file_interface_t * get_host_file_interface()  {
		return & host_file_interface_t_serializer;
	}
	host_debug_interface_t * get_host_debug_interface()  {
		return & host_debug_interface_t_serializer;
	}

// ------------------- public serial_data_receiver_t --------------------
	// прием фрейма данных
	void receive_frame(u8 *data, u32 len) OVERRIDE;
	// called from receive_frame
	void _receive_frame(void *pkt, u32 sizeof_pkt) {
		host_model_interface_t_serializer.receive_packet(pkt, sizeof_pkt);
		host_file_interface_t_serializer.receive_packet(pkt, sizeof_pkt);
		host_debug_interface_t_serializer.receive_packet(pkt, sizeof_pkt);
	}

}; // class


//сериализер на стороне хоста
class host_serializer_t :
	public packet_sender_t,
	public _internal_frame_receiver_t {

public:
	exported_model_interface_t_serializer_t exported_model_interface_t_serializer;
	exported_file_interface_t_serializer_t exported_file_interface_t_serializer;
	exported_debug_interface_t_serializer_t exported_debug_interface_t_serializer;
	framer_t framer;
public:

	void add_implementation(host_system_interface_t *impl) {
		host_model_interface_t *host_model_interface = dynamic_cast<host_model_interface_t*>(impl);
		if (host_model_interface) {
			exported_model_interface_t_serializer.set_implementation(host_model_interface);
		}
		host_file_interface_t *host_file_interface = dynamic_cast<host_file_interface_t*>(impl);
		if (host_file_interface) {
			exported_file_interface_t_serializer.set_implementation(host_file_interface);
		}
		host_debug_interface_t *host_debug_interface = dynamic_cast<host_debug_interface_t*>(impl);
		if (host_debug_interface) {
			exported_debug_interface_t_serializer.set_implementation(host_debug_interface);
		}
	}

	exported_model_interface_t * get_exported_model_interface()  {
		return & exported_model_interface_t_serializer;
	}
	exported_file_interface_t * get_exported_file_interface()  {
		return & exported_file_interface_t_serializer;
	}
	exported_debug_interface_t * get_exported_debug_interface()  {
		return & exported_debug_interface_t_serializer;
	}

	void init(serial_interface_t *asintf ) {
		framer.init(asintf, this);
		exported_model_interface_t_serializer.init(this);
		exported_file_interface_t_serializer.init(this);
		exported_debug_interface_t_serializer.init(this);
	}


	// прием фрейма данных
	void send_packet(void *packet, u32 sizeof_packet) OVERRIDE;
	void receive_frame(u8 *data, u32 len) OVERRIDE;

	void _receive_frame(void *pkt, u32 sizeof_pkt) {
		exported_model_interface_t_serializer.receive_packet(pkt, sizeof_pkt);
		exported_file_interface_t_serializer.receive_packet(pkt, sizeof_pkt);
		exported_debug_interface_t_serializer.receive_packet(pkt, sizeof_pkt);
	}
}; // host_serializer_t

} // ns myvi
#endif /* SERIALIZER_H_ */
