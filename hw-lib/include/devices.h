#ifndef __MONITOR_DEVICES__
#define __MONITOR_DEVICES__

#include "types.h"

//#include "mcbsp_spi.h"

// #define		SIZEOF_FRAM		0xFFF

//#define CPU_RATE    6.667L   // for a 150MHz CPU clock speed (SYSCLKOUT)
//#define Delay_us(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)



class FRAM
{
public:
	bool Write ( u16 address, s16 data );
	bool Write ( u16 address, s32 data );
	void Read  ( u16 address, s16 *data );
	void Read  ( u16 address, s32 *data );
	void rdsr  ( s16 *data );

	bool Write ( u16 address, s16 *data, u16 len );
	void Read  ( u16 address, s16 *data, u16 len );
	bool Write2 ( u16 address, s16 *data, u16 len );
	void Read2  ( u16 address, s16 *data, u16 len );

//private:
	void WriteEnable ( void );
};


	


//===------------------------------------------------------------------------------

class FlashDev
{
public:
	struct DataID
	{
		u32		manufacturerID:8;
		u32		memoryType:8;
		u32		memoryCapacity:8;
		u32		empty:8;
	};
	struct StatDataReg 
	{
		u16		WIP:1;	// Write In Progress
		u16		WEL:1;	// Write Enable Latch
		u16		BP:3;	// Block Protect
		u16		Null:2;	// Zeros
		u16		SRWD:1;	// Status Register Write Protect 
	};
public:
	void WriteEnable (void);
	void WriteDisable (void);
	void ReadID (DataID &data);
	void ReadStatusReg (StatDataReg &data);
	void WriteStatusReg (StatDataReg &data);
	u16 ReadData (u32 addr);
	u16 ReadData2 (u32 addr);
	void ReadData2 (u32 addr, u16* dst, u32 len);
	void PageProgram (u32 addr, u16 data);
	void PageProgram2 (u32 addr, u16 data);
	void PageProgram2 (u32 addr, u16* src, u16 len);
	void SectorErase (u32 addr);
	void BulkErase (void);
	bool isBusy (void);
};



//extern DAC 			dac;
//extern DDS			dds;
//extern FRAM			fram;
//extern Heterodin	heterodin;
//extern FlashDev		flash;

#endif
