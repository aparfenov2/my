/* This is an example of a header file for platforms/compilers that do
 * not come with stdint.h/stddef.h/stdbool.h/string.h. To use it, define
 * PB_SYSTEM_HEADER as "pb_syshdr.h", including the quotes, and add the
 * extra folder to your include path.
 *
 * It is very likely that you will need to customize this file to suit
 * your platform. For any compiler that supports C99, this file should
 * not be necessary.
 */

#ifndef _PB_SYSHDR_H_
#define _PB_SYSHDR_H_

#include "types.h"

//#define HAVE_STDINT_H
#define HAVE_STDDEF_H
#define HAVE_STDBOOL_H
#define HAVE_STRING_H

/* stdint.h subset */
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
/* You will need to modify these to match the word size of your platform. */
typedef s8 int8_t;
typedef u8 uint8_t;
typedef s32 int16_t;
typedef u32 uint16_t;
typedef s64 int32_t;
typedef u64 uint32_t;
typedef s64 int64_t;
typedef u64 uint64_t;

#endif

/* stddef.h subset */
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#else

typedef u32 size_t;
#define offsetof(st, m) ((size_t)(&((st *)0)->m))

#ifndef NULL
#define NULL 0
#endif

#endif

/* stdbool.h subset */
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#else

#ifndef __cplusplus
typedef int bool;
#define false 0
#define true 1
#endif

#endif

/* stdlib.h subset */
#ifdef PB_ENABLE_MALLOC
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
void *realloc(void *ptr, size_t size);
void free(void *ptr);
#endif
#endif

/* string.h subset */
#ifdef HAVE_STRING_H
#include <string.h>
#else

/* Implementations are from the Public Domain C Library (PDCLib). */
static size_t strlen( const char * s )
{
    size_t rc = 0;
    while ( s[rc] )
    {
        ++rc;
    }
    return rc;
}

static void * memcpy( void *s1, const void *s2, size_t n )
{
    char * dest = (char *) s1;
    const char * src = (const char *) s2;
    while ( n-- )
    {
        *dest++ = *src++;
    }
    return s1;
}

static void * memset( void * s, int c, size_t n )
{
    unsigned char * p = (unsigned char *) s;
    while ( n-- )
    {
        *p++ = (unsigned char) c;
    }
    return s;
}
#endif

#endif
