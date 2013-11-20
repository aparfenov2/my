#ifndef _MY_ASSERT_IMPL_H
#define _MY_ASSERT_IMPL_H

#include "log.h"

#define __MY_ASSERT(b) if (!(b)) my_assert(#b, __FILE__, __LINE__)
#define __TRACE(b) if (!(b)) my_trace(#b, __FILE__, __LINE__)
#define _MY_ASSERT(b,op) { bool bb; __MY_ASSERT(bb = (b)); if (!bb) op; }
#define _WEAK_ASSERT(b,op) { bool bb; __TRACE(bb = (b)); if (!bb) op; }
#define _STOP(v) while(1)

extern "C" void my_assert (const char *msg, const char *file, int line);
extern "C" void my_trace (const char *msg, const char *file, int line);
#endif
