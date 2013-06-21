// File: imagereader.hh
// Date: Fri Jun 21 18:50:03 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "color.hh"

class ImageReader {
	protected:
		void init(int _w, int _h) {
			size.w = _w, size.h = _h;
			pixel = new Color[_w * _h];
		}
	public:
		Geometry size;
		Color* pixel;

		ImageReader(){}

		ImageReader(int _w, int _h)
		{ init(_w, _h); }

		Color get(int x, int y) const
		{ m_assert(x < size.h && y < size.w); return pixel[x * size.w + y]; }

		~ImageReader()
		{ delete[] pixel; }
};

class MagickReader : public ImageReader {
	public:
		MagickReader(const char* fname);
};
