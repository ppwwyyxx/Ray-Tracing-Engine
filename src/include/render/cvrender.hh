// File: cvrender.hh
// Date: Tue Jun 18 19:30:20 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once
#include <opencv2/opencv.hpp>

#include "render.hh"

class CVRender: public RenderBase {
	cv::Mat img;

	public:
		int finish();

		void save(const char* fname = "output.png");

		CVRender(const Geometry &m_g);

		CVRender(int w, int h):
			CVRender(Geometry(w, h)){}

		~CVRender() {}

		void antialias();

		Color get(const cv::Mat& img, int x, int y) const;

	protected:
		void _write(int x, int y, const Color& c);
};

