#ifndef _MY_ASSERT_IMPL_H
#define _MY_ASSERT_IMPL_H

#include "log.h"
#define _MY_ASSERT(b,op) { bool bb = (b); if (!bb) my_assert(#b, __FILE__, __LINE__);}
#define _WEAK_ASSERT(b,op) { bool bb = (b); if (!bb) {my_trace(#b, __FILE__, __LINE__); op;} }
#define _STOP(v) while(1)

extern "C" void my_assert (const char *msg, const char *file, int line);
extern "C" void my_trace (const char *msg, const char *file, int line);
#endif
