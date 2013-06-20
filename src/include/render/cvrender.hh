// File: cvrender.hh
// Date: Thu Jun 20 11:25:55 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once
#include <opencv2/opencv.hpp>

#include "render.hh"

class CVRender: public RenderBase {
	cv::Mat img;

	public:
		int finish() override;

		void save(const char* fname = "output.png");

		CVRender(const Geometry &m_g);

		CVRender(int w, int h):
			CVRender(Geometry(w, h)){}

		~CVRender() {}

		void antialias();

		void blur();

		Color get(const cv::Mat& img, int x, int y) const;

		Color get(int x, int y) const
		{ return get(img, x, y); }

	protected:
		void _write(int x, int y, const Color& c) override;
};

