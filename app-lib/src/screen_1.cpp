#include "screen_1.h"

#include "resources.h"

#include "app_events.h"
#include "assert_impl.h"

using namespace myvi;

extern resources_t res;



void screen_1_t::init() {

//	globals::gui_debug = true;
	w = 480;
	h = 272;

	button_context_t bctx1;
	bctx1.bk_sel_color = 0x292929; // gray
	bctx1.bk_color = 0x203E95; // blue

	label_context_t lctx1;
	lctx1.font = &res.ttf;
	lctx1.font_size = font_size_t::FS_8;

	label_context_t lctxg;
	lctxg.font = &res.gly;
	lctxg.font_size = font_size_t::FS_30;

	menu_context_t::instance.bctx1 = bctx1;
	menu_context_t::instance.lctx1 = lctx1;
	menu_context_t::instance.lctxg = lctxg;

	init_children();

// layout buttons
	right_menu_set.x = 480 - 55;
	right_menu_set.y = 1;
	right_menu_set.menu_layout.bh = 42; // высота кнопки
	right_menu_set.h = 272 - 2 - right_menu_set.y;
	right_menu_set.w = 48;
	right_menu_set.menu_layout.spy = 3; // расстояние между кнопками

// double_labels

	top_row.x = 8;
	top_row.y = 1;
	top_row.menu_layout.bw = 35;
	top_row.h = 25;
	top_row.w = 480 - top_row.x;
	top_row.menu_layout.spx = 5;
	top_row.menu_layout.vertical = false;

	bot_row.x = 8;
	bot_row.y = 272 - 27;
	bot_row.menu_layout.bw = 35;
	bot_row.h = 25;
	bot_row.w = 480 - top_row.x;
	bot_row.menu_layout.spx = 5;
	bot_row.menu_layout.vertical = false;

	oscil.x = 8;
	oscil.y = 30;
	oscil.w = 480 - 70;
	oscil.h = 272 - 60;


	globals::dialogs::init();
	set_preferred_size_children();
	do_layout();
	dirty = true;
}

void globals::dialogs::show(gobject_t *dlg) {
	globals::modal_overlay.push_modal(dlg);
}

void globals::dialogs::init() {
	dialog_t::instance.x = 300;
	dialog_t::instance.y = 200;
	//dialog_t::instance.w = 200;
	//dialog_t::instance.h = 100;
	dialog_t::instance.init();
	menu_context_t &ctx = menu_context_t::instance;
	dialog_t::instance.lab1.visible = true;
	dialog_t::instance.lab1.text = string_t("ЗАПУСТИТЕ");
	dialog_t::instance.lab2.visible = true;
	dialog_t::instance.lab2.text = string_t("ИЗМЕРЕНИЕ");
	dialog_t::instance.lab3.visible = true;
	dialog_t::instance.lab3.ctx = ctx.lctxg;
	dialog_t::instance.lab3.ctx.font_size = font_size_t::FS_30;
	dialog_t::instance.lab3.glyph_code = 0x0032;
	dialog_t::instance.set_preferred_size();

	input_dialog_t::instance.init();
	input_dialog_t::instance.x = 300;
	input_dialog_t::instance.y = 200;


//	modal_dialogs_t::show(&modal_dialogs_t::input_dialog);
	//input_dialog.w += 10;
	//input_dialog.h += 10;

}

void screen_1_t::mouse_event(s32 mx, s32 my, mkey_t::mkey_t mkey) {
	if (mkey == mkey_t::MK_1 && mx && my && input_dialog_t::instance.parent) {
		if (dlg_catched) {
			// move dialog_t::instance
			// calc ax,ay
			s32 ax1 = mx-dlg_catched_dx;
			s32 ay1 = my-dlg_catched_dy;
			// curr pos
			s32 ax, ay;
			input_dialog_t::instance.translate(ax, ay);
			input_dialog_t::instance.x += ax1-ax;
			input_dialog_t::instance.y += ay1-ay;
			input_dialog_t::instance.dirty = true;
			return;
		}
		s32 ax, ay;
		input_dialog_t::instance.translate(ax, ay);
		if (ax <= mx && mx < ax + input_dialog_t::instance.w && ay <= my && my < ay + input_dialog_t::instance.h) {
			_MY_ASSERT(!dlg_catched,return);
			dlg_catched = true;
			dlg_catched_dx = mx-ax;
			dlg_catched_dy = my-ay;
			return;
		}
		//gobject_t *bt = right_menu_set.current_menu->next(0);
		//while (bt) {
		//	s32 ax, ay;
		//	bt->translate(ax, ay);
		//	if (bt->can_be_selected && ax <= mx && mx < ax + bt->w && ay <= my && my < ay + bt->h) {
		//		focus_manager.select(bt);
		//		return;
		//	}
		//	bt = main_menu.next(bt);
		//}
	} else {
		dlg_catched = false;
	}
}
