/*
 * types.h
 *
 *  Created on: 13.11.2012
 *      Author: Parfenov
 */

#ifndef TYPES_H_
#define TYPES_H_


typedef signed char s8;
typedef unsigned char u8;
typedef signed short int s16;
typedef unsigned short int u16;
typedef signed long int s32;
typedef unsigned long int u32;


#define null 0
#define NULL 0
#define OVERRIDE override
#define SEALED sealed
#define ABSTRACT abstract

#define STATIC_ASSERT( condition, name )\
    typedef char assert_failed_ ## name [ (condition) ? 1 : -1 ];
#define _MY_MIN(a,b) (((a)<(b))?(a):(b))


#endif /* TYPES_H_ */
