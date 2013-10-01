// File: myrender.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once
#include <opencv2/opencv.hpp>

#include "render.hh"

class MyRender: public RenderBase {
	cv::Mat img;

	public:
		int finish() override;

		void save(const char* fname = "output.png");

		MyRender(const Geometry &_g);

		MyRender(int w, int h):
			MyRender(Geometry(w, h)){}

		~MyRender() {}

		void antialias();

		void blur();

		void gamma_correction();

		static Color get(const cv::Mat& img, int x, int y);

		Color get(int x, int y) const
		{ return MyRender::get(img, x, y); }

	protected:
		void _write(int x, int y, const Color& c) override;
};

