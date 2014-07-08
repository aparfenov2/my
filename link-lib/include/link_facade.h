/*
* Фасад для библиотеки связи
*/

#ifndef _LINK_FACADE_H
#define _LINK_FACADE_H

#include "link_sys.h"
#include "link_app.h"

namespace link {

class app_link_facade_t {
public:
	app_link_decoder_t dec;
	app_link_encoder_t enc;
	myvi::host_serializer_t hs;
public:
	//ahost - сюда приходят ответы от дисплея (уровень приложения)
	//aexported2 - сюда будут отправляться запросы от хоста (системный уровень)
	// _chained - сюда приходят ответы от дисплея (ситемный уровень)
	void init(link::host_interface_t *ahost, myvi::serial_interface_t *asintf) {

		dec.init(ahost, 0);
		enc.init(&hs);

		hs.init(&dec, asintf);
	}

	// сюда хост посылает запросы
	link::exported_interface_t * get_app_interface() {
		return &enc;
	}

};

} // end ns link
#endif