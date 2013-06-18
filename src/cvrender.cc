// File: cvrender.cc
// Date: Tue Jun 18 18:02:56 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <opencv2/opencv.hpp>
#include <omp.h>

#include "render/cvrender.hh"
#include "viewer.hh"
#include "lib/utils.hh"
#include "lib/Timer.hh"
using namespace cv;


#define KEY_EXIT -1
#define KEY_ESC 27
#define KEY_S 115
#define KEY_Q 113
#define KEY_P 112

#define KEY_J 106
#define KEY_K 107
#define KEY_H 104
#define KEY_L 108
#define KEY_UP 65362
#define KEY_DOWN 65364
#define KEY_LEFT 65361
#define KEY_RIGHT 65363
#define KEY_plus 61
#define KEY_minus 45

#define VIEWER_ANGLE 15
#define ZOOMING 1.2
#define SHIFT_DISTANCE 20


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
	Timer timer;
	/*
	 *int i = 230, j = 230;
	 *v.render(i, j, true);
	 */

#pragma omp parallel for schedule(dynamic)
	REP(i, geo.h) {
		if (omp_get_thread_num() == 0) {
			print_debug("progress: %d %%\r", (i * 100 / geo.h));
			fflush(stderr);
		}
		REP(j, geo.w) {
			Color col = v.render(i, j);
			r.write(j, i, col);
		}
	}
	print_debug("Spend %lf seconds\n", timer.get_time());
}

void CVViewer::view() {
	while (true) {
		render_all();

		bool rerender = false;
		while (!rerender) {
			int ret = r.finish();
			rerender = true;
			switch (ret) {
				case KEY_EXIT:
				case KEY_Q:
				/*
				 *case KEY_ESC:
				 */
					return;
					break;
				case KEY_S:
					r.save();
					rerender = false;
					print_debug("saved\n");
					break;
				case KEY_P:
					cout << "viewpoint: " << v.view_point << endl;
					cout << "middle: " << v.mid << endl;
					rerender = false;
					break;
				case KEY_LEFT:
					v.rotate(VIEWER_ANGLE);
					break;
				case KEY_RIGHT:
					v.rotate(-VIEWER_ANGLE);
					break;
				case KEY_UP:
					v.twist(VIEWER_ANGLE);
					break;
				case KEY_DOWN:
					v.twist(-VIEWER_ANGLE);
					break;
				case KEY_J:
					v.shift(-SHIFT_DISTANCE, false);
					break;
				case KEY_K:
					v.shift(SHIFT_DISTANCE, false);
					break;
				case KEY_H:
					v.shift(-SHIFT_DISTANCE, true);
					break;
				case KEY_L:
					v.shift(SHIFT_DISTANCE, true);
					break;
				case KEY_plus:
					v.zoom(ZOOMING);
					break;
				case KEY_minus:
					v.zoom(1.0 / ZOOMING);
					break;
				default:
					rerender = false;
					cout << ret << endl;
					break;
			}

		}
	}
}
