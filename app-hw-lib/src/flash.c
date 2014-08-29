/*
 * flash.c
 *
 *  Created on: 29.08.2014
 *      Author: user
 */


#include "types.h"
#include "disp_def.h"

#include "Flash2833x_API_Library.h"
#include "assert_impl.h"
#include "flash.h"

void SSD1963_WriteCommand(unsigned int commandToWrite);
void SSD1963_WriteData(unsigned int dataToWrite);
void SSD1963_SetArea(u16 sx, u16 ex, u16 sy, u16 ey);
void SSD1963_WriteColor(u32 color);
void SSD1963_ClearScreen(u32 color);
void c_int00(void);

#define SSD1963_WRITE_MEMORY_START		0x2C


extern Uint32 Flash_CPUScaleFactor;

static u8 allow_pixel = 0;

#pragma CODE_SECTION (MyCallbackFunction, "ramfuncs")
static void MyCallbackFunction(void) {
	if (allow_pixel) {
		SSD1963_WriteColor(0x00FF00);
		SSD1963_WriteColor(0x000000);
		SSD1963_WriteColor(0x000000);
		SSD1963_WriteColor(0x000000);
		allow_pixel = 0;
	}
}


#pragma CODE_SECTION (flash_it, "ramfuncs")
void flash_it(section_t *first) {

	FLASH_ST FlashStatus;

	SSD1963_ClearScreen(0x000000);
	allow_pixel = 1;

	SSD1963_SetArea(0, TFT_WIDTH - 1, 0, TFT_HEIGHT - 1);
	SSD1963_WriteCommand(SSD1963_WRITE_MEMORY_START);

	Flash_CPUScaleFactor = SCALE_FACTOR;
	Flash_CallbackPtr = &MyCallbackFunction;

   Uint16  Status = STATUS_SUCCESS;
   Uint16  VersionHex;     // Version of the API in decimal encoded hex

   VersionHex = Flash_APIVersionHex();
   _MY_ASSERT(VersionHex == 0x0210, return);

   allow_pixel = 1;
   Status = Flash_Erase(SECTORA,&FlashStatus);
   allow_pixel = 1;
   Status = Flash_Erase(SECTORB,&FlashStatus);
   allow_pixel = 1;
   Status = Flash_Erase(SECTORC,&FlashStatus);
   allow_pixel = 1;
   Status = Flash_Erase(SECTORD,&FlashStatus);
   allow_pixel = 1;
   Status = Flash_Erase(SECTORE,&FlashStatus);
   allow_pixel = 1;
   Status = Flash_Erase(SECTORF,&FlashStatus);
   allow_pixel = 1;
   Status = Flash_Erase(SECTORG,&FlashStatus);
   allow_pixel = 1;
   Status = Flash_Erase(SECTORH,&FlashStatus);

   if (Status == STATUS_SUCCESS) {
	   section_t *sec = first;
	   while (sec) {
		   allow_pixel = 1;
		   Status = Flash_Program((Uint16 *)sec->flash_addr, (Uint16 *)sec->data,sec->len,&FlashStatus);
		   _WEAK_ASSERT(Status == STATUS_SUCCESS, break);
		   sec = sec->next;
	   }
   }

   if (Status != STATUS_SUCCESS) {
	   _HALT();
   }
//   _HALT();
   typedef void (*reset_vec_f)(void);
   reset_vec_f reset = (reset_vec_f )0x3ff9ce;
   reset();
}
