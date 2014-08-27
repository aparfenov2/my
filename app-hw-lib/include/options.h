/*
 * options.h
 *
 *  Created on: 27.08.2014
 *      Author: user
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "devices.h"

namespace hw {

// ключи для опций
#define OPT_LANG 1

class options_t {
private:
	static options_t _instance;
	FRAM *fram;

	options_t() {
		fram = 0;
	}
public:
	static options_t & instance() {
		return _instance;
	}
	void init(FRAM *_fram) {
		fram = _fram;
	}
	void get_int_value(u32 key, u32 &val);
	void set_int_value(u32 key, u32 val);
};


}

#endif /* OPTIONS_H_ */
