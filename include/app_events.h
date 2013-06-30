#ifndef _APP_EVENTS_H
#define _APP_EVENTS_H
namespace myvi {

namespace key_t {
	typedef enum {
	//	K_NONE = 0,
		K_LEFT = 1,
		K_RIGHT,
		K_UP,
		K_DOWN,
		K_ESC,
		K_ENTER,
		K_BKSP,
		K_F1,
		K_F2,
		K_F3,
		K_F4,
		K_F5,
		K_0,
		K_1,
		K_2,
		K_3,
		K_4,
		K_5,
		K_6,
		K_7,
		K_8,
		K_9,
		K_SAVE,
		K_EXIT,
		K_DOT,
		K_DELETE

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
