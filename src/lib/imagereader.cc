// File: imagereader.cc
// Date: Thu Jun 13 13:22:22 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "lib/imagereader.hh"
using namespace std;

#include <Magick++.h>
using namespace Magick;

MagickReader::MagickReader(const char* fname) {

	Image img(fname);
	Magick::Geometry size = img.size();
	init(size.width(), size.height());

	::Color *dest = pixel;

	const PixelPacket* src = img.getConstPixels(0, 0, w, h);
	for (int y = 0; y < h; y ++)
		for (int x = 0; x < w; x ++)
		{
			dest->x = double(src->red) / QuantumRange;
			dest->y = double(src->green) / QuantumRange;
			dest->z = double(src->blue) / QuantumRange;
			dest ++;
			src ++;
		}

}

