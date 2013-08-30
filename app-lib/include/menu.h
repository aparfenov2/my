#ifndef _MENU_H
#define _MENU_H

#include "widgets.h"
#include "exported.h"
//#include "TRexpp.h"
#include "utils.h"
#include "app_model.h"

 
namespace myvi {

#define _1M 1000000

class menu_context_t {
public:
	button_context_t bctx1;
	label_context_t lctx1;
	label_context_t lctxg;
private:
	menu_context_t() {
	}
public:
	static menu_context_t instance;
};


template<typename T>
class dialog_handler_t {
public:
	virtual void dialog_closed(T *dlg) = 0;
};

class abstract_dialog_t : public gobject_t {
	typedef gobject_t super;
public:
	center_layout_t center_layout;
	preferred_stack_layout_t preferred_stack_layout;
	dialog_handler_t<abstract_dialog_t> *handler;
public:
	abstract_dialog_t() {
		layout = &center_layout;
		preferred_layout = &preferred_stack_layout;
		handler = 0;
	}


	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}

	virtual void set_preferred_size() {
		gobject_t::set_preferred_size();
		w += 10;
		h += 10;
	}

	virtual void render(surface_t &dst) OVERRIDE {
		s32 ax, ay;
		translate(ax,ay);
		dst.ctx.reset();
		dst.ctx.pen_color = 0xffffff;
		dst.ctx.alfa = 60;
		dst.fill(ax,ay,w,h); 
	}

	virtual void key_event(key_t::key_t key) OVERRIDE {
		if (key == key_t::K_ESC) {
			close(false);
			return;
		}
		super::key_event(key);
	}

	virtual void close(bool isOK) {
		globals::modal_overlay.pop_modal(this);
		if (handler)
			handler->dialog_closed(this);
	}

};

class dialog_t : public abstract_dialog_t {
public:
	label_t lab1;
	label_t lab2;
	label_t lab3;
private:
	dialog_t() {
	}
public:
	static dialog_t instance;

	virtual void init() {
		gobject_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		lab1.ctx = ctx.lctx1;
		lab1.ctx.font_size = font_size_t::FS_20;
		lab2.ctx = ctx.lctx1;
		lab2.ctx.font_size = font_size_t::FS_20;
		lab3.ctx = ctx.lctx1;
		lab3.ctx.font_size = font_size_t::FS_20;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lab1;
		else if (prev == &lab1) return &lab2;
		else if (prev == &lab2) return &lab3;
		else return 0;
	}

};


class inp_dlg_row_t : public gobject_t {
public:
	text_box_t lval;
	combo_box_t lsfx;
	stack_layout_t stack_layout;
	preferred_stack_layout_t preferred_stack_layout;
public:
	inp_dlg_row_t() {
		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;
		preferred_stack_layout.vertical = false;
		layout = &stack_layout;
		preferred_layout = &preferred_stack_layout;
		this->can_be_selected = true;
	}

	virtual void init() {
		gobject_t::init();

		menu_context_t &ctx = menu_context_t::instance;

		lval.visible = true;
		lval.lab.ctx = ctx.lctx1;
		lval.lab.ctx.font_size = font_size_t::FS_20;

		lsfx.visible = true;
		lsfx.lab.ctx = ctx.lctx1;
		lsfx.lab.ctx.font_size = font_size_t::FS_20;
		_MY_ASSERT(parent,return);
		parent->capture_key_events(this);
//		this->focus_manager.select(&lval);
//		lval.key_event(key_t::K_ENTER);
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}


	virtual void child_size_changed(gobject_t *child) OVERRIDE {
		set_preferred_size();
		do_layout();
		if (parent)
			parent->child_size_changed(this);
		dirty = true;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lval;
//		else if (prev == &lval) return &lsfx;
		return 0;
	}
};



class validator_t {
public:
	string_t msg;
	property_t<s32, validator_t> multiplier;
protected:
	s32 _multiplier;
protected:
	s32 get_multiplier() {
		return _multiplier;
	}
	void set_multiplier(s32 v) {
		_multiplier = v;
		update_msg();
	}
	virtual bool update_msg() = 0;
public:
	validator_t() {
		_multiplier = 1;
		multiplier.init(this,&validator_t::get_multiplier, &validator_t::set_multiplier);
	}
	virtual bool validate(string_t value) = 0;
};


template<typename T>
class range_validator_t : public validator_t {
//	typedef validator_t super;
public:
	property_t<T, range_validator_t> lo;
	property_t<T, range_validator_t> hi;
private:
	T _lo;
	T _hi;
	string_impl_t<INPUT_MAX_LEN> range_lo_s;
	string_impl_t<INPUT_MAX_LEN> range_hi_s;
private:
	T get_lo() {
		return _lo;
	}
	T get_hi() {
		return _hi;
	}
	void set_lo(T v) {
		_lo = v;
		update_msg();
	}
	void set_hi(T v) {
		_hi = v;
		update_msg();
	}
	bool update_msg() OVERRIDE {
		if (!conv_utils::ftoa<T>(_lo,range_lo_s,_multiplier))
			return false;
		if (!conv_utils::ftoa<T>(_hi,range_hi_s,_multiplier))
			return false;
		range_lo_s.append(string_t("-"));
		range_lo_s.append(range_hi_s);
		msg = range_lo_s;
		return true;
	}
public:
	range_validator_t() {
		_lo = _hi = 0;
		multiplier = 1;
		lo.init(this,&range_validator_t::get_lo, &range_validator_t::set_lo);
		hi.init(this,&range_validator_t::get_hi, &range_validator_t::set_hi);
	}

	void init(T alo, T ahi, T amultiplier) {
		//super::init(pattern,_msg);
		_multiplier = amultiplier;
		msg = range_lo_s;

		_MY_ASSERT(alo < ahi,return);
		_lo = alo;
		_hi = ahi;

		update_msg();
	}

	virtual bool validate(string_t value) OVERRIDE {
		bool ret = true; // super::validate(value);
		T vali = 0;
		if (ret)
			ret = conv_utils::atof<T>(value,vali, multiplier);
		if (ret)
			ret = lo <= vali && vali <= hi;
		return ret;
	}
};


class input_dialog_context_t {
public:
	string_impl_t<INPUT_MAX_LEN> value;
//	string_impl_t<INPUT_MAX_LEN> suffix;
	suffix_t suffix;
	bool data_valid;
	validator_t *validator;
	iterator_t<suffix_t>* suffixes;
public:
	input_dialog_context_t() {
		suffixes = 0;
		validator = 0;
		data_valid = true;
	}
};

// menu set interface to interact with input_dialog_t
class input_dialog_t;

class menu_set_i {
public:
	static menu_set_i *instance;
public:
	menu_set_i() {
		instance = this;
	}
	virtual void set_sfx_menu(input_dialog_t *dlg, input_dialog_context_t &ctx) = 0;
	virtual void ret_sfx_menu() = 0;
	virtual void key_event_fwd(key_t::key_t key) = 0;
};





class input_dialog_t : public abstract_dialog_t {
	typedef abstract_dialog_t super;

public:
	property_t<input_dialog_context_t , input_dialog_t> ctx;
	text_box_t lval;
private:
	input_dialog_context_t _ctx;
private:

	input_dialog_context_t get_ctx() {
		_ctx.value = lval.value;
		return _ctx;
	}

	void set_ctx(input_dialog_context_t actx) {
		_ctx = actx;
//		lval.value = _ctx.value;
		lval.value = "";

		if (_ctx.suffix.is_empty() && _ctx.suffixes) {
			suffix_t *psfx = _ctx.suffixes->next(0);
			_MY_ASSERT(psfx,return);
			_ctx.suffix = *psfx;
		}

		menu_set_i::instance->set_sfx_menu(this, _ctx);
	}

	input_dialog_t() {
		ctx.init(this,&input_dialog_t::get_ctx, &input_dialog_t::set_ctx);
	}

public:
	static input_dialog_t instance;

	virtual void init() {
		super::init();

		menu_context_t &ctx = menu_context_t::instance;
		lval.visible = true;
		lval.lab.ctx = ctx.lctx1;
		lval.lab.ctx.font_size = font_size_t::FS_20;
		lval.can_be_selected = false;
		lval.cursor_visible = true;
		lval.cursor_color = 0xffffff;

		capture_key_events(&lval);
		this->set_preferred_size();
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}

	virtual void render(surface_t &dst) OVERRIDE {
		s32 ax, ay;
		translate(ax,ay);
		dst.ctx.reset();
		dst.ctx.pen_color = 0x000000;
		dst.ctx.alfa = 60;
		dst.fill(ax,ay,w,h); 
	}

	virtual void child_size_changed(gobject_t *child) OVERRIDE {
		// input box value changed
		set_preferred_size();
		do_layout();
		dirty = true;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lval;
		else return 0;
	}

	virtual void key_event(key_t::key_t key) OVERRIDE {
		// redirect menu keys to sfx menu
		if (key == key_t::K_F1 || key == key_t::K_F2 || 
			key == key_t::K_F3 || key == key_t::K_F4 || key == key_t::K_F5) {
			menu_set_i::instance->key_event_fwd(key);
//HACK: to redraw dialog on top of screen
			globals::modal_overlay.dirty = true;
			return;
		}
		// filter ENTER key out of a textbox
		if (key == key_t::K_ENTER)
			return;

		super::key_event(key);
	}

	virtual void close(bool isOK) OVERRIDE {
		_ctx.data_valid = isOK;
		menu_set_i::instance->ret_sfx_menu();
		super::close(isOK);
	}

	// called after sfx btn click
	void set_suffix(suffix_t sfx) {
		_ctx.suffix = sfx;
	}
};








namespace globals {
	namespace dialogs {
//		extern dialog_t dialog;
//		extern input_dialog_t input_dialog;

		extern void init();

		extern void show(gobject_t *dlg);
	}
}


class menu_item_t :public gobject_t {
	typedef gobject_t super;
public:
public:
	menu_item_t() {
		can_be_selected = true;
	}
	virtual void next_state() {
	}

	virtual void set_selected(bool selected) OVERRIDE {
		if (selected) {
			next_state();
			dirty = true;
		}
		super::set_selected(selected);
	}

	virtual void key_event(key_t::key_t key) OVERRIDE {
		if (key == key_t::K_ENTER) {
			next_state();
			dirty = true;
			return;
		}
		gobject_t::key_event(key);
	}

};


class btn_menu_item_t : public menu_item_t {
	typedef menu_item_t super;
public:
	button_t btn;
	stretch_layout_t stretch_layout;
public:
	btn_menu_item_t() {
		layout = &stretch_layout;
	}

	virtual void set_selected(bool selected) OVERRIDE {
		btn.selected = (selected);
		super::set_selected(selected);
	}

	virtual void set_enabled(bool enabled) OVERRIDE {
		super::set_enabled(enabled);
		btn.enabled = enabled;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &btn;
		else return 0;
	}
};





class menu_t : public gobject_t {
	typedef gobject_t super;
public:
	string_t name;
public:
	virtual menu_item_t* next_item(void* prev) = 0;

	virtual gobject_t* next_all(void* prev) SEALED OVERRIDE {
		return next_item(prev);
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		super::set_dirty(dirty);
		_MY_ASSERT(parent, return);
		parent->set_dirty(dirty);
	}

	virtual void key_event(key_t::key_t key) OVERRIDE {
		s32 num = -1;
		switch (key) {
		case key_t::K_F1 :
			num = 0;
			break;
		case key_t::K_F2 :
			num = 1;
			break;
		case key_t::K_F3 :
			num = 2;
			break;
		case key_t::K_F4 :
			num = 3;
			break;
		case key_t::K_F5 :
			num = 4;
			break;
		}
		if (num >= 0) {
			menu_item_t * mi = next_item(0);
			while (mi) {
				if (num <= 0 && mi->visible) {
					this->focus_manager.select(mi);
					break;
				}
				num--;
				mi = next_item(mi);
			}
			return;
		}
		gobject_t::key_event(key);
	}

};


class double_label_t : public gobject_t {
public:
	label_t l_top;
	label_t l_bot;
	center_layout_t center_layout;
	preferred_stack_layout_t preferred_stack_layout;
public:
	double_label_t() {
		layout = &center_layout;
		preferred_layout = &preferred_stack_layout;
		center_layout.spy = 2;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &l_top;
		else if (prev == &l_top) return &l_bot;
		else return 0;
	}

};


class dl_menu_item_t : public menu_item_t {
public:
	double_label_t dl;
	stretch_layout_t stretch_layout;
	preferred_stack_layout_t prl;
public:
	dl_menu_item_t() {
		layout = &stretch_layout;
		preferred_layout = &prl;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &dl;
		else return 0;
	}

};


// --------------------- application specific ----------------------------

class menu_item_izm_t : public btn_menu_item_t, public subscriber_t<model_changed_msg_t> {
public:
	u8 state;
public:
	menu_item_izm_t() {
		state = 0;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();

		app_model_t::instance.subscribe(this);

		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("»«Ã≈–≈Õ»≈");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctxg;

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;

		app_model_t::instance.start_stop_changed(state);
		set_state(false);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::DEV_STAT) {
			state = (u8) msg.model->dev_status;
			set_state(true);
		}
	}

	virtual void next_state() OVERRIDE {
		next_state(true);
	}

	void next_state(bool _update) {

		state++;
		state &= 0x01;

		app_model_t::instance.start_stop_changed(state);
		set_state(_update);
	}

	void set_state(bool _update) {
		if (!state) { // stopped
			btn.l_mid.glyph_code = 0x0028;
			btn.l_bot.text = string_t("—“¿–“");
		} else { // started
			btn.l_mid.glyph_code = 0x0029;
			btn.l_bot.text = string_t("—“Œœ");
		}

		if (_update) {
			do_layout();
			dirty = true;
		}
	}
};

#define MAX_MENU_VAL_LEN INPUT_MAX_LEN
#define MAX_MENU_SFX_LEN 5

class menu_editable_item_t : public btn_menu_item_t {
	typedef btn_menu_item_t super;
	class hdl_t :  public dialog_handler_t<abstract_dialog_t> {
	public:
		menu_editable_item_t *menu;
	public:
		virtual void dialog_closed(abstract_dialog_t *dlg) OVERRIDE {
			_MY_ASSERT(menu,return);
			menu->set_ctx(input_dialog_t::instance.ctx);
			menu->do_layout();
			menu->dirty = true;
		}

	};
protected:
	input_dialog_context_t dialog_ctx;
	label_t *active_label; // ÏÂÚÍ‡, ‚ ÍÓÚÓÛ˛ ·Û‰ÛÚ ‚˚‚Ó‰ËÚ¸Òˇ ËÁÏÂÌˇÂÏ˚Â ‰‡ÌÌ˚Â
protected:
	string_impl_t<MAX_MENU_VAL_LEN + MAX_MENU_SFX_LEN> val_sfx;
	hdl_t hdl;
protected:

	void set_ctx(input_dialog_context_t ctx, bool allow_update = true) {

		if (!ctx.data_valid) 
			return;
		dialog_ctx = ctx;
		val_sfx = (ctx.value);
		val_sfx += (ctx.suffix.name);
		active_label->text = val_sfx;
		if (allow_update)
			data_changed();
	}

	virtual void data_changed() = 0;

public:
	menu_editable_item_t() {
		active_label = &btn.l_mid;
	}

	virtual void next_state() OVERRIDE {
		hdl.menu = this;
		input_dialog_t::instance.handler = &hdl;
		input_dialog_t::instance.ctx = dialog_ctx;

		input_dialog_t::instance.set_preferred_size();
		input_dialog_t::instance.do_layout();

		globals::dialogs::show(&input_dialog_t::instance);
	}


};

class menu_item_dme_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
	typedef menu_editable_item_t super;
// ÒÔËÒÓÍ ÒÛÙÙËÍÒÓ‚
public:
	class suffixes_t : public iterator_t<suffix_t> {
	public:
		suffix_t x;
		suffix_t y;
	private:
		suffixes_t(): x("X",1), y("Y",1) {
		}
	public:
		static suffixes_t instance;

		virtual suffix_t* next(void* prev) OVERRIDE {
			if (!prev) return &x;
			if (prev == &x) return &y;
			return 0;
		}

		msg::tSuffix::tSuffix get_val(suffix_t v) {
			//_MY_ASSERT(v == &x || v == &y, return msg::tSuffix::X);
			if (v == x)
				return msg::tSuffix::X;
			if (v == y)
				return msg::tSuffix::Y;
			return msg::tSuffix::X;
		}

		string_t & get_val(msg::tSuffix::tSuffix v) {
			if (v == msg::tSuffix::X)
				return x.name;
			if (v == msg::tSuffix::Y)
				return y.name;
			_MY_ASSERT(0,return x.name);
			return x.name;
		}
		suffix_t & get_val2(msg::tSuffix::tSuffix v) {
			if (v == msg::tSuffix::X)
				return x;
			if (v == msg::tSuffix::Y)
				return y;
			_MY_ASSERT(0,return x);
			return x;
		}
	};


public:
//	static suffixes_t suffixes;
	range_validator_t<u32> validator;
public:

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t(" ¿Õ¿À DME");
 
		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_20;

		dialog_ctx.value = string_t("50");
		dialog_ctx.suffix = suffixes_t::instance.x;
		dialog_ctx.suffixes = &suffixes_t::instance;

		validator.init(1,126,1);
		dialog_ctx.validator = &validator;

		set_ctx(dialog_ctx,false);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::DME) {
			if (msg.model->dme.enabled) {
				if (!conv_utils::ftoa<s32>(app_model_t::instance.dme.ch,dialog_ctx.value,1))
					return;
				dialog_ctx.suffix = suffixes_t::instance.get_val2(app_model_t::instance.dme.suffix);
				validator.init(
					app_model_t::instance.dme.range.lo,
					app_model_t::instance.dme.range.hi,
					1);
				set_ctx(dialog_ctx, false);
			} else {
				active_label->text = string_t("USER");
			}
			this->dirty = true;
			this->do_layout();
		}
	}

	virtual void data_changed() OVERRIDE {
		u32 num = 0;
		if (!conv_utils::atof<u32>(dialog_ctx.value,num,1))
			return;
		msg::tSuffix::tSuffix sfx = suffixes_t::instance.get_val(dialog_ctx.suffix);
		app_model_t::instance.dme_ch_changed((u8)num,sfx);
	}

};


class menu_item_ant_t : public btn_menu_item_t, public subscriber_t<model_changed_msg_t> {
public:
	msg::OutputMode::OutputMode state;
public:
	menu_item_ant_t() {
		state = msg::OutputMode::omAntenna;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;
		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("¬€’Œƒ");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctxg;
		btn.l_mid.ctx.font_size = font_size_t::FS_30;

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;

		app_model_t::instance.output_mode_changed(state);
		set_state(false);
	}

	virtual void next_state() OVERRIDE {
		next_state(true);
	}

	void next_state(bool _update) {

		switch (state) {
		case msg::OutputMode::omAntenna:
			state = msg::OutputMode::omBeacon;
			break;

		case msg::OutputMode::omBeacon:
			state = msg::OutputMode::omCalibration;
			break;

		case msg::OutputMode::omCalibration:
			state = msg::OutputMode::omAntenna;
			break;
		}

		app_model_t::instance.output_mode_changed(state);
		set_state(_update);
	}

	void set_state(bool _update) {
		switch (state) {
		case msg::OutputMode::omAntenna:
			btn.l_mid.glyph_code = 0x002a;
			btn.l_bot.text = string_t("¿Õ“≈ÕÕ¿");
			break;

		case msg::OutputMode::omBeacon:
			btn.l_mid.glyph_code = 0x002f;
			btn.l_bot.text = string_t("¿““≈Õ.");
			break;

		case msg::OutputMode::omCalibration:
			btn.l_mid.glyph_code = 0x0030;
			btn.l_bot.text = string_t(" ¿À»¡–.");
			break;
		}
		if (_update) {
			do_layout();
			dirty = true;
		}
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::OUTPUT_MODE) {
			state = app_model_t::instance.out_mode;
			set_state(true);
		}
	}

};

class menu_item_lvl_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
public:
	range_validator_t<s32> validator;
public:

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;
		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("”–Œ¬≈Õ‹");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_20;
//		btn.l_mid.text = string_t("+10");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
		btn.l_bot.text = string_t("ƒ¡Ã");

		dialog_ctx.value = (string_t("+10"));

		validator.init(-40,40,1); 
		dialog_ctx.validator = &validator;

		set_ctx(dialog_ctx,false);
	}

	virtual void data_changed() OVERRIDE {
		s32 num = -1;
		if (!conv_utils::atof<s32>(dialog_ctx.value,num,1))
			return;

		if (num >=0 && val_sfx.length() > 0 && val_sfx[0] != '+') {
			val_sfx.insert_at(0,'+');
			active_label->text = val_sfx;
		}

		validator.lo = app_model_t::instance.req_lev_range.lo;
		validator.hi = app_model_t::instance.req_lev_range.hi;

		app_model_t::instance.req_level_changed(num);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		if (msg.what == model_changed_what_t::REQ_LEVEL) {

			if (!conv_utils::ftoa<s32>(app_model_t::instance.req_lev, dialog_ctx.value,1))
				return;

			validator.lo = app_model_t::instance.req_lev_range.lo;
			validator.hi = app_model_t::instance.req_lev_range.hi;

			set_ctx(dialog_ctx, false);

			this->dirty = true;
			this->do_layout();
		}
	}


};

static u32 hip_str[] = {0x002b,0x002b,0x002b,0x002b};

class menu_item_hip_t : public btn_menu_item_t, public subscriber_t<model_changed_msg_t> {
public:
	msg::tRequestForm::tRequestForm state;
public:
	menu_item_hip_t() {
		state = msg::tRequestForm::fRandom;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;
		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("«¿œ”— ");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_15;

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctxg;
		btn.l_bot.ctx.font_size = font_size_t::FS_8;
		btn.l_bot.text32 = string32_t(hip_str,sizeof(hip_str)/sizeof(u32));

		app_model_t::instance.req_form_changed(state);
		set_state(false);
	}

	virtual void next_state() OVERRIDE {
		next_state(true);
	}

	void next_state(bool _update)  {

		switch(state) {
		case msg::tRequestForm::fRandom : state = msg::tRequestForm::fDetermine;
			break;
		case msg::tRequestForm::fDetermine : state = msg::tRequestForm::fExternal;
			break;
		case msg::tRequestForm::fExternal : state = msg::tRequestForm::fReqOff;
			break;
		case msg::tRequestForm::fReqOff : state = msg::tRequestForm::fRandom;
			break;
		}


		app_model_t::instance.req_form_changed(state);
		set_state(_update);
	}

	void set_state(bool _update)  {

		if (state == msg::tRequestForm::fRandom) {
			btn.l_mid.text = string_t("’»œ");
			btn.l_bot.ctx.font_size = font_size_t::FS_8;
			btn.l_bot.text32 = string32_t(hip_str,sizeof(hip_str)/sizeof(u32));

		} else if (state == msg::tRequestForm::fDetermine) {
			btn.l_mid.text = string_t("œ¿◊ ¿");
			btn.l_bot.ctx.font_size = font_size_t::FS_8;
			btn.l_bot.text32 = string32_t(hip_str,sizeof(hip_str)/sizeof(u32));

		} else if (state == msg::tRequestForm::fExternal) {
			btn.l_mid.text = string_t("¬Õ≈ÿ");
			btn.l_bot.ctx.font_size = font_size_t::FS_15;
			btn.l_bot.text32 = string32_t();
			btn.l_bot.glyph_code = 0x0031;

		} else if (state == msg::tRequestForm::fReqOff) {
			btn.l_mid.text = string_t("¬€ À");
			btn.l_bot.ctx.font_size = font_size_t::FS_15;
			btn.l_bot.text32 = string32_t();
			btn.l_bot.glyph_code = 0;
		}


		if (_update) {
			do_layout();
			dirty = true;
		}
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::REQ_FORM) {
			state = app_model_t::instance.req_form;
			set_state(true);
		}
	}

};


class main_menu_t : public menu_t {
public:
	menu_item_izm_t mi_izm;
	menu_item_dme_t mi_dme;
	menu_item_ant_t mi_ant;
	menu_item_lvl_t mi_lvl;
	menu_item_hip_t mi_hip;
public:
	main_menu_t() {
		name = string_t("√À¿¬ÕŒ≈");
	}

	virtual menu_item_t* next_item(void* prev) OVERRIDE {
		if (!prev) return &mi_izm;
		else if (prev == &mi_izm) return &mi_dme;
		else if (prev == &mi_dme) return &mi_ant;
		else if (prev == &mi_ant) return &mi_lvl;
		else if (prev == &mi_lvl) return &mi_hip;
		else return 0;
	}
};


// ---------------- PARAM MENU -----------------------------------------------------
class menu_item_zapr_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
public:
	range_validator_t<u32> validator;
public:

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("«¿œ–Œ—€");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_20;
//		btn.l_mid.text = string_t("1300");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
		btn.l_bot.text = string_t("»Ãœ. œ¿–/C");

		dialog_ctx.value = (string_t("1300"));

		validator.init(0,5000,1);
		dialog_ctx.validator = &validator;

		set_ctx(dialog_ctx,false);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		if (msg.what == model_changed_what_t::REQ_FORM || 
			msg.what == model_changed_what_t::REQ_FREQ_ZAPR) {


			validator.lo = app_model_t::instance.req_zapr_range.lo;
			validator.hi = app_model_t::instance.req_zapr_range.hi;

			if (msg.model->req_form == msg::tRequestForm::fExternal) {
				active_label->text = string_t("EXT");
				enabled = false;

			} else if (msg.model->req_form == msg::tRequestForm::fReqOff) {
				active_label->text = string_t("OFF");
				enabled = false;

			} else {
				if (!conv_utils::ftoa<u32>(app_model_t::instance.req_zapr,dialog_ctx.value,1))
					return;
				enabled = true;
				set_ctx(dialog_ctx, false);
			}

			this->do_layout();
			this->dirty = true;

		}
	}

	virtual void data_changed() OVERRIDE {
		u32 num = 0;
		if (!conv_utils::atof<u32>(dialog_ctx.value,num,1))
			return;
		app_model_t::instance.req_freq_zapr_changed(num);
	}

};


class menu_item_otv_t : public menu_item_zapr_t {
	typedef menu_item_zapr_t super;
public:
public:
	virtual void init() OVERRIDE {
		super::init();
		btn.l_top.text = string_t("Œ“¬≈“€");
	}


	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		if (msg.what == model_changed_what_t::REQ_FORM || 
			msg.what == model_changed_what_t::REQ_FREQ_OTV) {

			validator.lo = app_model_t::instance.req_otv_range.lo;
			validator.hi = app_model_t::instance.req_otv_range.hi;

			if (msg.model->req_form == msg::tRequestForm::fExternal) {
				active_label->text = string_t("EXT");
				enabled = false;
			} else if (msg.model->req_form == msg::tRequestForm::fReqOff) {
				active_label->text = string_t("OFF");
				enabled = false;
			} else {
				if (!conv_utils::ftoa<u32>(app_model_t::instance.req_otv,dialog_ctx.value,1))
					return;
				enabled = true;
				set_ctx(dialog_ctx, false);
			}

			this->dirty = true;
			this->do_layout();
		}
	}

	virtual void data_changed() OVERRIDE {
		u32 num = 0;
		if (!conv_utils::atof<u32>(dialog_ctx.value,num,1))
			return;
		app_model_t::instance.req_freq_otv_changed(num);
	}


};

class menu_item_prd_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
public:
	range_validator_t<u32> validator1;
//	range_validator_t validator2;
public:

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("F œ–ƒ");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_15;
//		btn.l_mid.text = string_t("1050,20");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
		btn.l_bot.text = string_t("Ã√÷");

		validator1.init(1024*_1M,1151*_1M,_1M); 
//		validator2.init(960*_1M,1215*_1M,_1M,"\\d{3,4}(.\\d{1,3})?", "960-1215"); 
		dialog_ctx.validator = &validator1;

		dialog_ctx.value = (string_t("1050.20"));
		set_ctx(dialog_ctx,false);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::REQ_FREQ) {
			if (!app_model_t::instance.req_f.ftoa(dialog_ctx.value,units::MHZ))
				return;

//			validator1.msg = range_lo_s;
			validator1.lo = app_model_t::instance.req_f_range.lo.value;
			validator1.hi = app_model_t::instance.req_f_range.hi.value;

			set_ctx(dialog_ctx, false);
			this->dirty = true;
			this->do_layout();
		}
	}

	virtual void data_changed() OVERRIDE {
		app_model_t::freq_t num;
		if (!num.atof(dialog_ctx.value,units::MHZ))
			return;
		app_model_t::instance.req_freq_changed(num);
	}

};

class menu_item_req_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
public:
	range_validator_t<u32> validator;
	static u32 dl_12mks1_str[];
public:

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("«¿œ–Œ—");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_8;
//		btn.l_mid.text = string_t("12,0 Ã —");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctxg;
		btn.l_bot.ctx.font_size = font_size_t::FS_8;

		btn.l_bot.text32 = string32_t(dl_12mks1_str);

		dialog_ctx.value = (string_t("12.0"));
		dialog_ctx.suffix = suffix_t("Ã —",units::MKS);

		validator.init(9u *units::MKS,40u *units::MKS, units::MKS);
		dialog_ctx.validator = &validator;

		set_ctx(dialog_ctx,false);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::REQ_SPAN) {

			if (!app_model_t::instance.req_span.ftoa(dialog_ctx.value,units::MKS))
				return;
			validator.lo = app_model_t::instance.req_span_range.lo.value;
			validator.hi = app_model_t::instance.req_span_range.hi.value;
			set_ctx(dialog_ctx, false);

			this->dirty = true;
			this->do_layout();
		}
	}


	virtual void data_changed() OVERRIDE {
		app_model_t::time_t num;
		if (!num.atof(dialog_ctx.value,units::MKS))
			return;
		app_model_t::instance.req_span_changed(num);
	}


};

class menu_item_rsp_t : public menu_item_req_t {
public:
public:

	virtual void init() OVERRIDE {
		menu_item_req_t::init();
		btn.l_top.text = string_t("Œ“¬≈“");
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::RSP_SPAN) {

			if (!app_model_t::instance.rsp_span.ftoa(dialog_ctx.value,units::MKS))
				return;
			validator.lo = app_model_t::instance.rsp_span_range.lo.value;
			validator.hi = app_model_t::instance.rsp_span_range.hi.value;
			set_ctx(dialog_ctx, false);

			this->dirty = true;
			this->do_layout();
		}
	}

	virtual void data_changed() OVERRIDE {
		app_model_t::time_t num;
		if (!num.atof(dialog_ctx.value,units::MKS))
			return;
		app_model_t::instance.rsp_span_changed(num);
	}

};


class menu_item_mode_t : public btn_menu_item_t, public subscriber_t<model_changed_msg_t> {
public:
	msg::UserMode::UserMode state;
public:
	menu_item_mode_t() {
		state = msg::UserMode::umInterrogator;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;
		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("–≈∆»Ã");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;

		app_model_t::instance.user_mode_changed(state);
		set_state(false);
	}
	virtual void next_state() OVERRIDE {
		next_state(true);
	}

	void next_state(bool _update) {

		switch (state) {
		case msg::UserMode::umTransmitter :
			state = msg::UserMode::umInterrogator;
			break;
		case msg::UserMode::umInterrogator :
			state = msg::UserMode::umTransmitter;
			break;
		}
		app_model_t::instance.user_mode_changed(state);
		set_state(_update);
	}

	void set_state(bool _update) {

		switch (state) {
		case msg::UserMode::umInterrogator :
			btn.l_mid.text = string_t("«¿œ–Œ—◊» ");
			break;
		case msg::UserMode::umTransmitter :
			btn.l_mid.text = string_t("Œ“¬≈“◊» ");
			break;
		}

		if (_update) {
			do_layout();
			dirty = true;
		}
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::USER_MODE) {
			state = app_model_t::instance.user_mode;
			set_state(true);
		}
	}

};

class param_menu_t : public menu_t {
public:
	menu_item_zapr_t mi1;
	menu_item_prd_t mi2;
	menu_item_req_t mi3;
	menu_item_rsp_t mi4;
	menu_item_mode_t mi5;
public:
	param_menu_t() {
		name = string_t("œ¿–¿Ã≈“–€");
	}

	virtual menu_item_t* next_item(void* prev) OVERRIDE {
		if (!prev) return &mi1;
		else if (prev == &mi1) return &mi2;
		else if (prev == &mi2) return &mi3;
		else if (prev == &mi3) return &mi4;
		else if (prev == &mi4) return &mi5;
		else return 0;
	}
};

// ------------------------------ ÏÂÌ˛ √–¿‘»  ----------------------------

class menu_item_scanx_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {

private:
	units::time2_suffixes_t suffixes;
	range_validator_t<u32> validator1;
public:
	menu_item_scanx_t() {
		active_label = &btn.l_bot;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;
		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("–¿«¬≈–“ ¿");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_15;
		btn.l_mid.text = string_t("X");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
//		btn.l_bot.text = string_t("5 Ã —");

		dialog_ctx.value = (string_t("5"));
		dialog_ctx.suffix = * suffixes.next(0);
		dialog_ctx.suffixes = &suffixes;

		validator1.init(1 * units::MS,30 * units::MIN, units::MS);
		dialog_ctx.validator = &validator1;

		set_ctx(dialog_ctx,false);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::RESOLUTION_X) {
			if (!app_model_t::instance.osc.dpx.ftoa(dialog_ctx.value,(units::time2_t) dialog_ctx.suffix.multiplier))
				return;
			set_ctx(dialog_ctx, false);
			this->dirty = true;
			this->do_layout();
		}
	}

	virtual void data_changed() OVERRIDE {
		app_model_t::time2_t dpx;
		if (!dpx.atof(dialog_ctx.value, (units::time2_t) dialog_ctx.suffix.multiplier))
			return;
		app_model_t::instance.resolution_x_changed(dpx,(units::time2_t) dialog_ctx.suffix.multiplier);
	} 



};

class menu_item_scany_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
public:
	range_validator_t<u32> validator1;
public:
	menu_item_scany_t() {
		active_label = &btn.l_bot;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("–¿«¬≈–“ ¿");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_15;
		btn.l_mid.text = string_t("Y");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
		btn.l_bot.text = string_t("100 %");


		dialog_ctx.value = (string_t("100"));
		dialog_ctx.suffix = suffix_t("%",1);

		validator1.init(1,100, 1);
		dialog_ctx.validator = &validator1;

		set_ctx(dialog_ctx,false);

	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::RESOLUTION_Y) {
			if (!app_model_t::instance.osc.dpy.ftoa(dialog_ctx.value,units::INT))
				return;
			set_ctx(dialog_ctx, false);
			this->dirty = true;
			this->do_layout();
		}
	}

	virtual void data_changed() OVERRIDE {
		app_model_t::real_t dpy;
		if (!dpy.atof(dialog_ctx.value, units::INT))
			return;
		app_model_t::instance.resolution_y_changed(dpy);
	} 

};


class menu_item_curonoff_t : public btn_menu_item_t, public subscriber_t<model_changed_msg_t> {
public:
	u8 state;
public:
	menu_item_curonoff_t() {
		state = 1;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t(" ”–—Œ–");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctxg;
		btn.l_bot.ctx.font_size = font_size_t::FS_20;
		next_state(false);
	}
	virtual void next_state() OVERRIDE {
		next_state(true);
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::CURSOR) {
			this->dirty = true;
			if ((state != 0) != app_model_t::instance.osc.cursor.enabled) {
				next_state(false);
			}
		}
	}


	void next_state(bool _update) {
		if (!state) {
			btn.l_mid.text = string_t("¬ À");
			btn.l_bot.glyph_code = 0x0027;
		} else if (state == 1) {
			btn.l_mid.text = string_t("Œ“ À");
			btn.l_bot.glyph_code = 0x0026;
		}
		state++;
		if (state > 1) state = 0;
//		send_state_changed(state);
		if (_update) {
			app_model_t::instance.cursor_enabled_changed(state);
			do_layout();
			dirty = true;
		}
	}
};





class menu_item_curx_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
	typedef menu_editable_item_t super;
public:
	units::time2_suffixes_t suffixes;
public:
	menu_item_curx_t() {
		this->active_label = &btn.l_bot;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t(" ”–—Œ– X");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.text = string_t("1502 M");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
		btn.l_bot.text = string_t("11 Ã—");


		dialog_ctx.value = (string_t("11.30"));
		dialog_ctx.suffixes = &suffixes;
		dialog_ctx.suffix = *suffixes.next(0);

		//validator1.init(1,100, 1);
		//dialog_ctx.validator = &validator1;

		set_ctx(dialog_ctx,false);

	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		if (msg.what == model_changed_what_t::CURSOR) {
			if (!app_model_t::instance.osc.cursor.x.ftoa(
					dialog_ctx.value,
					(units::time2_t) dialog_ctx.suffix.multiplier)
					)
				return;
			set_ctx(dialog_ctx, false);
			this->dirty = true;
			this->do_layout();
		}
	}

	//virtual void key_event(key_t::key_t key) OVERRIDE {
	//	app_model_t::time2_t dpx = app_model_t::instance.osc.cursor.x;
	//	u32 sfx = app_model_t::instance.osc.cursor.x_sfx;
	//	if (sfx > 1) sfx /= 10;

	//	if (key == key_t::K_LEFT) {
	//		if (dpx.value >= sfx)
	//			dpx.value -= sfx;
	//		app_model_t::instance.cursor_x_changed(dpx,  app_model_t::instance.osc.cursor.x_sfx);
	//		dirty = true;

	//	} else if (key == key_t::K_RIGHT) {
	//		dpx.value += sfx;
	//		app_model_t::instance.cursor_x_changed(dpx, app_model_t::instance.osc.cursor.x_sfx);
	//		dirty = true;
	//	}
	//}

	virtual void data_changed() OVERRIDE {
		app_model_t::time2_t dpx;
		if (!dpx.atof(dialog_ctx.value, (units::time2_t) dialog_ctx.suffix.multiplier))
			return;
		app_model_t::instance.cursor_x_changed(dpx, (units::time2_t) dialog_ctx.suffix.multiplier);
	} 

};



class menu_item_offsx_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
	typedef menu_editable_item_t super;
public:
	units::time2_suffixes_t suffixes;
public:
	menu_item_offsx_t() {
		this->active_label = &btn.l_bot;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("—Ã≈Ÿ. X");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.text = string_t("150 M");

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
		btn.l_bot.text = string_t("10 Ã—");


		dialog_ctx.value = (string_t("10.30"));
		dialog_ctx.suffixes = &suffixes;
		dialog_ctx.suffix = *suffixes.next(0);

		//validator1.init(1,100, 1);
		//dialog_ctx.validator = &validator1;

		set_ctx(dialog_ctx,false);

	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {

		if (msg.what == model_changed_what_t::OFFSETX) {
			if (!app_model_t::instance.osc.offset.x.ftoa(
					dialog_ctx.value,
					(units::time2_t) dialog_ctx.suffix.multiplier)
					)
				return;
			set_ctx(dialog_ctx, false);
			this->dirty = true;
			this->do_layout();
		}
	}

	//virtual void key_event(key_t::key_t key) OVERRIDE {
	//	app_model_t::time2_t dpx = app_model_t::instance.osc.offset.x;
	//	u32 sfx = app_model_t::instance.osc.offset.x_sfx;
	//	if (sfx > 1) sfx /= 10;

	//	if (key == key_t::K_LEFT) {
	//		if (dpx.value > sfx)
	//			dpx.value -= sfx;
	//		app_model_t::instance.offset_x_changed(dpx,  app_model_t::instance.osc.offset.x_sfx);
	//		this->dirty = true;
	//		this->do_layout();

	//	} else if (key == key_t::K_RIGHT) {
	//		dpx.value += sfx;
	//		app_model_t::instance.offset_x_changed(dpx, app_model_t::instance.osc.offset.x_sfx);
	//		this->dirty = true;
	//		this->do_layout();
	//	}
	//}

	virtual void data_changed() OVERRIDE {
		app_model_t::time2_t dpx;
		if (!dpx.atof(dialog_ctx.value, (units::time2_t) dialog_ctx.suffix.multiplier))
			return;
		app_model_t::instance.offset_x_changed(dpx, (units::time2_t) dialog_ctx.suffix.multiplier);
	} 

};

class menu_item_cury_t : public menu_editable_item_t, public subscriber_t<model_changed_msg_t> {
public:
public:
	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		app_model_t::instance.subscribe(this);

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t(" ”–—Œ– Y");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.text = string_t("99 %");

		dialog_ctx.value = (string_t("99"));
		dialog_ctx.suffix = suffix_t("%",1);

		//validator1.init(1,100, 1);
		//dialog_ctx.validator = &validator1;

		set_ctx(dialog_ctx,false);


	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		if (msg.what == model_changed_what_t::CURSOR) {
			if (!app_model_t::instance.osc.cursor.y.ftoa(dialog_ctx.value,units::INT))
				return;
			set_ctx(dialog_ctx, false);
			this->dirty = true;
			this->do_layout();
		}
	}

	virtual void data_changed() OVERRIDE {
		app_model_t::real_t dpy;
		if (!dpy.atof(dialog_ctx.value, units::INT))
			return;
		app_model_t::instance.cursor_y_changed(dpy);
	} 

	//virtual void key_event(key_t::key_t key) OVERRIDE {
	//	app_model_t::real_t dpx = app_model_t::instance.osc.cursor.y;

	//	if (key == key_t::K_LEFT) {
	//		if (dpx.value > 0) 
	//			dpx.value -= units::INT;
	//		if (dpx.value >= 0) {
	//			app_model_t::instance.cursor_y_changed(dpx);
	//			this->dirty = true;
	//			this->do_layout();
	//		}

	//	} else if (key == key_t::K_RIGHT) {
	//		dpx.value += units::INT;
	//		app_model_t::instance.cursor_y_changed(dpx);
	//		this->dirty = true;
	//		this->do_layout();
	//	}
	//}


};


class graphic_menu_t : public menu_t {
public:
	menu_item_scanx_t mi1;
	menu_item_scany_t mi2;
	menu_item_curonoff_t mi3;
	menu_item_curx_t mi4;
	menu_item_cury_t mi5;
public:
	graphic_menu_t() {
		name = string_t("√–¿‘» ");
	}

	virtual menu_item_t* next_item(void* prev) OVERRIDE {
		if (!prev) return &mi1;
		else if (prev == &mi1) return &mi2;
		else if (prev == &mi2) return &mi3;
		else if (prev == &mi3) return &mi4;
		else if (prev == &mi4) return &mi5;
		return 0;
	}
};

// ---------------------------------------- SCREEN menu ----------------------------------------

class menu_item_brightness_t : public menu_editable_item_t {
public:
	menu_item_brightness_t() {
//		whoami = MENU_ITEM_ZAPR_BRI;
	}
	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("ﬂ– Œ—“‹");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_15;
//		btn.l_mid.text = string_t("100 %");

		dialog_ctx.value = string_t("100");
		dialog_ctx.suffix = suffix_t("%",1);
		set_ctx(dialog_ctx,false);
	}

	virtual void data_changed() OVERRIDE {
	}

};


class menu_item_disptimer_t : public btn_menu_item_t {
public:
	u8 state;
public:
	menu_item_disptimer_t() {
		state = 0;
//		whoami = MENU_ITEM_ZAPR_DSPTMR;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;
		btn.l_top.text = string_t("Œ“ À");

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.ctx.font_size = font_size_t::FS_15;

		btn.l_bot.visible = true;
		btn.l_bot.ctx = ctx.lctx1;
		btn.l_bot.text = string_t("ƒ»—œÀ≈ﬂ");

		next_state(false);
	}
	virtual void next_state() OVERRIDE {
		next_state(true);
	}

	void next_state(bool _update) {
		switch(state) {
		case 0: 
			btn.l_mid.text = string_t("Õ≈“");
			break;
		case 1: 
			btn.l_mid.text = string_t("1 Ã»Õ");
			break;
		case 2: 
			btn.l_mid.text = string_t("5 Ã»Õ");
			break;
		case 3: 
			btn.l_mid.text = string_t("10 Ã»Õ");
			break;
		}
		state++;
		if (state > 3) state = 0;

		app_model_t::instance.screen_saver_changed(state);

		//message_t msg;
		//msg.iparam = state;
		//send_state_changed(msg);

		if (_update) {
			do_layout();
			dirty = true;
		}
	}
};


class menu_item_lang_t : public btn_menu_item_t {
public:
	u8 state;
public:
	menu_item_lang_t() {
		state = 0;
//		whoami = MENU_ITEM_ANT;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;

		btn.l_top.visible = true;
		btn.l_top.ctx = ctx.lctx1;

		btn.l_mid.visible = true;
		btn.l_mid.ctx = ctx.lctx1;
//		btn.l_mid.ctx.font_size = FS_10;

		next_state(false);
	}
	virtual void next_state() OVERRIDE {
		next_state(true);
	}

	void next_state(bool _update) {
		switch(state) {
		case 0: 
			btn.l_top.text = string_t("ﬂ«€ ");
			btn.l_mid.text = string_t("–”—— »…");
			break;
		case 1: 
			btn.l_top.text = string_t("LANGUAGE");
			btn.l_mid.text = string_t("ENGLISH");
			break;
		}
		state++;
		if (state > 1) state = 0;
//		send_state_changed(state);
		if (_update) {
			do_layout();
			dirty = true;
		}
	}
};


class screen_menu_t : public menu_t {
public:
	menu_item_brightness_t mi1;
	menu_item_disptimer_t mi2;
	menu_item_lang_t mi3;
	menu_item_otv_t otv;
	menu_item_offsx_t ofsx;
public:
	screen_menu_t() {
		name = string_t("› –¿Õ");
	}

	virtual menu_item_t* next_item(void* prev) OVERRIDE {
		if (!prev) return &mi1;
		if (prev == &mi1) return &mi2;
		if (prev == &mi2) return &mi3;
		if (prev == &mi3) return &otv;
		if (prev == &otv) return &ofsx;
		return 0;
	}
};


// ----------------------------- ÏÂÌ˛ "≈‰. ËÁÏÂÂÌËˇ\ÒÛÙÙËÍÒ˚" --------------------------------------------

class menu_item_sfx_t : public btn_menu_item_t {
	typedef btn_menu_item_t super;
public:
	input_dialog_t *dlg;
	suffix_t sfx;
	bool set_sfx;
	bool is_cancel;
public:

	menu_item_sfx_t() {
		dlg = 0;
		set_sfx = true;
		is_cancel = false;
	}

	virtual void init() OVERRIDE {
		btn_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		btn.ctx = ctx.bctx1;
		btn.l_mid.ctx = ctx.lctx1;
		btn.l_mid.visible = true;
	}

	void init2(input_dialog_t *dlg, suffix_t sfx) {
		this->sfx = sfx;
		btn.l_mid.text = this->sfx.name;
		btn.do_layout();
		this->dlg = dlg;
	}

	virtual void set_selected(bool selected) OVERRIDE {
		_MY_ASSERT(dlg, return);
		super::set_selected(selected);
		if (selected) {
			if (set_sfx) {
				dlg->set_suffix(sfx);
			}
			dlg->close(!is_cancel);
		}
	}
};

class sfx_menu_t : public menu_t {

public:
	menu_item_sfx_t sfx1;
	menu_item_sfx_t sfx2;
	menu_item_sfx_t sfx3;
	menu_item_sfx_t sfx4;
	menu_item_sfx_t sfx5;
private:
	suffix_t sfx_ok;
	suffix_t sfx_cancel;
public:
	sfx_menu_t() {
		name = string_t("≈ƒ. »«Ã.");
		sfx_ok.name = "OK";
		sfx_cancel.name = "Œ“Ã≈Õ¿";
	}

	virtual menu_item_t* next_item(void* prev) OVERRIDE {
		if (!prev) return &sfx1;
		if (prev == &sfx1) return &sfx2;
		if (prev == &sfx2) return &sfx3;
		if (prev == &sfx3) return &sfx4;
		if (prev == &sfx4) return &sfx5;
		return 0;
	}

	void init2(input_dialog_t *dlg, input_dialog_context_t &ctx) {

		this->focus_manager.select(0);

		menu_item_sfx_t *sfxi = (menu_item_sfx_t *) next_item(0);
		while (sfxi) {
			sfxi->visible = false;
			sfxi->set_sfx = true;
			sfxi->is_cancel = false;
			sfxi = (menu_item_sfx_t *) next_item(sfxi);
		}
		// init suffixes
		if (ctx.suffixes) {
			suffix_t *psfx = ctx.suffixes->next(0);
			sfxi = 0;
			while (psfx) {
				sfxi = (menu_item_sfx_t *) next_item(sfxi);
				_WEAK_ASSERT(sfxi,break);
				sfxi->init2(dlg, *psfx);
				sfxi->visible = true;

				psfx = ctx.suffixes->next(psfx);
			}
		} else {
			sfxi = (menu_item_sfx_t *) next_item(0);
			_MY_ASSERT(sfxi,return);
			sfxi->init2(dlg, sfx_ok);
			sfxi->set_sfx = false;
			sfxi->visible = true;
		}

		sfxi = (menu_item_sfx_t *) next_item(0);
		while (sfxi) {
			if (!sfxi->visible) {
				sfxi->init2(dlg, sfx_cancel);
				sfxi->set_sfx = false;
				sfxi->visible = true;
				sfxi->is_cancel = true;
				break;
			}
			sfxi = (menu_item_sfx_t *) next_item(sfxi);
		}
	}
};

// --------------------- top menu -----------------------------
// double label menu item name-value pair
class dl_mi_nv_pair_t : public dl_menu_item_t {
public:
	wo_property_t<string_t , dl_mi_nv_pair_t> name;
	wo_property_t<string_t , dl_mi_nv_pair_t> value;
private:
//	preferred_stack_layout_t prl;
public:
	dl_mi_nv_pair_t() {
		name.init(this,&dl_mi_nv_pair_t::set_name);
		value.init(this,&dl_mi_nv_pair_t::set_value);
//		preferred_layout = &prl;
	}

	virtual void init() OVERRIDE {
		dl_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		dl.l_top.visible = true;
		dl.l_top.ctx = ctx.lctx1;

		dl.l_bot.visible = true;
		dl.l_bot.ctx = ctx.lctx1;
	}
private:
	void set_name(string_t name) {
		dl.l_top.text = name;
	}
	void set_value(string_t value) {
		dl.l_bot.text = value;
	}
};


class dl_menu_item_12mks_t : public dl_menu_item_t {
public:
	wo_property_t<string_t , dl_menu_item_12mks_t> value;
private:
	void set_value(string_t value) {
		dl.l_bot.text = value;
	}
public:
	dl_menu_item_12mks_t() {
		value.init(this,&dl_menu_item_12mks_t::set_value);
	}

	virtual void init() OVERRIDE {
		dl_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		dl.l_top.visible = true;
		dl.l_top.ctx = ctx.lctxg;
		dl.l_top.ctx.font_size = font_size_t::FS_8;

		dl.l_top.text32 = string32_t(menu_item_req_t::dl_12mks1_str);

		dl.l_bot.visible = true;
		dl.l_bot.ctx = ctx.lctx1;
	}

};


class dl_menu_item_rdy_t : public dl_menu_item_t {
public:
	wo_property_t<string_t , dl_menu_item_rdy_t> value;
private:
	void set_value(string_t value) {
		dl.l_bot.text = value;
		if (value == "√Œ“Œ¬") {
			dl.l_top.glyph_code = 0x0029;
		} else {
			dl.l_top.glyph_code = 0x0028;
		}
	}
public:
	dl_menu_item_rdy_t() {
		value.init(this,&dl_menu_item_rdy_t::set_value);
	}

	virtual void init() OVERRIDE {
		dl_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		dl.preferred_stack_layout.vertical = false;
		dl.center_layout.vertical = false;

		dl.l_top.visible = true;
		dl.l_top.ctx = ctx.lctxg;
		dl.l_top.ctx.font_size = font_size_t::FS_8;
		dl.l_top.glyph_code = 0x0029;

		dl.l_bot.visible = true;
		dl.l_bot.ctx = ctx.lctx1;
	}
};


class progress_bar_part_t : public gobject_t {
public:
	s32 percent;
	stretch_layout_t stretch_layout;
public:
	progress_bar_part_t() {
		layout = &stretch_layout;
		percent = 70;
	}

	virtual void set_preferred_size() OVERRIDE {
		w = 45;
		h = 10;
	}

	void render(surface_t &dst) OVERRIDE {
		s32 ax, ay;
		translate(ax,ay);
		dst.ctx.reset();
		dst.ctx.pen_color = 0xffffff;
		dst.ctx.alfa = 0xff;
		dst.fill(ax,ay,w,h); 
		dst.ctx.pen_color = 0x00;
		dst.ctx.mode = copy_mode_t::cmNone;
		s32 pw = w * percent / 100;
		dst.fill(ax,ay,pw,h);

//		render_children(dst);
	}
};


class progress_bar_t : public menu_item_t {
public:
	property_t<s32,progress_bar_t> percent;
private:
	progress_bar_part_t progress_bar_part;
	center_layout_t clayout;
	preferred_stack_layout_t prl;
private:
	s32 get_percent() {
		return progress_bar_part.percent;
	}
	void set_percent(s32 val) {
		progress_bar_part.percent = val;
	}

public:
	progress_bar_t() {
		layout = &clayout;
		preferred_layout = &prl;
		percent.init(this, &progress_bar_t::get_percent, &progress_bar_t::set_percent);
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &progress_bar_part;
		return 0;
	}

};


class main_top_row_t : public menu_t, public subscriber_t<model_changed_msg_t> {
	typedef menu_t super;
public:
	progress_bar_t progress_bar;
	dl_mi_nv_pair_t dl_ch50x;
	dl_mi_nv_pair_t dl_fprd;
	dl_menu_item_12mks_t dl_12mks1;
	dl_mi_nv_pair_t dl_fprm;
	dl_menu_item_12mks_t dl_12mks2;
	dl_menu_item_rdy_t dl_rdy;
	stack_layout_t menu_layout;
private:
	string_impl_t<INPUT_MAX_LEN> dl_ch50x_value;
	string_impl_t<INPUT_MAX_LEN> dl_fprd_value;
	string_impl_t<INPUT_MAX_LEN> dl_12mks1_value;
	string_impl_t<INPUT_MAX_LEN> dl_fprm_value;
	string_impl_t<INPUT_MAX_LEN> dl_12mks2_value;

public:
	main_top_row_t() {
		layout = &menu_layout;
		menu_layout.preferred_item_size = true;
	}

	virtual void init() OVERRIDE {
		super::init();

		dl_ch50x.name = (string_t(" ¿Õ¿À"));
		dl_ch50x.value = (string_t("50X"));

		dl_fprd.name = (string_t("F œ–ƒ"));
		dl_fprd.value = (string_t("1074 Ã√÷"));

		dl_fprm.name = (string_t("F œ–Ã"));
		dl_fprm.value = (string_t("1011 Ã√÷"));

		dl_12mks1.value = (string_t("12,0 Ã —"));
		dl_12mks2.value = (string_t("12,0 Ã —"));

		dl_rdy.value = (string_t("√Œ“Œ¬"));

		app_model_t::instance.subscribe(this);
	}

	virtual menu_item_t* next_item(void* prev) OVERRIDE {
		if (!prev) return &progress_bar;
		else if (prev == &progress_bar) return &dl_ch50x;
		else if (prev == &dl_ch50x) return &dl_fprd;
		else if (prev == &dl_fprd) return &dl_12mks1;
		else if (prev == &dl_12mks1) return &dl_fprm;
		else if (prev == &dl_fprm) return &dl_12mks2;
		else if (prev == &dl_12mks2) return &dl_rdy;
		return 0;
	}

	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
		//u32 num = 0;
		switch (msg.what) {
		case model_changed_what_t::DME:
			dirty = true;
			if (!conv_utils::ftoa<u32>(app_model_t::instance.dme.ch,dl_ch50x_value,1))
				return;
			dl_ch50x_value += menu_item_dme_t::suffixes_t::instance.get_val(app_model_t::instance.dme.suffix);
			dl_ch50x.value = dl_ch50x_value;
			break;
		case model_changed_what_t::REQ_FREQ:
			dirty = true;
			if (!app_model_t::instance.req_f.ftoa(dl_fprd_value,units::MHZ))
				return;
			dl_fprd_value += string_t(" Ã√÷");
			dl_fprd.value = dl_fprd_value;
			break;
		case model_changed_what_t::RSP_FREQ:
			dirty = true;
			if (!app_model_t::instance.rsp_f.ftoa(dl_fprm_value,units::MHZ))
				return;
			dl_fprm_value += string_t(" Ã√÷");
			dl_fprm.value = dl_fprm_value;
			break;
		case model_changed_what_t::REQ_SPAN:
			dirty = true;
			if (!app_model_t::instance.req_span.ftoa(dl_12mks1_value,units::MKS))
				return;
			dl_12mks1_value += string_t(" Ã —");
			dl_12mks1.value = dl_12mks1_value;
			break;
		case model_changed_what_t::RSP_SPAN:
			dirty = true;
			if (!app_model_t::instance.rsp_span.ftoa(dl_12mks2_value,units::MKS))
				return;
			dl_12mks2_value += string_t(" Ã —");
			dl_12mks2.value = dl_12mks2_value;
			break;
		case model_changed_what_t::BAT_STAT:
			dirty = true;
			progress_bar.percent = app_model_t::instance.bat_stat;
			break;
		case model_changed_what_t::DEV_STAT:
			dirty = true;
			if (app_model_t::instance.dev_status) {
				dl_rdy.value = (string_t("œ≈–≈ƒ"));
			} else {
				dl_rdy.value = (string_t("√Œ“Œ¬"));
			}
			break;
		default : break;
		} 

	}


};

// ÍÓÏÔÓÁËÚÌ˚È Ó·¸ÂÍÚ "–‡Á‚ÂÚÍ‡ ÔÓ „ÓËÁÓÌÚ‡ÎË"

class razh_mi_t : public menu_item_t {
	typedef menu_item_t super;
public:
	wo_property_t<string_t , razh_mi_t> name;
	wo_property_t<string_t , razh_mi_t> value;
private:
	double_label_t zd;
	double_label_t dl;
	label_t arrow;
	preferred_stack_layout_t prl;
	stack_layout_t sl;
public:
	razh_mi_t() {
		name.init(this,&razh_mi_t::set_name);
		value.init(this,&razh_mi_t::set_value);
		preferred_layout = &prl;
		prl.vertical = false;
		layout = &sl;
		sl.vertical = false;
		sl.preferred_item_size = true;
	}

	virtual void init() OVERRIDE {
		menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		dl.l_top.visible = true;
		dl.l_top.ctx = ctx.lctx1;

		dl.l_bot.visible = true;
		dl.l_bot.ctx = ctx.lctx1;

		zd.l_top.visible = true;
		zd.l_top.ctx = ctx.lctxg;
		zd.l_top.ctx.font_size = font_size_t::FS_8;
		zd.l_top.glyph_code = 0x0034;

		zd.l_bot.visible = true;
		zd.l_bot.ctx = ctx.lctxg;
		zd.l_bot.ctx.font_size = font_size_t::FS_8;
		zd.l_bot.glyph_code = 0x0033;
		zd.set_preferred_size();

		arrow.visible = true;
		arrow.ctx = ctx.lctxg;
		arrow.ctx.font_size = font_size_t::FS_8;
		arrow.glyph_code = 0x0036;
		arrow.set_preferred_size();
	}

	virtual void set_preferred_size() OVERRIDE {
		super::set_preferred_size();
	}

	virtual void do_layout() OVERRIDE {
		super::do_layout();
		arrow.set_preferred_size();
		arrow.y = (h-arrow.h)/2;
		//zd.x = zd.y = 0;
		//dl.y = 0;
		//zd.h = dl.h = h;
		//arrow.y = (h-arrow.h)/2;
		//s32 spx = 5;
		//dl.x = zd.x + zd.w + spx;
		//dl.w = w - zd.w - spx - arrow.w;
		//arrow.x = zd.x+zd.w+dl.x + dl.w + spx;
		//layout_children();
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &zd;
		else if (prev == &zd) return &dl;
		else if (prev == &dl) return &arrow;
		return 0;
	}
private:
	void set_name(string_t name) {
		dl.l_top.text = name;
	}
	void set_value(string_t value) {
		dl.l_bot.text = value;
	}
};

class razv_mi_t : public menu_item_t {
public:
	wo_property_t<string_t , razv_mi_t> value;
private:
	label_t arrow;
	label_t l_val;
	preferred_stack_layout_t prl;
public:

	razv_mi_t() {
		value.init(this,&razv_mi_t::set_value);
		preferred_layout = &prl;
		prl.vertical = false;
	}

	virtual void init() OVERRIDE {
		menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		arrow.visible = true;
		arrow.ctx = ctx.lctxg;
		arrow.ctx.font_size = ctx.lctx1.font_size;
		arrow.glyph_code = 0x002d;
		arrow.set_preferred_size();

		l_val.visible = true;
		l_val.ctx = ctx.lctx1;
	}

	virtual void do_layout() OVERRIDE {
		arrow.x = arrow.y = 0;
		l_val.y = 0;
		s32 spx = 5;
		l_val.set_preferred_size();
		l_val.x = arrow.x + arrow.w + spx;
		l_val.w = w - l_val.x;
		layout_children();
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &arrow;
		else if (prev == &arrow) return &l_val;
		return 0;
	}
private:
	void set_value(string_t value) {
		l_val.text = value;
	}

};

class ip1300_mi_t : public dl_menu_item_t {
public:
	wo_property_t<string_t , ip1300_mi_t> value;
public:
	ip1300_mi_t() {
		value.init(this,&ip1300_mi_t::set_value);
	}

	virtual void init() OVERRIDE {
		dl_menu_item_t::init();
		menu_context_t &ctx = menu_context_t::instance;
		dl.l_top.visible = true;
		dl.l_top.ctx = ctx.lctxg;
		dl.l_top.ctx.font_size = ctx.lctx1.font_size;
		dl.l_top.glyph_code = 0x002b;

		dl.l_bot.visible = true;
		dl.l_bot.ctx = ctx.lctx1;
	}

	virtual void set_preferred_size() OVERRIDE {
		dl.set_preferred_size();
		w = dl.w;
		h = dl.h;
	}
private:
	void set_value(string_t value) {
		dl.l_bot.text = value;
	}
};

class ip1300_mi_t_composite : public menu_item_t {
public:
	ip1300_mi_t  ip1300;
	dl_mi_nv_pair_t hip_imp;
	preferred_stack_layout_t prl;
public:
	ip1300_mi_t_composite() {
		preferred_layout = &prl;
		prl.vertical = false;

		ip1300.value = (string_t("1300"));

		hip_imp.name = (string_t("’»œ"));
		hip_imp.value = (string_t("»Ãœ.œ¿–/C"));
	}

	virtual void do_layout() OVERRIDE {
		s32 spx = 5;
		ip1300.x = 0;
		ip1300.y = 0;
		ip1300.set_preferred_size();
		ip1300.h = h;
		hip_imp.x = ip1300.w + spx;
		hip_imp.y = 0;
		hip_imp.h = h;
		hip_imp.w = w - hip_imp.x;

		layout_children();
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &ip1300;
		//else if (prev == &ip1300) return &hip_imp;
		return 0;
	}

};


class main_bot_row_t : public menu_t, public subscriber_t<model_changed_msg_t> {
	typedef menu_t super;
public:
	razh_mi_t razh;
	razv_mi_t razv;
	dl_mi_nv_pair_t req;
	dl_mi_nv_pair_t answ;
	dl_mi_nv_pair_t ksvn;
	ip1300_mi_t_composite ip1300;
	ip1300_mi_t_composite ip1500;

	stack_layout_t menu_layout;
private:
	string_impl_t<INPUT_MAX_LEN> req_value;
	string_impl_t<INPUT_MAX_LEN> otv_value;
	string_impl_t<INPUT_MAX_LEN> rf_value;
	string_impl_t<INPUT_MAX_LEN> rsp_lev_value;
	string_impl_t<INPUT_MAX_LEN> ksvn_value;
	string_impl_t<INPUT_MAX_LEN> resx_value;
	string_impl_t<INPUT_MAX_LEN> resy_value;
	
public:
	main_bot_row_t() {
		layout = &menu_layout;
		menu_layout.preferred_item_size = true;
	}

	virtual void init() OVERRIDE {
		super::init();

		app_model_t::instance.subscribe(this);

		razh.name = string_t("1 Ã—");
		razh.value = string_t("100 Ã");

		razv.value = string_t("100 %");

		req.name = string_t("«¿œ–Œ—");
		req.value = string_t("+10 ƒ¡Ã");

		answ.name = string_t("Œ“¬≈“");
		answ.value = string_t("-76 ƒ¡Ã");

		ksvn.name = string_t(" —¬Õ");
		ksvn.value = string_t("1,63");
	}


	virtual void accept(model_changed_msg_t &msg) OVERRIDE {
//		s32 num = 0;
		switch (msg.what) {

		case model_changed_what_t::RESOLUTION_Y:
			dirty = true;
			if (!app_model_t::instance.osc.dpy.ftoa(resy_value,units::INT))
				return;
			resy_value.append(string_t(" %"));
			razv.value = resy_value;
			break;

		case model_changed_what_t::RESOLUTION_X:
			dirty = true;
			if (!app_model_t::instance.osc.dpx.ftoa(resx_value,app_model_t::instance.osc.dpx_sfx))
				return;

			//suffix_t matched = ;

			resx_value.append(units::time2_suffixes_t::match(
				app_model_t::instance.osc.dpx_sfx
				).name);

			razh.name = resx_value;
			break;

		case model_changed_what_t::REQ_FREQ_ZAPR:
			dirty = true;
			if (!conv_utils::ftoa<u32>(app_model_t::instance.req_zapr,req_value,1))
				return;
			ip1300.ip1300.value = req_value;
			break;

		case model_changed_what_t::REQ_FREQ_OTV:
			dirty = true;
			if (!conv_utils::ftoa<u32>(app_model_t::instance.req_otv,otv_value,1))
				return;
			ip1500.ip1300.value = otv_value;
			break;

		case model_changed_what_t::REQ_LEVEL:
			dirty = true;
			if (!conv_utils::ftoa<s32>(app_model_t::instance.req_lev,rf_value,1))
				return;
			rf_value += string_t(" ƒ¡Ã");
			req.value = rf_value;
			break;

		case model_changed_what_t::RSP_LEVEL:
			dirty = true;
			if (!conv_utils::ftoa<s32>(app_model_t::instance.rsp_lev,rsp_lev_value,1))
				return;
			rsp_lev_value += string_t(" ƒ¡Ã");
			answ.value = rsp_lev_value;
			break;

		case model_changed_what_t::KSVN:
			dirty = true;
			if (!app_model_t::instance.ksvn.ftoa(ksvn_value,units::INT))
				return;
			ksvn.value = ksvn_value;
			break;
		default : break;
		}
	}


	virtual menu_item_t* next_item(void* prev) OVERRIDE {
		if (!prev) return &razv;
		else if (prev == &razv) return &razh;
		else if (prev == &razh) return &req;
		else if (prev == &req) return &answ;
		else if (prev == &answ) return &ksvn;
		else if (prev == &ksvn) return &ip1300;
		else if (prev == &ip1300) return &ip1500;
		return 0;
	}

};

// ----------------------- ÒÓÒÚ‡‚Ì˚Â Ó·¸ÂÍÚ˚ -----------------------------

// ÛÔ‡‚ÎˇÂÚ ÔÂÂÏÂ˘ÂÌËÂÏ ÔÓ ÏÂÌ˛, ÓÚÓ·‡ÊÂÌËÂÏ ÒÓÓÚ‚. ÏÂÌ˛
class menu_set_t : public gobject_t, public menu_set_i {
	typedef gobject_t super;
public:
	label_t l_name;
	menu_t *current_menu;
	menu_t *last_menu;

	main_menu_t main_menu;
	param_menu_t param_menu;
	graphic_menu_t graphic_menu;
	screen_menu_t screen_menu;
	sfx_menu_t sfx_menu;

	stack_layout_t menu_layout;
public:

	menu_set_t() {
		current_menu = 0;
		last_menu = 0;
		l_name.visible = true;
		l_name.text = string_t("unset");
	}

	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(current_menu,return);
		l_name.set_preferred_size();

		current_menu->x = 0;
		current_menu->y = 0;
		current_menu->w = w;
		current_menu->h = h - l_name.h;
		l_name.x = (w - l_name.w)/2;
		l_name.y = h - l_name.h - 3;

		layout_children();
	}

	menu_t* next_menu(void* prev) {
		if (!prev) return &main_menu;
		else if (prev == &main_menu) return &param_menu;
		else if (prev == &param_menu) return &graphic_menu;
		else if (prev == &graphic_menu) return &screen_menu;
		else if (prev == &screen_menu) return &sfx_menu;
		return 0;
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		super::set_dirty(dirty);
		_MY_ASSERT(parent, return);
		parent->set_dirty(dirty);
	}

	void set_menu(menu_t *menu) {
		set_menu(menu,true);
	}

	void set_menu(menu_t *menu, bool upd) {
		menu->visible = true;
		current_menu = menu;
		this->focus_manager.selected = menu;
		l_name.text = menu->name;

		menu_t *m = next_menu(0);
		while(m) {
			m->visible = (m == menu);
			m = next_menu(m);
		}

		if (upd) {
			do_layout();
			dirty = true;
		}
	}

	virtual void init() OVERRIDE {
		gobject_t::init();

		menu_t *menu = next_menu(0);
		while(menu) {
			menu->layout = &menu_layout;
			menu = next_menu(menu);
		}

		menu_context_t &ctx = menu_context_t::instance;
		l_name.ctx = ctx.lctx1;

		set_menu(next_menu(0), false);
	}

//TODO: change key bindings

	virtual void key_event(key_t::key_t key) OVERRIDE {

		_MY_ASSERT(current_menu,return);

#define _MENUS_LEN 4
		menu_t* menus[4] = {&main_menu, &param_menu, &graphic_menu, &screen_menu};

		if (key == key_t::K_LEFT || key == key_t::K_RIGHT) {
			s32 j = 0;
			for (s32 i=0; i < _MENUS_LEN; i++) {

				if (menus[i] == current_menu) {

					if (key == key_t::K_LEFT) {
						j = i-1;
						if (j < 0) j = _MENUS_LEN-1;

					} else if (key == key_t::K_RIGHT) {
						j = i+1;
						if (j >= _MENUS_LEN) j = 0;
					}
					break;
				}
			}
			set_menu(menus[j]);
			return;
		}
		super::key_event(key);

	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &l_name;
		else if (prev == &l_name) return next_menu(0);
		return next_menu(prev);
	}
// menu_set_i implementation
	virtual void key_event_fwd(key_t::key_t key) OVERRIDE {
		key_event(key);
	}

	virtual void set_sfx_menu(input_dialog_t *dlg, input_dialog_context_t &ctx) OVERRIDE {
		last_menu = current_menu;
		set_menu(&sfx_menu);
		sfx_menu.init2(dlg, ctx);
	}

	virtual void ret_sfx_menu() OVERRIDE {
		if (last_menu)
			set_menu(last_menu);
	}

};

} // ns
#endif
