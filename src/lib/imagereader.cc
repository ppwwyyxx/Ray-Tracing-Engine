// File: imagereader.cc
// Date: Mon Sep 23 01:24:36 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "lib/imagereader.hh"
using namespace std;

#include <Magick++.h>
using namespace Magick;

MagickReader::MagickReader(const char* fname) {

	Image img(fname);
	Magick::Geometry tsize = img.size();
	init(tsize.width(), tsize.height());

	::Color *dest = pixel;

	const PixelPacket* src = img.getConstPixels(0, 0, size.w, size.h);
	REP(y, size.h) REP(x, size.w) {
		dest->x = double(src->red) / QuantumRange;
		dest->y = double(src->green) / QuantumRange;
		dest->z = double(src->blue) / QuantumRange;
		dest ++;
		src ++;
	}

}

