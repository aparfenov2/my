#include "menu.h"

#define CALC_Y_OFFS(pc,h) (((h) * (pc)) / 100)
#define CALC_X_OFFS(w,ws) (((w)-(ws))/2)
/*
void double_label_t::render(surface_t &dst) {
	s32 ax = 0, ay = 0;
	translate(ax,ay);
	dst.ctx.reset();
	//dst.ctx.alfa = 0xff;
	//dst.ctx.pen_color = 0x203E95;
	//dst.fill(ax,ay,w,h);
	dst.ctx.pen_color = 0xffffff;
//	dst.rect(ax,ay,w,h);

	s32 y1 = CALC_Y_OFFS(5,h);
	s32 y2 = CALC_Y_OFFS(50,h);

	if (l_top.visible) {
		l_top.adjust_self_size();
		l_top.x = w-l_top.w-2;
		l_top.y = y1;
	}
	if (l_bot.visible) {
		l_bot.adjust_self_size();
		l_bot.x = w-l_bot.w-2;
		l_bot.y = y2;
	}

	render_children(dst);
}

*/
