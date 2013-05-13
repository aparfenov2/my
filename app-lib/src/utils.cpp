#include "utils.h"

namespace myvi {

template<>
u32 math::bit_hi<u32>(u32 a) {
	u32 bits = 0;
	while (a!=0) {
		++bits;
		a>>=1;
	};
	return bits;
}

template<>
u32 math::bit_hi<s32>(s32 a) {
	return bit_hi<u32>(a < 0 ? -a : a);
}


template<>
bool math::addition_is_safe<s32>(s32 a, s32 b) {
	return _addition_is_safe<s32,31>(a,b);
}

template<>
bool math::addition_is_safe<u32>(u32 a, u32 b) {
	return _addition_is_safe<u32,32>(a,b);
}

template<>
bool math::multiplication_is_safe<s32>(s32 a, s32 b) {
	return _multiplication_is_safe<s32,31>(a,b);
}

template<>
bool math::multiplication_is_safe<u32>(u32 a, u32 b) {
	return _multiplication_is_safe<u32,32>(a,b);
}


#define _CHECK_STR(dst,op) { _MY_ASSERT(dst.length() < dst.capacity(),return false); dst.op; }

template<typename T>
bool conv_utils::__ftoa(T value, volatile_string_t &dst, T divider) {

	_MY_ASSERT(value >= 0 && divider >= 1, return false);
	dst.reset();

	bool dot_inserted = false;
	if (!value) {
		_CHECK_STR(dst,insert_at(0, '0'));
		return true;
	}

	T v = value;
//		bool sign = value < 0;
//		if (sign) v = -v;

	while (v < divider) {
		dst.append('0');
		if (!dot_inserted) {
			dot_inserted = true;
			_CHECK_STR(dst,append('.'));
		}
		v *= 10;
	}

	v = value;
//		if (sign) v = -v;
	s32 pos = dst.length();
	T v1 = 1;
	bool was_1 = false;
	// main loop
	while (v > 0) {
		T mod = v % 10;
		if (mod)
			was_1 = true;
		u8 ch = (u8)mod + '0';
		v /= 10;
		v1 *= 10;

		if (mod || was_1 || v1 > divider) {
			_CHECK_STR(dst,insert_at(pos, ch));
		}

		if ((mod || was_1) && !dot_inserted && v > 0 && v1 == divider) {
			dot_inserted = true;
			_CHECK_STR(dst,insert_at(pos, '.'));
		}
	}
	return true;
}


template<typename T>
bool conv_utils::__atof(string_t src, T &res, T multiplier, s32 &pos) {

	_MY_ASSERT(multiplier >= 1,return false);
	T result = 0;
	if (src.length() <= 0)
		return false;

	pos = src.length()-1;
	s32 dot_pos = -1;

	for (; pos >= 0; pos--) {
		if (src[pos] == '.') {
			dot_pos = pos;
			break;

		} else if (src[pos] < '0' || src[pos] > '9') {
			break;
		}
	}

	T mul = 1;
	if (dot_pos >= 0) {
		pos = src.length()-1;
		for (; pos >= 0; pos--) {
			if (src[pos] == '.') {
				break;

			} else if (src[pos] < '0' || src[pos] > '9') {
				break;
			}
			if (pos > 0) {
				if (!math::multiplication_is_safe<T>(mul,10)) return false;
				mul *= 10;
			}
		}
	}

	pos = src.length()-1;

	if (dot_pos >= 0) {
		while (pos >= 0 && mul > multiplier) {
			pos -= 1;
			mul /= 10;
		}
		mul = multiplier / mul;

	} else {
		mul = multiplier; // точки нет - сразу умножаем на множитель
	}
	// main loop
	for (; pos >= 0; pos--) {
		if (src[pos] == '.'){;} // miss dot
		else if (src[pos] < '0' || src[pos] > '9') {
			break;
		} else {
			T v = src[pos] - '0';
			if (!math::multiplication_is_safe<T>(v,mul)) return false;
			T mx = v * mul;
			if (!math::addition_is_safe<T>(result,mx)) return false;
			result += mx;
			if (pos > 0) {
				if (!math::multiplication_is_safe<T>(mul,10)) return false;
				mul *= 10;
			}
		}
	}
	res = result;
	return true;
}


template<>
bool conv_utils::ftoa<s32>(s32 value, volatile_string_t &dst, s32 divider) {
	bool sign = value < 0;
	if (sign) value = -value;
	bool ret = __ftoa<s32>(value,dst,divider);
	if (ret && sign) {
		_CHECK_STR(dst,insert_at(0, '-'));
	}
	return ret;
}

template<>
bool conv_utils::ftoa<u32>(u32 value, volatile_string_t &dst, u32 divider) {
	return __ftoa<u32>(value,dst,divider);
}

template<>
bool conv_utils::atof<s32>(string_t src, s32 &res, s32 multiplier) {
	if (src.length() <= 0)
		return false;
	s32 pos = -1;
	bool ret = __atof<s32>(src,res,multiplier, pos);
	if (ret && pos >= 0 && src[pos] == '-')
		res = -res;
	return ret;
}


template<>
bool conv_utils::atof<u32>(string_t src, u32 &res, u32 multiplier) {
	if (src.length() <= 0)
		return false;
	s32 pos = -1;
	bool ret = __atof<u32>(src,res,multiplier, pos);
	if (ret && pos >= 0 && src[pos] == '-')
		return false;
	return ret;
}

template
bool conv_utils::__ftoa<s32>(s32 value, volatile_string_t &dst, s32 divider);
template
bool conv_utils::__ftoa<u32>(u32 value, volatile_string_t &dst, u32 divider);
template
bool conv_utils::__atof<u32>(string_t src, u32 &res, u32 multiplier, s32 &pos);
template
bool conv_utils::__atof<s32>(string_t src, s32 &res, s32 multiplier, s32 &pos);

} //ns myvi
