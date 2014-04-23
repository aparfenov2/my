#include "devices.h"
#include "spi.h"
#include "c28/pins_def.h"
#include "DSP2833x_device.h"
#include "DSP2833x_Examples.h"

//#include "DSP2833x_GlobalPrototypes.h"

#include "assert_impl.h"

#define cFRAM_SIZE 0x8000 // 256kbits = 32kbytes
#define _ASSERT(b) _MY_ASSERT(b,return false)

bool FRAM::Write (u16 address, s16 data)
{
    _ASSERT(address + 2 < cFRAM_SIZE);
    WriteEnable();

    s64 buf = 0x020000LL | (u64)address;
    spi.Write (&buf, 3, &data, 2, Spi::FRAM); // Write Memory Data
    return true;
}

bool FRAM::Write (u16 address, s32 data)
{
    _ASSERT(address + 4 < cFRAM_SIZE);
    WriteEnable();

    s64 buf = 0x020000LL | (u64)address;
    spi.Write (&buf, 3, &data, 4, Spi::FRAM); // Write Memory Data
    return true;
}


void FRAM::Read (u16 address, s16 *data)
{
	s64 buf = ( 0x03LL << 16  ) | ( (u64)address);

	spi.Read((void*)&buf,3,data,2,Spi::FRAM);
}

void FRAM::rdsr  ( s16 *data ) {
	s16 buf = 0x05;
	spi.Read((void*)&buf,1,data,1,Spi::FRAM);
}

void FRAM::Read (u16 address, s32 *data)
{
    s64 buf = ( 0x03LL << 16  ) | ( (u64)address);

    spi.Read((void*)&buf,3,data,4,Spi::FRAM);
}


bool FRAM::Write ( u16 address, s16 *data, u16 len )
{
    _ASSERT(address + len < cFRAM_SIZE);

    WriteEnable();

    u32 buf = 0x020000L | (u32)address;
    spi.Write (&buf, 3, data, len, Spi::FRAM); // Write Memory Data
    return true;
}
bool FRAM::Write2 ( u16 address, s16 *data, u16 len )
{
    _ASSERT(address + len < cFRAM_SIZE);

    WriteEnable();

    u32 buf = 0x020000L | (u32)address;
    spi.Write2 (&buf, 3, data, len, Spi::FRAM); // Write Memory Data
    return true;
}

void FRAM::Read  ( u16 address, s16 *data, u16 len )
{
    u32 buf = ( 0x03LL << 16  ) | ( (u64)address);
    spi.Read((void*)&buf,3,data,len,Spi::FRAM);
}

void FRAM::Read2  ( u16 address, s16 *data, u16 len )
{
    u32 buf = ( 0x03LL << 16  ) | ( (u64)address);
    spi.Read2((void*)&buf,3,data,len,Spi::FRAM);
}

void FRAM::WriteEnable (void)
{
	int buf;
	buf = 0x06;
	spi.Write (&buf, 1, Spi::FRAM);	// Set Write Enable Latch
}


//---------------------------------------------------------------------------------


void FlashDev::WriteEnable (void)
{
	u16 data = 0x06;
	spi.Write (&data, 1, Spi::Flash);
}
 

void FlashDev::WriteDisable (void)
{
	u16 data = 0x04;
	spi.Write (&data, 1, Spi::Flash);
}
 

void FlashDev::ReadID (FlashDev::DataID &data)
{
	u32 *d = (u32 *)((void*)&data);
	*d = 0x9F;
	spi.Read (d, 1, d, 3, Spi::Flash);
}
 

void FlashDev::ReadStatusReg (StatDataReg &data)
{
	u16 *d = (u16 *)((void*)&data);
	*d = 0x05L;
	spi.Read (d, 1,d, 1, Spi::Flash);
}
 

void FlashDev::WriteStatusReg (StatDataReg &data)
{
	WriteEnable ();
	u16 *d = (u16 *)((void*)&data);
	*d |= 0x0100L;
	spi.Read (d, 1,d, 1, Spi::Flash);
}
 

u16 FlashDev::ReadData (u32 addr)
{
	u64 data = ((u64)addr & 0x00ffffff) | 0x03000000LL;
	spi.Read (&data, 4,&data, 2, Spi::Flash, ByteOrderConverter::eboHighFirst, ByteOrderConverter::eboHighFirst);
	return data;
}

u16 FlashDev::ReadData2 (u32 addr)
{
	u64 data = ((u64)addr & 0x00ffffff) | 0x03000000LL;
	spi.Read (&data, 4,&data, 1, Spi::Flash);
	return data & 0xff;
}

void FlashDev::ReadData2 (u32 addr, u16* dst, u32 len)
{
	u64 data = ((u64)addr & 0x00ffffff) | 0x03000000LL;
	spi.Read2 (&data, 4,dst, len, Spi::Flash);

//	for (u32 ofs = 0; ofs < len;) {
//		dst[ofs] = ReadData2(addr);
//		addr++;
//		ofs++;
//	}

}


void FlashDev::PageProgram (u32 addr, u16 data)
{
	while (isBusy ()) { };
	WriteEnable ();
	u64 d = ((u64)addr<<16) | 0x020000000000LL | data;
//	while (spi.isBusy());
	spi.Write (&d, 6, Spi::Flash);
}

void FlashDev::PageProgram2 (u32 addr, u16 data)
{
	while (isBusy ()) { };
	WriteEnable ();
	u64 d = (((u64)addr & 0x00ffffff) << 8) | 0x0200000000LL | (data & 0xff);
//	while (spi.isBusy());
	spi.Write (&d, 5, Spi::Flash);
}

void FlashDev::PageProgram2 (u32 addr, u16* src, u16 len)
{
	while (isBusy ()) { };
	WriteEnable ();
	u64 d = ((u64)addr & 0x00ffffff) | 0x02000000LL;
//	while (spi.isBusy());
	spi.Write2 (&d, 4,src,len, Spi::Flash);
}

void FlashDev::SectorErase (u32 addr)
{
	while (isBusy ()) { };
	WriteEnable ();
	u32 d = addr | 0xD8000000L;
//	while (spi.isBusy());
	spi.Write (&d, 4, Spi::Flash);
}
 
void FlashDev::BulkErase (void)
{
	while (isBusy ()) { };
	WriteEnable ();
	u16 d = 0xC7;
//	while (spi.isBusy());
	spi.Write (&d, 1, Spi::Flash);
}
 
bool FlashDev::isBusy (void)
{
	StatDataReg dataStat;
	ReadStatusReg (dataStat);
	return dataStat.WIP;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*


*/
