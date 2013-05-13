#include <iostream>
#include <fstream>

#define cimg_use_jpeg
#include "CImg.h"

#include "bmp_math.h"
#include "types.h"
#include "surface.h"

using namespace cimg_library;
using namespace std;

int main(int argc, char **argv) {


	if (argc < 3) {
		cout << "usage : .exe .bmp .bin [-bw]" << endl;
		return 1;
	}

	bool isBW = (argc >= 4) && (strncmp(argv[3],"-bw",3) == 0);

	if (isBW) {
		cout << "BW mode !" << endl;
	}

	CImg<unsigned char> img(argv[1]);

	int w = img.width();
    int h = img.height();
    cout << w << "x" << h << endl;

	int size = 0;

	if (!isBW) {
		size = BMP_GET_SIZE(w,h,24);
		u8* buf = new u8[size];

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				rgb_t *p = (rgb_t *)&buf[BMP_GET_OFFS(x,y,w,24)];
				p->r = img(x,y,0,0);
				p->g = img(x,y,0,1);
				p->b = img(x,y,0,2);
			}
		}
		ofstream outfile (argv[2],ofstream::binary);
		outfile.write((char *)buf, BMP_GET_SIZE(w,h,24));
		outfile.close();
		delete buf;

	} else {
		// BW mode
		size = BMP_GET_SIZE(w,h,1);
		u8* buf = new u8[size];

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				s32 offs = BMP_GET_OFFS(x,y,w,1);
				u8 msk = BMP_MAKE_MSK(x);
				rgb_t p;
				p.r = img(x,y,0,0);
				p.g = img(x,y,0,1);
				p.b = img(x,y,0,2);
				u32 c = RGB2U32(p.r,p.g,p.b);
				if (THRESHOLD(c)) {
					buf[offs] |= msk;
				} else {
					buf[offs] &= ~msk;
				}
			}
		}
		ofstream outfile (argv[2],ofstream::binary);
		outfile.write((char *)buf, BMP_GET_SIZE(w,h,1));
		outfile.close();
		delete [] buf;
	}
	cout << "done " << size << " bytes" << endl;
}
