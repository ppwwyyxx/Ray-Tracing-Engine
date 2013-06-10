// File: cvrender.hh
// Date: Mon Jun 10 21:18:36 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

#include "render.hh"
#include <opencv2/opencv.hpp>

class CVRender: public RenderBase {
	cv::Mat img;

	public:
		int finish();

		void save(const char* fname = "output.png");

		CVRender(const Geometry &m_g);

		CVRender(int w, int h):
			CVRender(Geometry(w, h)){}


		~CVRender() {}

	protected:
		void _write(int x, int y, const Color& c);
};

