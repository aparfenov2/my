/*
* ‘асад дл€ библиотеки св€зи
* Ё“ќ ј¬“ќ ќѕ»я !!!
*/

#ifndef _LINK_FACADE_H
#define _LINK_FACADE_H

#include "link_sys_impl_link.h"
#include "link_app.h"

namespace link {


class host_facade_t {
public:
	app_link_decoder_t dec;
	app_link_encoder_t enc;
	host_serializer_t hs;
public:
	//ahost - сюда приход€т ответы от диспле€ (уровень приложени€)
	//aexported2 - сюда будут отправл€тьс€ запросы от хоста (системный уровень)
	// _chained - сюда приход€т ответы от диспле€ (ситемный уровень)

	void init(host_interface_t *ahost, serial_interface_t *asintf) {

		hs.init(asintf);

		dec.init(ahost);
		hs.add_implementation(&dec);

		enc.init(hs.get_exported_model_interface());

	}

	// сюда хост посылает запросы
	exported_interface_t * get_app_interface() {
		return &enc;
	}

	host_serializer_t & get_serializer() {
		return hs;
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
