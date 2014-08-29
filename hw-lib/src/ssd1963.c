//=============================================================================
// SSD1963 driver for STM32 microcontrollers
// (c) Radoslaw Kwiecien, radek@dxp.pl	
// http://en.radzio.dxp.pl/ssd1963/
//=============================================================================
//#include "stm32f10x.h"
#include "ssd1963.h"
#include "types.h"


//=============================================================================
// Write command to SSD1963
//=============================================================================
void SSD1963_WriteCommand(u8 commandToWrite);
//=============================================================================
// Write data to SSD1963
//=============================================================================
void SSD1963_WriteData(u8 dataToWrite);
//=============================================================================
// 
//=============================================================================
void SSD1963_Init(void) {
	volatile u32 dly;
	//SSD1963_CTRLPORT->BRR = SSD1963_PIN_RST;
	//for(dly = 0; dly < 1000; dly++);
	//SSD1963_CTRLPORT->BSRR = SSD1963_PIN_RST;
	//for(dly = 0; dly < 1000; dly++);

	SSD1963_WriteCommand(SSD1963_SOFT_RESET);

	SSD1963_WriteCommand(SSD1963_SET_PLL_MN);
	SSD1963_WriteData(49); // PLLclk = REFclk * 50 (500MHz)
	SSD1963_WriteData(4); // SYSclk = PLLclk / 5  (100MHz)
	SSD1963_WriteData(4); // dummy

	SSD1963_WriteCommand(SSD1963_SET_PLL);
	SSD1963_WriteData(0x01);

	for (dly = 0; dly < 100000; dly++);

	SSD1963_WriteCommand(SSD1963_SET_PLL);
	SSD1963_WriteData(0x03);

	SSD1963_WriteCommand(SSD1963_SET_LCD_MODE);
	SSD1963_WriteData(0x0C);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(mHIGH((TFT_WIDTH-1)));
	SSD1963_WriteData(mLOW((TFT_WIDTH-1)));
	SSD1963_WriteData(mHIGH((TFT_HEIGHT-1)));
	SSD1963_WriteData(mLOW((TFT_HEIGHT-1)));
	SSD1963_WriteData(0x000000);

	SSD1963_WriteCommand(SSD1963_SET_PIXEL_DATA_INTERFACE);

#ifdef RGB565_MODE
	SSD1963_WriteData(SSD1963_PDI_16BIT565);
#else
	SSD1963_WriteData(SSD1963_PDI_8BIT);
#endif

	SSD1963_WriteCommand(SSD1963_SET_LSHIFT_FREQ);
	SSD1963_WriteData((LCD_FPR >> 16) & 0xFF);
	SSD1963_WriteData((LCD_FPR >> 8) & 0xFF);
	SSD1963_WriteData(LCD_FPR & 0xFF);

	SSD1963_WriteCommand(SSD1963_SET_HORI_PERIOD);
	SSD1963_WriteData(mHIGH(TFT_HSYNC_PERIOD)); //524
	SSD1963_WriteData(mLOW(TFT_HSYNC_PERIOD));
	SSD1963_WriteData(mHIGH((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH))); // 42
	SSD1963_WriteData(mLOW((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
	SSD1963_WriteData(TFT_HSYNC_PULSE); // 7
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);

	SSD1963_WriteCommand(SSD1963_SET_VERT_PERIOD);
	SSD1963_WriteData(mHIGH(TFT_VSYNC_PERIOD)); //285
	SSD1963_WriteData(mLOW(TFT_VSYNC_PERIOD));
	SSD1963_WriteData(mHIGH((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH))); // 11
	SSD1963_WriteData(mLOW((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
	SSD1963_WriteData(TFT_VSYNC_PULSE); // 9
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);

	SSD1963_WriteCommand(SSD1963_SET_DISPLAY_ON); //SET display on
}
//=============================================================================
//
//=============================================================================
#pragma CODE_SECTION (SSD1963_SetArea, "ramfuncs")
void SSD1963_SetArea(u16 sx, u16 ex, u16 sy, u16 ey) {
	SSD1963_WriteCommand(SSD1963_SET_COLUMN_ADDRESS);
	SSD1963_WriteData((sx >> 8) & 0xFF);
	SSD1963_WriteData((sx >> 0) & 0xFF);
	SSD1963_WriteData((ex >> 8) & 0xFF);
	SSD1963_WriteData((ex >> 0) & 0xFF);

	SSD1963_WriteCommand(SSD1963_SET_PAGE_ADDRESS);
	SSD1963_WriteData((sy >> 8) & 0xFF);
	SSD1963_WriteData((sy >> 0) & 0xFF);
	SSD1963_WriteData((ey >> 8) & 0xFF);
	SSD1963_WriteData((ey >> 0) & 0xFF);
}

#pragma CODE_SECTION (SSD1963_WriteColor, "ramfuncs")
void SSD1963_WriteColor(u32 color) {
	u8 r = ((color >> 16) & 0xff) >> 3;
	u8 g = ((color >> 8) & 0xff) >> 2;
	u8 b = ((color) & 0xff) >> 3;
#ifdef RGB565_MODE
	u16 rgb = (((u16)r) << 11) | ((((u16)g) & 0x3f) << 5) | (((u16)b) & 0x1f);
	SSD1963_WriteData(rgb);
#else
	SSD1963_WriteData(r);
	SSD1963_WriteData(g);
	SSD1963_WriteData(b);
#endif
}
//=============================================================================
// Fill area of specified color
//=============================================================================
void SSD1963_FillArea(u16 sx, u16 ex, u16 sy, u16 ey, u32 color) {
	s32 i;
	SSD1963_SetArea(sx, ex, sy, ey);
	SSD1963_WriteCommand(SSD1963_WRITE_MEMORY_START);

	s32 end = ((s32)(ex-sx+1))*((s32)(ey-sy+1));
	for (i = 0; i < end; i++) {
		SSD1963_WriteColor(color);
	}
}
//=============================================================================
// Fills whole screen specified color
//=============================================================================
void SSD1963_ClearScreen(u32 color) {
	u16 x, y;
	SSD1963_SetArea(0, TFT_WIDTH - 1, 0, TFT_HEIGHT - 1);
	SSD1963_WriteCommand(0x2c);
	for (x = 0; x < TFT_WIDTH; x++) {
		for (y = 0; y < TFT_HEIGHT; y++) {
			SSD1963_WriteColor(color);
		}
	}
}
//=============================================================================
//
//=============================================================================
void GLCD_SetPixel(u16 x, u16 y, u32 color) {
	SSD1963_SetArea(x, x, y, y);
	SSD1963_WriteCommand(0x2c);
	SSD1963_WriteColor(color);
	SSD1963_WriteCommand(0x0);
}
//=============================================================================
//
//=============================================================================
