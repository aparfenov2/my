#include "CImg.h"
#include "win32/test_tools.h"
#include "app_events.h"
#include <iostream>

using namespace std;
using namespace cimg_library;
using namespace myvi;

void draw_img(surface_t &s, CImg<unsigned char> &img) {
	for (int y = 0; y < s.h; y++) {
		for (int x = 0; x < s.w; x++) {
			u32 c = s.getpx(x,y);
			img(x,y,0,0) = U32R(c);
			img(x,y,0,1) = U32G(c);
			img(x,y,0,2) = U32B(c);
		}
	}
}

key_t::key_t translate_key(CImgDisplay &dsp) {
	key_t::key_t key = (key_t::key_t)0;
	if (dsp.is_keyESC()) key = key_t::K_ESC; 
	else if (dsp.is_keyARROWLEFT()) key = key_t::K_LEFT; 
	else if (dsp.is_keyARROWRIGHT()) key = key_t::K_RIGHT; 
	else if (dsp.is_keyARROWUP()) key = key_t::K_UP; 
	else if (dsp.is_keyARROWDOWN()) key = key_t::K_DOWN; 
	else if (dsp.is_keyENTER()) key = key_t::K_ENTER; 
	else if (dsp.is_keyBACKSPACE()) key = key_t::K_BKSP;
	else if (dsp.is_keyF1()) key = key_t::K_F1;
	else if (dsp.is_keyF2()) key = key_t::K_F2;
	else if (dsp.is_keyF3()) key = key_t::K_F3;
	else if (dsp.is_keyF4()) key = key_t::K_F4;
	else if (dsp.is_keyF5()) key = key_t::K_F5;
	else if (dsp.is_key0() || dsp.is_keyPAD0()) key = key_t::K_0;
	else if (dsp.is_key1() || dsp.is_keyPAD1()) key = key_t::K_1;
	else if (dsp.is_key2() || dsp.is_keyPAD2()) key = key_t::K_2;
	else if (dsp.is_key3() || dsp.is_keyPAD3()) key = key_t::K_3;
	else if (dsp.is_key4() || dsp.is_keyPAD4()) key = key_t::K_4;
	else if (dsp.is_key5() || dsp.is_keyPAD5()) key = key_t::K_5;
	else if (dsp.is_key6() || dsp.is_keyPAD6()) key = key_t::K_6;
	else if (dsp.is_key7() || dsp.is_keyPAD7()) key = key_t::K_7;
	else if (dsp.is_key8() || dsp.is_keyPAD8()) key = key_t::K_8;
	else if (dsp.is_key9() || dsp.is_keyPAD9()) key = key_t::K_9;
	else if (dsp.key(1) == 17 && dsp.is_keyS()) key = key_t::K_SAVE;
	else if (dsp.key(1) == 17 && dsp.is_keyQ()) key = key_t::K_EXIT;
	else if (dsp.key(0) == 188 || dsp.key(0) == 190) key = key_t::K_DOT;
	else if (dsp.is_keyDELETE()) key = key_t::K_DELETE;
//	cout << "keycode: " << dsp.key(0) << ", translated: " << key << endl;
	return key;
};

key_t::key_t last_key = (key_t::key_t)0;
mkey_t::mkey_t last_mkey = mkey_t::MK_NONE;
s32 last_x = -1, last_y = -1;
time_t last_time = 0;

CImg<unsigned char> *img = 0;
CImgDisplay *dsp = 0;

void test_drawer_t::create_window(surface_t &s) {
	img = new CImg<unsigned char>(s.w,s.h,1,3);
	draw_img(s, *img);
	dsp = new CImgDisplay(*img,"Surface Viewer");

	dsp->resize(w,h);


}


bool test_drawer_t::cycle(surface_t &s) {
	bool update = false;

	key_t::key_t key = translate_key(*dsp);
	update = key != last_key;
	last_key = key;
	if (!update) 
		key = (key_t::key_t)0;

	bool exit = dsp->is_closed() || key == key_t::K_EXIT;
	if (exit) return true;

	mkey_t::mkey_t mkey = mkey_t::MK_NONE;
	if (dsp->button() & 0x01) 
		mkey = mkey_t::MK_1;
	else if (dsp->button() & 0x02) mkey = mkey_t::MK_2;
	else if (dsp->button() & 0x04) mkey = mkey_t::MK_3;

	if (!update) 
		update = mkey != last_mkey;
	last_mkey = mkey;

	s32 mx = dsp->mouse_x(), my = dsp->mouse_y();
	if (!update)
		update = mx != last_x || my != last_y;
	last_x = mx;
	last_y = my;

//		cout << mx << "," << my << endl;

	static u32 tic = 0;
	tic++;
	u32 tim = tic / 10;
//		time(&tim);
	bool tik = tim != last_time;
	if (!update) 
		update = tik;
	last_time = tim;
	//if (tik)
	//	tick();

	if (update)
		update = callback(key, mx, my, mkey);

	if (update) {
		draw_img(s, *img);
		img->display(*dsp);
	}
	dsp->wait(1);
	return false;
}

