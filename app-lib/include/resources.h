#ifndef _RES_H
#define _RES_H

#include "truetype.h"

class resources_t {
public:
	myvi::ttype_font_t ttf;
	myvi::ttype_font_t ttf_bold;
//	myvi::ttype_font_t gly;
public:
	void init();
};

#endif
