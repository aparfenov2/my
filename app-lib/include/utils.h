#ifndef _UTILS_H
#define _UTILS_H

#include "types.h"
#include "assert_impl.h"
#include "basic.h"
// ----------------------------- math limits support ----------------------

namespace myvi {

class math {
public:
	template<typename T>
	static u32 bit_hi(T a);

	template<typename T, u32 bits>
	static bool _addition_is_safe(T a, T b) {
		u32 a_bits=bit_hi<T>(a), b_bits=bit_hi<T>(b);
		return (a_bits < bits && b_bits < bits);
	}

	template<typename T>
	static bool addition_is_safe(T a, T b);


	template<typename T, u32 bits>
	static bool _multiplication_is_safe(T a, T b) {
		u32 a_bits=bit_hi<T>(a), b_bits=bit_hi<T>(b);
		return (a_bits + b_bits <= bits);
	}

	template<typename T>
	static bool multiplication_is_safe(T a, T b);

}; // class math
/*
template<>
u32 math::bit_hi<u32>(u32 a);

template<>
u32 math::bit_hi<s32>(s32 a);

template<>
bool math::addition_is_safe<s32>(s32 a, s32 b);

template<>
bool math::addition_is_safe<u32>(u32 a, u32 b);

template<>
bool math::multiplication_is_safe<s32>(s32 a, s32 b);

template<>
bool math::multiplication_is_safe<u32>(u32 a, u32 b);
*/

class conv_utils {
public:

	template<typename T>
	static bool __ftoa(T value, volatile_string_t &dst, T divider);

	template<typename T>
	static bool ftoa(T value, volatile_string_t &dst, T divider);


	template<typename T>
	static bool __atof(string_t src, T &res, T multiplier, s32 &pos);

	template<typename T>
	static bool atof(string_t src, T &res, T multiplier);

};

template<>
bool conv_utils::ftoa<s32>(s32 value, volatile_string_t &dst, s32 divider);

template<>
bool conv_utils::ftoa<u32>(u32 value, volatile_string_t &dst, u32 divider);

template<>
bool conv_utils::atof<s32>(string_t src, s32 &res, s32 multiplier);

template<>
bool conv_utils::atof<u32>(string_t src, u32 &res, u32 multiplier);

} // ns
#endif
