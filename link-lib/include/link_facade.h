/*
* Фасад для библиотеки связи
*/

#ifndef _LINK_FACADE_H
#define _LINK_FACADE_H

#include "link_sys_impl.h"
#include "link_app.h"

namespace link {


class host_facade_t {
public:
	app_link_decoder_t dec;
	app_link_encoder_t enc;
	host_serializer_t hs;
public:
	//ahost - сюда приходят ответы от дисплея (уровень приложения)
	//aexported2 - сюда будут отправляться запросы от хоста (системный уровень)
	// _chained - сюда приходят ответы от дисплея (ситемный уровень)

	void init(host_interface_t *ahost, serial_interface_t *asintf) {

		dec.init(ahost);
		enc.init(&hs);

		hs.init(asintf);
		hs.add_implementation(&dec);
	}

	// сюда хост посылает запросы
	exported_interface_t * get_app_interface() {
		return &enc;
	}

	exported_system_interface_t * get_sys_interface() {
		return &hs;
	}

};

/*
class local_facade_t {
public:
	serializer_t hs;
public:

	void init(exported_system_interface_t *exported_sys, serial_interface_t *asintf) {

		hs.init(exported_sys, asintf);
	}


	host_system_interface_t * get_sys_interface() {
		return &hs;
	}

};
*/

} // end ns link
#endif