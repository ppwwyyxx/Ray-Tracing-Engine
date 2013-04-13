// File: image.hh
// Date: Wed Apr 10 13:05:49 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "color.hh"

class ImageReader {
	protected:
		void init(int m_w, int m_h) {
			w = m_w, h = m_h;
			pixel = new Color[w * h];
		}
	public:
		int w, h;
		Color* pixel;

		ImageReader(){}

		ImageReader(int m_w, int m_h)
		{ init(m_w, m_h); }

		~ImageReader()
		{ delete[] pixel; }
};

class MagickReader : public ImageReader {
	public:
		MagickReader(const char* fname);

};
