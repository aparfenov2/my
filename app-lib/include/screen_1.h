#ifndef _SCR_1_H
#define _SCR_1_H

#include "widgets.h"
#include "app_events.h"
#include "menu.h"

namespace myvi {




class label_distance_t : public gobject_t {
public:
	label_t lab1;
	label_t lab2;
	label_t lab3;
	center_layout_t center_layout;
	preferred_stack_layout_t preferred_stack_layout;
public:
	label_distance_t() {
		layout = &center_layout;
		preferred_layout = &preferred_stack_layout;
	}

	virtual void render(surface_t &dst) OVERRIDE {
		_MY_ASSERT(lab3.visible,return);
		s32 ax,ay;
		translate(ax,ay);
		dst.ctx = lab3.ctx.sctx;
		dst.line(ax,ay+lab3.y+1,w,false);
//		render_children(dst);
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lab1;
		else if (prev == &lab1) return &lab2;
		else if (prev == &lab2) return &lab3;
		else return 0;
	}
};


class mks1500_part_t : public gobject_t {
public:
	label_t lab1;
	label_t lab2;
	center_layout_t center_layout;
	preferred_stack_layout_t pref_layout;
public:
	mks1500_part_t() {
		layout = &center_layout;
		preferred_layout = &pref_layout;
	}


	virtual void init() OVERRIDE {
		gobject_t::init();
		lab1.visible = true;
		lab1.ctx = menu_context_t::instance.lctx1;
		lab1.text = string_t("1500 M");
		lab2.visible = true;
		lab2.ctx = menu_context_t::instance.lctx1;
		lab2.text = string_t("10,00 MKC");
	}


	virtual void render(surface_t &dst) OVERRIDE {
		s32 ax,ay;
		translate(ax,ay);
		dst.ctx = lab1.ctx.sctx;
		dst.line(ax,ay+h/2,w,false);
//		render_children(dst);
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lab1;
		else if (prev == &lab1) return &lab2;
		return 0;
	}
};

class stat_m_part_t : public gobject_t {
public:
	label_t lab1;
	mks1500_part_t mks1500_part;
	stack_layout_t stack_layout;
	preferred_stack_layout_t pref_layout;
public:
	stat_m_part_t() {
		pref_layout.vertical = false;
		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;

		layout = &stack_layout;
		preferred_layout = &pref_layout;
	}


	virtual void init() OVERRIDE {
		gobject_t::init();
		lab1.visible = true;
		lab1.ctx = menu_context_t::instance.lctx1;
		lab1.ctx.font_size = font_size_t::FS_30;
		lab1.text = string_t("M");
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lab1;
		else if (prev == &lab1) return &mks1500_part;
		return 0;
	}
};

class statistics_t : public gobject_t , public subscriber_t<model_changed_msg_t>{
public:
	label_t lab1;
	stat_m_part_t stat_m_part;
	label_t lab2;
	label_t lab3;

	stack_layout_t stack_layout;
	preferred_stack_layout_t pref_layout;
private:
	string_impl_t<INPUT_MAX_LEN> ev_range_s;
	string_impl_t<INPUT_MAX_LEN> ev_delay_s;
	string_impl_t<INPUT_MAX_LEN> sigma_s;
	string_impl_t<INPUT_MAX_LEN> sko_s;

public:
	statistics_t() {
		stack_layout.preferred_item_size = true;
		layout = &stack_layout;
		preferred_layout = &pref_layout;
	}


	virtual void init() OVERRIDE {
		gobject_t::init();

		app_model_t::instance.subscribe(this);

		lab1.visible = true;
		lab1.ctx = menu_context_t::instance.lctx1;
		lab1.text = string_t("—“¿“»—“» ¿");
		lab2.visible = true;
		lab2.ctx = menu_context_t::instance.lctx1;
		lab2.text = string_t("q = 0,00");
		lab3.visible = true;
		lab3.ctx = menu_context_t::instance.lctx1;
		lab3.text = string_t("CKO = 0,00");
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		if (msg.what == model_changed_what_t::STATISTICS) {

			if (!conv_utils::ftoa<u32>(app_model_t::instance.staticstics.ev_range,ev_range_s,1)) 
				return;
			ev_range_s += string_t(" Ã");
			stat_m_part.mks1500_part.lab1.text = ev_range_s;

			if (!conv_utils::ftoa<u32>(app_model_t::instance.staticstics.ev_delay.value,ev_delay_s,units::MKS))
				return;
			ev_delay_s += string_t(" Ã —");
			stat_m_part.mks1500_part.lab2.text = ev_delay_s;

			if (!conv_utils::ftoa<s32>(app_model_t::instance.staticstics.sigma.value,sigma_s,units::INT))
				return;
			sigma_s.insert_at(0,string_t("q = "));
			lab2.text = sigma_s;

			if (!app_model_t::instance.staticstics.sko.ftoa(sko_s,units::INT))
				return;
			sko_s.insert_at(0,string_t("CKO = "));
			lab3.text = sko_s;

			dirty = true;
		}
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lab1;
		else if (prev == &lab1) return &stat_m_part;
		else if (prev == &stat_m_part) return &lab2;
		else if (prev == &lab2) return &lab3;
		return 0;
	}
};

class kod_co_t : public gobject_t, public subscriber_t<model_changed_msg_t> {
public:
	label_t lab1;
	label_t lab2;
	label_t lab3;

	stack_layout_t stack_layout;
	preferred_stack_layout_t pref_layout;
public:
	kod_co_t() {
		stack_layout.preferred_item_size = true;
		layout = &stack_layout;
		preferred_layout = &pref_layout;
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		gobject_t::set_dirty(dirty);
		parent->set_dirty(dirty);
	}

	virtual void init() OVERRIDE {
		gobject_t::init();
		app_model_t::instance.subscribe(this);

		lab1.visible = true;
		lab1.ctx = menu_context_t::instance.lctx1;
		lab1.text = string_t(" Œƒ CO:");
		lab2.visible = true;
		lab2.ctx = menu_context_t::instance.lctx1;
		lab2.text = string_t("ÃÕ—");
		lab3.visible = true;
		lab3.ctx = menu_context_t::instance.lctx1;
		lab3.text = string_t("Ã¿Õ¿—");
	}


	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		if (msg.what == model_changed_what_t::CO_CODE) {
			lab2.text = app_model_t::instance.co_code.code;
			lab3.text = app_model_t::instance.co_code.airport;
			dirty = true;
			set_preferred_size();
			do_layout();
		}
	}


	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lab1;
		else if (prev == &lab1) return &lab2;
		else if (prev == &lab2) return &lab3;
		return 0;
	}
};


class oscil_t : public gobject_t, public subscriber_t<model_changed_msg_t> {
public:
	double_label_t dl_effect;
	label_distance_t label_distance;
	statistics_t statistics;
	kod_co_t kod_co;

private:
	string_impl_t<INPUT_MAX_LEN> dl_effect_value;
	string_impl_t<INPUT_MAX_LEN> rsp_delay_s;
	string_impl_t<INPUT_MAX_LEN> range_s;

public:

	virtual void init() OVERRIDE {
		gobject_t::init();
		app_model_t::instance.subscribe(this);

		dl_effect.x = 20-8;
		dl_effect.y = 30-20;

		dl_effect.l_top.visible = true;
		dl_effect.l_top.ctx = menu_context_t::instance.lctx1;
		dl_effect.l_top.ctx.sctx.reset();
		dl_effect.l_top.ctx.font_size = font_size_t::FS_8;
		dl_effect.l_top.text = string_t("›‘‘≈ “»¬ÕŒ—“‹");

		dl_effect.l_bot.visible = true;
		dl_effect.l_bot.ctx = menu_context_t::instance.lctx1;
		dl_effect.l_bot.ctx.sctx.reset();
		dl_effect.l_bot.ctx.font_size = font_size_t::FS_30;
		dl_effect.l_bot.text = string_t("100 %");



		label_distance.x = 250-8;
		label_distance.y = 25-20;
		label_distance.center_layout.spy = 1;

		label_distance.lab1.visible = true;
		label_distance.lab1.ctx = menu_context_t::instance.lctx1;
		label_distance.lab1.ctx.font_size = font_size_t::FS_15;
		label_distance.lab1.text = string_t("ƒ¿À‹ÕŒ—“‹");

		label_distance.lab2.visible = true;
		label_distance.lab2.ctx = menu_context_t::instance.lctx1;
		label_distance.lab2.ctx.font_size = font_size_t::FS_30;
		label_distance.lab2.text = string_t("1500 M");

		label_distance.lab3.visible = true;
		label_distance.lab3.ctx = menu_context_t::instance.lctx1;
		label_distance.lab3.ctx.font_size = font_size_t::FS_15;
		label_distance.lab3.text = string_t("10,00 Ã —");


		statistics.x = 20-8;
		statistics.y = 100-20;

		kod_co.x = label_distance.x;
		kod_co.y = 100-20;

		set_preferred_size_children();
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &dl_effect;
		if (prev == &dl_effect) return &label_distance;
		if (prev == &label_distance) return &statistics;
		if (prev == &statistics) return &kod_co;
		
		return 0;
	}

	virtual void render(surface_t &dst) OVERRIDE {

		s32 ax, ay;
		translate(ax,ay);
		dst.ctx.reset();
		dst.ctx.pen_color = 0x292929;
		dst.fill(ax,ay,w,h); // 8,25,250,200

		u32 dpx = app_model_t::instance.osc.dpx.value;
		u32 dpy = app_model_t::instance.osc.dpy.value / units::INT;
		s32 maxy = app_model_t::instance.osc.disp_buf->maxy;

		s32 last_x = 0, last_y = h, cx = 0, cy = 0;
// ÒÂÚÍ‡
		dst.ctx.pen_color = 0xffffff;
		dst.ctx.alfa = 128;
		s32 cw = 26;
		s32 ch = 20;

		while(1) {
			s32 px = (cx * cw)/dpx; // pixel per dot x
			cx++;
			if (px >= w) break;
			dst.line(ax+px,ay,h,true);
		};


		while(1) {
			s32 py = cy*h/100;
			cy += 10;
			if (py >= h) break;
			py = h-py;
			dst.line(ax,ay+py,w,false);
		};

		// vert
		//for (int c=1; c*cw < w; c++)
		//	dst.line(ax+c*cw,ay,h,true);

		// hor
		//for (int r=1; r*ch < h; r++)
		//	dst.line(ax,ay+r*ch,w,false);
// ‰‡ÌÌ˚Â
		dst.ctx.pen_color = 0x00ff00;



		s32 len = app_model_t::instance.osc.disp_buf->y_data.length();

		// ‰Îˇ Í‡Ê‰ÓÈ ÚÓ˜ÍË ‰‡ÌÌ˚ı
		for (s32 i=0; i < len; i++) {

//			cx = (xv*cw)/dpx;

			cx = (i * w) / len;
			if (cx >= w)
				break;

			s32 v = app_model_t::instance.osc.disp_buf->y_data[i];
			_MY_ASSERT(v <= maxy, break);

			s32 vy = (dpy*v*h)/(100*maxy);

			if (vy > h) vy = h;
			vy = h-vy;
			if (vy >= h)
				vy = h-1;

			if (cx > last_x) {
				dst.line(ax+last_x,ay+last_y,ax+cx,ay+vy);
			}
			last_x = cx;
			last_y = vy;
		}

// ÍÛÒÓ
		if (app_model_t::instance.osc.cursor.enabled) {

			dst.ctx.pen_color = 0xff0000;

			do {
				app_model_t::time2_t 
					vx = app_model_t::instance.osc.cursor.x;
				s32 px = (vx.value * cw)/dpx; // pixel per dot x
				if (px >= w) break;

				dst.line(ax+px,ay,h,true);
				px++;
				if (px >= w) break;
				dst.line(ax+px,ay,h,true);
			} while(0);


			do {
				s32 vy = app_model_t::instance.osc.cursor.y.value / units::INT; // %
				s32 py = vy*h/100;
				if (py >= h) break;
				py = h-py;
				dst.line(ax,ay+py,w,false);
				py++;
				if (py >= h) break;
				dst.line(ax,ay+py,w,false);
			} while(0);
		}
	}


	virtual void set_dirty(bool dirty) OVERRIDE {
		gobject_t::set_dirty(dirty);
		if (globals::modal_overlay.modals.length() > 1)
			globals::modal_overlay.dirty = dirty;
	}


	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		switch (msg.what) {

		case model_changed_what_t::CURSOR :
		case model_changed_what_t::RESOLUTION_X :
		case model_changed_what_t::RESOLUTION_Y :
		case model_changed_what_t::CHART :
			dirty = true;
			break;

		case model_changed_what_t::EFFICIENCY:
			if (!conv_utils::ftoa<s32>(app_model_t::instance.efficiency,dl_effect_value,1))
				return;
			dl_effect_value += string_t(" %");
			dl_effect.l_bot.text = dl_effect_value;
			dirty = true;
			break;

		case model_changed_what_t::RSP_DELAY:
			if (!app_model_t::instance.rsp_delay.ftoa(rsp_delay_s,units::MKS))
				return;
			rsp_delay_s += string_t(" Ã —");
			label_distance.lab3.text = rsp_delay_s;
			dirty = true;
			break;

		case model_changed_what_t::RANGE:
			if (!conv_utils::ftoa<u32>(app_model_t::instance.range,range_s,1))
				return;
			range_s += string_t(" Ã");
			label_distance.lab2.text = range_s;
			dirty = true;
			break;

		default:
			break;

		}
	}

};


class screen_1_t : public gobject_t {
public:
	menu_set_t right_menu_set;
	main_top_row_t top_row;
	main_bot_row_t bot_row;

	oscil_t oscil;

	bool dlg_catched;
	s32 dlg_catched_dx;
	s32 dlg_catched_dy;

public:
	screen_1_t() {
		dlg_catched = false, dlg_catched_dx = 0, dlg_catched_dy = 0;
	}
	virtual void init() OVERRIDE;

	virtual void key_event(key_t::key_t key) OVERRIDE {
		right_menu_set.key_event(key);
	}

	void mouse_event(s32 mx, s32 my, mkey_t::mkey_t mkey);

	virtual void render(surface_t &dst) OVERRIDE {
		// ÒËÌËÈ ÙÓÌ
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0x203E95;
		dst.fill(0,0,w,h);

	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &oscil;
		if (prev == &oscil) return &right_menu_set;
		if (prev == &right_menu_set) return &top_row;
		if (prev == &top_row) return &bot_row;
		
		return 0;
	}

};

} // ns
#endif
