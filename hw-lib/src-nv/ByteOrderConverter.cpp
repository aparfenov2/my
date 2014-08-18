/*
 * ByteOrderConverter.cpp
 *
 *  Created on: 28.02.2012
 *      Author: pushkarev
 */

#include "ByteOrderConverter.h"

using namespace hw;

//=============================================================================
/// Конструктор класса
//=============================================================================
ByteOrderConverter::ByteOrderConverter()
    : data(0),
      cnt(0),
      byte_order(eboHighFirst),
      cur_index(0)
{
}

//=============================================================================
/// Функция для проверки достижения конца массива
///\return 1, если конец достигнут, иначе - 0
///
//=============================================================================
u16 ByteOrderConverter::isEnd()
{
    return (byte_order == eboHighFirst) ? cur_index > cnt : cur_index >= cnt;
}

//=============================================================================
/// Чтение байта
//=============================================================================
u16 ByteOrderConverter::read()
{
    if (!data || isEnd())
        return 0;

    if (byte_order == eboHighFirst)
        return __byte(data, cnt - cur_index++);
    
    return __byte(data, cur_index++);
}

//=============================================================================
/// Запись байта
//=============================================================================
void ByteOrderConverter::write(u16 byte_value)
{
    if (!data || isEnd())
        return;

    if (byte_order == eboHighFirst)
        __byte(data, cnt - cur_index++) = byte_value & 0xFF;
    else
        __byte(data, cur_index++) = byte_value & 0xFF;
}

//=============================================================================
/// Установить конфигурацию
//=============================================================================
void ByteOrderConverter::setConfig(void* data_ptr, u16 data_cnt, eByteOrder w)
{
    data = (int*)data_ptr;
    cnt  = data_cnt;
    byte_order   = w;

    if (byte_order == eboHighFirst)
        cur_index = 1;
    else
        cur_index = 0;
}
