#include <cstdio>

#include "assert_impl.h"

extern "C" {
	void SSD1963_WriteCommand(unsigned int commandToWrite);
	void SSD1963_WriteData(unsigned int dataToWrite);
	void SSD1963_SetArea(u16 sx, u16 ex, u16 sy, u16 ey);
	void SSD1963_WriteColor(u32 color);
}

#define SSD1963_WRITE_MEMORY_START		0x2C

#pragma CODE_SECTION ("ramfuncs")
static void flash_it() {

	SSD1963_SetArea(0, 400, 0, 200);
	SSD1963_WriteCommand(SSD1963_WRITE_MEMORY_START);

	for (s32 i=0; i<10000; i++) {
		SSD1963_WriteColor(0x00FF00);
		SSD1963_WriteColor(0x000000);
		SSD1963_WriteColor(0x000000);
		SSD1963_WriteColor(0x000000);
		for (s32 j=0; j < 100000; j++);
	}
}

void my_main() {
	flash_it();
	while (1) {
	}

}
