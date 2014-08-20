/*
 * ConverterByteOrder.h
 *
 *  Created on: 28.02.2012
 *      Author: pushkarev
 */

#ifndef BYTEORDERCONVERTER_H_
#define BYTEORDERCONVERTER_H_
#include "types.h"

namespace hw {
/// Класс сериализатора для SPI
class ByteOrderConverter
{
public:
    /// тип передачи
    enum eByteOrder
    {
        eboHighFirst,          ///< первым читается/пишется старший байт
        eboLowFirst            ///< первым читается/пишется младший байт
    };

private:
    int*        data;           ///< указатель на массив данных для чтения/записи
    u16         cnt;            ///< кол-ко байт для чтения/записи
    eByteOrder  byte_order;     ///< тип передачи
    u16         cur_index;      ///< индекс текущего байта

public:
    ByteOrderConverter();
    u16 isEnd();

    u16 read();
    void write(u16 byte_value);

    void setConfig(void* data_ptr, u16 data_cnt, eByteOrder bo);

};

}

#endif /* BYTEORDERCONVERTER_H_ */
