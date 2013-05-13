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

// Шаблон предназначен для проверки истинности условия во время компиляции. Работает только
// с константами. Если условие в угловых скобках ложно - компиляция завершится ошибкой:
// Ошибка: class "CompileTimeAssert<false>" has no member "assert"
template <bool val>
struct CompileTimeAssert
{
};

template<>
struct CompileTimeAssert<true>
{
    static void assert(){}
};

// макрос, использующий шаблон CompileTimeAssert
#define  _STATIC_ASSERT(v) CompileTimeAssert<(v)>::assert()

// Пример:
//  _STATIC_ASSERT(cFramUsed < 1000);
#endif /* MACRO_H_ */
