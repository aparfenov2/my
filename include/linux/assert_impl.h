#ifndef _MY_ASSERT_IMPL_H
#define _MY_ASSERT_IMPL_H
#include "assert.h"
#include "log.h"

#define _MY_ASSERT(b,op) { bool bb; assert(bb = (b)); if (!bb) op; }
//#define _WEAK_ASSERT(b,op) _MY_ASSERT(b,op)
#define _WEAK_ASSERT(b,op) { bool bb; (bb = (b)); if (!bb) _LOG6("WA: \"",#b, "\" in \"",__FILE__, "\" at ", __LINE__); if (!bb) op; }

#define _STOP(v) while(1)

#endif
