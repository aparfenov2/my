/*
 * Описание реализации интерфейса связи системного уровня
 *	
 * ЭТО АВТОКОПИЯ !!!
 *  Created on: 12.03.2013
 *      Author: gordeev
 */

#ifndef LINK_COMMON_H_
#define LINK_COMMON_H_

#include "types.h"

namespace link {

// интерфейс канала передачи данных
class serial_data_receiver_t {
public:
	virtual void receive(u8 *data, u32 len) = 0;
};

class serial_interface_t {
public:
	virtual void send(u8 *data, u32 len) = 0;
	virtual void subscribe(serial_data_receiver_t *receiver) = 0;
};

} // ns link
#endif /* LINK_COMMON_H_ */
