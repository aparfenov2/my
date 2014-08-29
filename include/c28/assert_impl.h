#ifndef _MY_ASSERT_IMPL_H
#define _MY_ASSERT_IMPL_H
#ifdef __cplusplus
#include "log.h"
#endif

#ifndef __cplusplus
typedef u8 bool;
#endif
//#define _MY_ASSERT(b,op) { bool bb = (b); if (!bb) my_assert(#b, __FILE__, __LINE__);}
//#define _WEAK_ASSERT(b,op) { bool bb = (b); if (!bb) {my_trace(#b, __FILE__, __LINE__); op;} }
#define _MY_ASSERT(b,op) { bool bb = (b); if (!bb) {asm(" ESTOP0"); op;} }
#define _WEAK_ASSERT(b,op) { bool bb = (b); if (!bb) {asm(" ESTOP0"); op;} }
#define _STOP(v) while(1) {asm(" ESTOP0");}
#define _HALT() _STOP(0)

#ifdef __cplusplus
extern "C" {
#endif

void my_assert (const char *msg, const char *file, int line);
void my_trace (const char *msg, const char *file, int line);
#ifdef __cplusplus
}
#endif

#endif
