// File: imagereader.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <string>
#include <Magick++.h>
#include "color.hh"

class ImageReader {
	protected:
		void init(int _w, int _h) {
			size.w = _w, size.h = _h;
			pixel = new Color[size.area()];
		}
	public:
		Geometry size;
		Color* pixel;

		ImageReader(){}

		ImageReader(int _w, int _h)
		{ init(_w, _h); }

		Color get(int x, int y) const {
			m_assert(x < size.h && y < size.w);
			return pixel[x * size.w + y];
		}

		~ImageReader()
		{ delete[] pixel; }
};

class MagickReader : public ImageReader {
	public:
		MagickReader(const std::string& fname) {

			Magick::Image img(fname);
			Magick::Geometry tsize = img.size();
			init(tsize.width(), tsize.height());

			::Color *dest = pixel;

			const Magick::PixelPacket* src = img.getConstPixels(0, 0, size.w, size.h);
			REP(y, size.h) REP(x, size.w) {
				dest->r = double(src->red) / QuantumRange;
				dest->g = double(src->green) / QuantumRange;
				dest->b = double(src->blue) / QuantumRange;
				dest ++;
				src ++;
			}


		}
};
