#ifndef _APP_EVENTS_H
#define _APP_EVENTS_H
namespace myvi {

namespace key_t {
	typedef enum {
	//	K_NONE = 0,
		K_LEFT = 1,
		K_RIGHT = 2,
		K_UP = 3,
		K_DOWN = 4,
		K_ESC = 5,
		K_ENTER = 6,
		K_BKSP = 7,
		K_F1 = 8,
		K_F2 = 9,
		K_F3 = 10,
		K_F4 = 11,
		K_F5 = 12,
		K_0 = 13,
		K_1 = 14,
		K_2 = 15,
		K_3 = 16,
		K_4 = 17,
		K_5 = 18,
		K_6 = 19,
		K_7 = 20,
		K_8 = 21,
		K_9 = 22,
		K_SAVE = 23,
		K_EXIT = 24,
		K_DOT = 25,
		K_DELETE = 26

	} key_t;
}

namespace mkey_t {
	typedef enum {
		MK_NONE = 0,
		MK_1,
		MK_2,
		MK_3
	} mkey_t;
}
} // ns
#endif
