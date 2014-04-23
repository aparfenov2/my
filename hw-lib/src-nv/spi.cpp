#include "spi.h"     // DSP281x Headerfile Include File
#include "c28/pins_def.h"     // DSP281x Headerfile Include File
#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "DSP2833x_Examples.h"
#include "assert_impl.h"


//ВАЖНО!!!! Исторически сложилось, что при записи команды по SPI первым идет старший байт
#pragma CODE_SECTION ("ramfuncs") 
void Spi::Init(void)
{
    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0; // Enable pull-up on GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0; // Enable pull-up on GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0; // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0; // Enable pull-up on GPIO19 (SPISTEA)

    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;

    GpioCtrlRegs.GPADIR.bit.PIN_SPI_CS = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_SPI_D0 = 1; // SPI_D0  out
    GpioCtrlRegs.GPADIR.bit.PIN_SPI_D1 = 1; // SPI_D1  out
//    GpioCtrlRegs.GPADIR.bit.PIN_SPI_D2 = 1; // SPI_D2  out

    EDIS;

    channel = FRAM;
    Setup(channel);

    SpiaRegs.SPICCR.bit.SPISWRESET  = 0;            // Reset SPI
    SpiaRegs.SPICTL.all             = 0x06;         // (MASTER(bit 2) TALK (bit 1))
    SpiaRegs.SPICCR.all             = 7;            // character len = 8 bit.
    SpiaRegs.SPIFFTX.all            = 0xE040;       // TX FIFO settings
    SpiaRegs.SPIFFRX.all            = 0x2040;       // RX FIFO settings
    SpiaRegs.SPIFFCT.all            = 0;            // tramsmit delay 0
    SpiaRegs.SPIPRI.bit.FREE        = 1;
    SpiaRegs.SPICCR.bit.SPISWRESET  = 1;            // SPI to work mode

    SpiaRegs.SPIFFTX.bit.TXFIFO     = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;

    Setup(channel);

}

bool Spi::writeRaw(void *data, u16 length, eChannel channel,ByteOrderConverter::eByteOrder w)
{
    if (!data || !length)
        return false;

    u16 tx_cnt = 0;

    if (channel != this->channel)
    {
        Setup (channel);
        this->channel = channel;
    }

    converter.setConfig(data,length,w);

    // очищаю RXFIFO
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;

    while (!converter.isEnd())
    {
        // ожидаю появления свободного места в TXFIFO
        if (SpiaRegs.SPIFFTX.bit.TXFFST < 15)
            SpiaRegs.SPITXBUF = converter.read() << 8;                  // сдвиг обусловлен особенностю TX SPI (left aligned)

        // попутно веду учет переданных байт
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    // ожидаю завершения передачи
    while ( tx_cnt < length)
    {
        // попутно веду учет переданных байт
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    return true;
}

bool Spi::writeRaw2(void *data, u16 length, eChannel channel,ByteOrderConverter::eByteOrder w)
{
    if (!data || !length)
        return false;

    u16 tx_cnt = 0;

    if (channel != this->channel)
    {
        Setup (channel);
        this->channel = channel;
    }

    converter.setConfig(data,length,w);

    // очищаю RXFIFO
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;

    u16 ofs = 0;

    while (ofs < length)
    {
        // ожидаю появления свободного места в TXFIFO
        if (SpiaRegs.SPIFFTX.bit.TXFFST < 15)
            SpiaRegs.SPITXBUF = (((u16*)data)[ofs++] & 0xff) << 8;                  // сдвиг обусловлен особенностю TX SPI (left aligned)

        // попутно веду учет переданных байт
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    // ожидаю завершения передачи
    while ( tx_cnt < length)
    {
        // попутно веду учет переданных байт
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    return true;
}

// SPI модуль жутко корявый. Нет флагов, позволяющих узнать, что передача полностью выполнена.
// Можно определить, что TX FIFO пуст, но передача-то еще может идти (сдвиговый регистр не пуст)
// В качестве контроля завершения передачи будем считать принятые байты
bool Spi::Write(void *data, u16 length, eChannel channel,ByteOrderConverter::eByteOrder w)
{
    GpioDataRegs.GPACLEAR.bit.PIN_SPI_CS = 1;

    bool result = writeRaw(data, length, channel,w);

    GpioDataRegs.GPASET.bit.PIN_SPI_CS = 1;
    return result;
}

bool Spi::Write (void *cmd, u16 cmd_len, void *data, u16 data_len, eChannel channel,ByteOrderConverter::eByteOrder cmd_order, ByteOrderConverter::eByteOrder data_order)
{
    GpioDataRegs.GPACLEAR.bit.PIN_SPI_CS = 1;

    bool result = writeRaw(cmd, cmd_len, channel, cmd_order);
    result &= writeRaw(data, data_len, channel, data_order);

    GpioDataRegs.GPASET.bit.PIN_SPI_CS = 1;
    return result;

}

bool Spi::Write2 (void *cmd, u16 cmd_len, void *data, u16 data_len, eChannel channel,ByteOrderConverter::eByteOrder cmd_order, ByteOrderConverter::eByteOrder data_order)
{
    GpioDataRegs.GPACLEAR.bit.PIN_SPI_CS = 1;

    bool result = writeRaw(cmd, cmd_len, channel, cmd_order);
    result &= writeRaw2(data, data_len, channel, data_order);

    GpioDataRegs.GPASET.bit.PIN_SPI_CS = 1;
    return result;

}

bool Spi::Read(void *cmd, u16 cmd_len, void *data, u16 data_len, eChannel channel,ByteOrderConverter::eByteOrder w_order, ByteOrderConverter::eByteOrder r_order)
{
    if (!cmd || !data || !data_len)
        return false;

    u16 tx_cnt = 0;

    if (channel != this->channel)
    {
        Setup (channel);
        this->channel = channel;
    }

    converter.setConfig(cmd,cmd_len,w_order);

    GpioDataRegs.GPACLEAR.bit.PIN_SPI_CS = 1;

    // очищаю RXFIFO
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;

    while (!converter.isEnd())
    {
        // ожидаю появления свободного места в TXFIFO
        if (SpiaRegs.SPIFFTX.bit.TXFFST < 15)
            SpiaRegs.SPITXBUF = converter.read() << 8;                  // сдвиг обусловлен особенностю TX SPI (left aligned)

        // попутно веду учет переданных байт
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    // ожидаю завершения передачи
    while ( tx_cnt < cmd_len)
    {
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    // готовлюсь к считыванию данных
    converter.setConfig((int*)data, data_len, r_order);

    while (!converter.isEnd())
    {
        if (data_len && SpiaRegs.SPIFFTX.bit.TXFFST < 15)
        {
            // подкидываю пустые байты для считывания данных
            data_len--;
            SpiaRegs.SPITXBUF = 0;
        }

        // есть данные для считывания?
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
            converter.write(SpiaRegs.SPIRXBUF & 0xFF);
    }

    // Восстанавливаю CS
    GpioDataRegs.GPASET.bit.PIN_SPI_CS = 1;
    return true;
}

bool Spi::Read2(void *cmd, u16 cmd_len, void *data, u32 data_len, eChannel channel,ByteOrderConverter::eByteOrder w_order, ByteOrderConverter::eByteOrder r_order)
{
    if (!cmd || !data || !data_len)
        return false;

    u32 tx_cnt = 0;

    if (channel != this->channel)
    {
        Setup (channel);
        this->channel = channel;
    }

    converter.setConfig(cmd,cmd_len,w_order);

    GpioDataRegs.GPACLEAR.bit.PIN_SPI_CS = 1;

    // очищаю RXFIFO
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;

    while (!converter.isEnd())
    {
        // ожидаю появления свободного места в TXFIFO
        if (SpiaRegs.SPIFFTX.bit.TXFFST < 15)
            SpiaRegs.SPITXBUF = converter.read() << 8;                  // сдвиг обусловлен особенностю TX SPI (left aligned)

        // попутно веду учет переданных байт
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    // ожидаю завершения передачи
    while ( tx_cnt < cmd_len)
    {
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 dummy_data = SpiaRegs.SPIRXBUF;
            tx_cnt++;
        }
    }

    // готовлюсь к считыванию данных
//    converter.setConfig((int*)data, data_len, r_order);

    u32 ofs = 0;
    u32 len = data_len;
    while (ofs < data_len)
    {
        if (len && SpiaRegs.SPIFFTX.bit.TXFFST < 15)
        {
            // подкидываю пустые байты для считывания данных
            len--;
            SpiaRegs.SPITXBUF = 0;
        }

        // есть данные для считывания?
        if (SpiaRegs.SPIFFRX.bit.RXFFST)
            ((u16*)data)[ofs++] = (SpiaRegs.SPIRXBUF & 0xFF);
    }

    // Восстанавливаю CS
    GpioDataRegs.GPASET.bit.PIN_SPI_CS = 1;
    return true;
}

#pragma CODE_SECTION ("ramfuncs")
void Spi::Setup(eChannel channel)
{
    EALLOW;
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;

    //set spi channel
    GpioDataRegs.GPACLEAR.bit.PIN_SPI_D0 = 1;
    GpioDataRegs.GPACLEAR.bit.PIN_SPI_D1 = 1;
    const u16 spi_brr = 50;
    switch (channel)
    {
        case FRAM:
            GpioDataRegs.GPASET.bit.PIN_SPI_D0 = 1;
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case Flash:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        default:
        	_MY_ASSERT(0,return);
    }

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;
    EDIS;
}






