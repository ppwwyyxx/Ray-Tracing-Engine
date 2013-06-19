// File: cvrender.cc
// Date: Wed Jun 19 11:45:55 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <opencv2/opencv.hpp>
#include <omp.h>
#include <algorithm>

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
#define KEY_lbracket 91
#define KEY_rbracket 93

#define VIEWER_ANGLE 15
#define ZOOMING 1.2
#define SHIFT_DISTANCE 20
#define SHIFT_SCREEN 3


CVRender::CVRender(const Geometry &m_g):
	RenderBase(m_g) {
		img.create(m_g.h, m_g.w, CV_8UC3);
		img.setTo(Scalar(0, 0, 0));
}

int CVRender::finish() {
	imshow("show", img);
	int k = waitKey(0);
	return k;
}

void CVRender::save(const char* fname) { imwrite(fname, img); }

void CVRender::_write(int x, int y, const Color& c) {
	// bgr color space
	img.ptr<uchar>(y)[x * 3] = c.z * 255;
	img.ptr<uchar>(y)[x * 3 + 1] = c.y * 255;
	img.ptr<uchar>(y)[x * 3 + 2] = c.x * 255;
}

Color CVRender::get(const Mat& img, int i, int j) const
{ return Color(img.ptr<uchar>(j)[i * 3] / 255.0,
		img.ptr<uchar>(j)[i * 3 + 1] / 255.0,
		img.ptr<uchar>(j)[i * 3 + 2] / 255.0); }

/*
 *void CVRender::antialias() {
 *    float kernel[9] = {1, 2, 1,
 *                     2, 4, 2,
 *                     1, 2, 1};
 *    double sum = std::accumulate(kernel, kernel + 9, 0);
 *    REP(k, 9) kernel[k] /= sum;
 *
 *    Mat km = Mat(3, 3, CV_32F, kernel);
 *    Mat dst;
 *    dst.create(img.size(), img.type());
 *    filter2D(img, dst, img.depth(), km);
 *    img = dst;
 *}
 */

void CVRender::antialias() {
	float kernel[9] = {1, 2, 1,
					 2, 4, 2,
					 1, 2, 1};
	double sum = std::accumulate(kernel, kernel + 9, 0);
	REP(k, 9) kernel[k] /= sum;

	vector<Coor> cand;

	REPL(i, 1, img.size().width - 1) REPL(j, 1, img.size().height - 1) {
		Color col = get(img, i, j);
		real_t s = 0;
		for (int di : {-1, 0, 1}) for (int dj : {-1, 0, 1}) {
			Color newcol = get(img, i + di, j + dj);
			s += (newcol - col).sqr();
		}
		if (s > 5) cand.push_back(Coor(i, j));
	}
	Mat dst = img;
	for (auto &k : cand) {
		Color newcol = Color::BLACK;
		for (int di : {-1, 0, 1}) for (int dj : {-1, 0, 1})
			newcol += get(dst, k.x + di, k.y + dj) * kernel[(di + 1) * 3 + dj + 1];
		_write(k.x, k.y, newcol);
	}
}

void CVRender::blur() {
	Mat dst = img;
	GaussianBlur(dst, img, Size(3, 3), 4);
}

void CVViewer::render_all() {
	Timer timer;
#pragma omp parallel for schedule(dynamic)
	REP(i, geo.h) {
		if (!omp_get_thread_num()) {
			printf("progress: %d %%\r", (i * 100 / geo.h));
			fflush(stdout);
		}
		REP(j, geo.w) {
			Color col = v.render(i, j);
			r.write(j, i, col);
		}
	}
	printf("Spend %lf seconds\n", timer.get_time());
//	r.antialias();
	if (gaussian_blur) r.blur();
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
					cout << "middle: " << v.mid <<", size: " << v.size << endl;
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
				case KEY_lbracket:
					v.move_screen(-SHIFT_SCREEN);
					break;
				case KEY_rbracket:
					v.move_screen(SHIFT_SCREEN);
					break;
				default:
					rerender = false;
					cout << ret << endl;
					break;
			}

		}
	}
}

