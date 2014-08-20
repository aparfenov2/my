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
/// ����� ������������� ��� SPI
class ByteOrderConverter
{
public:
    /// ��� ��������
    enum eByteOrder
    {
        eboHighFirst,          ///< ������ ��������/������� ������� ����
        eboLowFirst            ///< ������ ��������/������� ������� ����
    };

private:
    int*        data;           ///< ��������� �� ������ ������ ��� ������/������
    u16         cnt;            ///< ���-�� ���� ��� ������/������
    eByteOrder  byte_order;     ///< ��� ��������
    u16         cur_index;      ///< ������ �������� �����

public:
    ByteOrderConverter();
    u16 isEnd();

    u16 read();
    void write(u16 byte_value);

    void setConfig(void* data_ptr, u16 data_cnt, eByteOrder bo);

};

}

#endif /* BYTEORDERCONVERTER_H_ */
