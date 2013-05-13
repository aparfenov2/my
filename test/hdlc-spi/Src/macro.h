/*
 * macro.h
 *
 *  Created on: 30.03.2012
 *      Author: pushkarev
 */

#ifndef MACRO_H_
#define MACRO_H_

#define _ASSERT(v) do {\
    if(!(v))\
        asm(" ESTOP0 "); \
    }while(0)

// ������ ������������ ��� �������� ���������� ������� �� ����� ����������. �������� ������
// � �����������. ���� ������� � ������� ������� ����� - ���������� ���������� �������:
// ������: class "CompileTimeAssert<false>" has no member "assert"
template <bool val>
struct CompileTimeAssert
{
};

template<>
struct CompileTimeAssert<true>
{
    static void assert(){}
};

// ������, ������������ ������ CompileTimeAssert
#define  _STATIC_ASSERT(v) CompileTimeAssert<(v)>::assert()

// ������:
//  _STATIC_ASSERT(cFramUsed < 1000);
#endif /* MACRO_H_ */
