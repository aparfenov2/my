#ifndef _LINK_SYS_IMPL_COMMON
#define _LINK_SYS_IMPL_COMMON

#include "types.h"

namespace link {

class packet_sender_t {
public:
	virtual void send_packet(void *packet, u32 sizeof_packet) = 0;
};

}
#endif