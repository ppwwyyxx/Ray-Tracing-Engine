// File: cvrender.cc
// Date: Sat Jun 15 21:47:29 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <opencv2/opencv.hpp>
#include <omp.h>

#include "render/cvrender.hh"
#include "viewer.hh"
#include "lib/utils.hh"
using namespace cv;

#define KEY_UP 65362
#define KEY_DOWN 65364
#define KEY_LEFT 65361
#define KEY_RIGHT 65363
#define KEY_EXIT -1
#define KEY_ESC 27
#define KEY_S 115
#define KEY_J 106
#define KEY_K 107

#define VIEWER_ANGLE 30
#define ZOOMING 1.2


int CVRender::finish() {
	imshow("show", img);
	int k = waitKey(0);
	return k;
}

void CVRender::save(const char* fname)
{ imwrite(fname, img); }

void CVRender::_write(int x, int y, const Color& c) {
	// bgr color space
	img.ptr<uchar>(y)[x * 3] = c.z * 255;
	img.ptr<uchar>(y)[x * 3 + 1] = c.y * 255;
	img.ptr<uchar>(y)[x * 3 + 2] = c.x * 255;
}

CVRender::CVRender(const Geometry &m_g):
	RenderBase(m_g) {
		img.create(m_g.h, m_g.w, CV_8UC3);
		img.setTo(Scalar(0, 0, 0));
}

void CVViewer::render_all() {
	HWTimer timer;

#pragma omp parallel for schedule(dynamic)
	REP(i, geo.h) {
		if (i % 20 == 0)
			print_debug("progress: %lf\n", (double)i / geo.h);
		REP(j, geo.w) {
			Color col = v.render(i, j);
			r.write(j, i, col);
		}
	}
	print_debug("%lf seconds.\n", timer.get_sec());
}

void CVViewer::view() {
	while (true) {
		render_all();

		bool rerender = false;
		while (!rerender) {
			int ret = r.finish();
			switch (ret) {
				case KEY_EXIT:
				case KEY_ESC:
					return;
					break;
				case KEY_LEFT:
					v.rotate(VIEWER_ANGLE);
					rerender = true;
					break;
				case KEY_RIGHT:
					v.rotate(-VIEWER_ANGLE);
					rerender = true;
					break;
				case KEY_UP:
					v.twist(VIEWER_ANGLE);
					rerender = true;
					break;
				case KEY_DOWN:
					v.twist(-VIEWER_ANGLE);
					rerender = true;
					break;
				case KEY_S:
					r.save();
					print_debug("saved\n");
					break;
				case KEY_J:
					v.zoom(1 / ZOOMING);
					rerender = true;
					break;
				case KEY_K:
					v.zoom(ZOOMING);
					rerender = true;
					break;
				default:
					cout << ret << endl;
					break;
			}

		}
	}
}
