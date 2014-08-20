#ifndef SPI_H
#define SPI_H

#include "types.h"
#include "ByteOrderConverter.h"
//#include "DSP2833x_Spi.h"


interrupt void SpiTxInt (void);
void SpiTx (void);

#define SPI_TXFIFO_SIZE     16
#define SPI_RXFIFO_SIZE     16

namespace hw {

class Spi
{
public:
	enum eChannel
	{							 //     210
		FRAM 	       = 0x00400000, // 001
		Flash		   = 0x00000000 // 000
	};

private:
    ByteOrderConverter  converter;              ///< тасовщик байтов
    eChannel channel;

private:
    void Setup (eChannel channel);
	friend void SpiTxInt(void);
    bool writeRaw (void *data, u16 length, eChannel channel,ByteOrderConverter::eByteOrder w = ByteOrderConverter::eboHighFirst);
    bool writeRaw2 (void *data, u16 length, eChannel channel,ByteOrderConverter::eByteOrder w = ByteOrderConverter::eboHighFirst);

public:
	void Init (void);
    bool Write (void *data, u16 length, eChannel channel,ByteOrderConverter::eByteOrder w = ByteOrderConverter::eboHighFirst);
    bool Write (void *cmd,  u16 cmd_len, void *data, u16 data_len, eChannel channel,ByteOrderConverter::eByteOrder cmd_order = ByteOrderConverter::eboHighFirst, ByteOrderConverter::eByteOrder data_order = ByteOrderConverter::eboLowFirst);
    bool Read (void *cmd, u16 cmd_len, void *data, u16 data_len, eChannel channel,ByteOrderConverter::eByteOrder w = ByteOrderConverter::eboHighFirst, ByteOrderConverter::eByteOrder r = ByteOrderConverter::eboLowFirst);
    bool Write2 (void *cmd,  u16 cmd_len, void *data, u16 data_len, eChannel channel,ByteOrderConverter::eByteOrder cmd_order = ByteOrderConverter::eboHighFirst, ByteOrderConverter::eByteOrder data_order = ByteOrderConverter::eboLowFirst);
    bool Read2 (void *cmd, u16 cmd_len, void *data, u32 data_len, eChannel channel,ByteOrderConverter::eByteOrder w = ByteOrderConverter::eboHighFirst, ByteOrderConverter::eByteOrder r = ByteOrderConverter::eboLowFirst);
	void Set_spi_channel( eChannel channel );
};

//extern Spi spi;

} // ns

#endif


