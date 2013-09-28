// File: myrender.cc
// Date: Sat Sep 28 23:58:40 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <iostream>
#include <functional>
#include <opencv2/opencv.hpp>
#include <omp.h>
#include <mutex>
#include <algorithm>

#include "librender/myrender.hh"
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
#define KEY_greater 46
#define KEY_smaller 44

#define VIEWER_ANGLE 15
#define ZOOMING 1.2
#define SHIFT_DISTANCE 20
#define SHIFT_SCREEN 4


MyRender::MyRender(const Geometry &m_g):
	RenderBase(m_g) {
		img.create(m_g.h, m_g.w, CV_8UC3);
		img.setTo(Scalar(0, 0, 0));
}

int MyRender::finish() {
	imshow("show", img);
	int k = waitKey(0);
	return k;
}

void MyRender::save(const char* fname)
{ imwrite(fname, img); }

void MyRender::_write(int x, int y, const Color& c) {
	// bgr color space
	img.ptr<uchar>(y)[x * 3] = c.z * 255;
	img.ptr<uchar>(y)[x * 3 + 1] = c.y * 255;
	img.ptr<uchar>(y)[x * 3 + 2] = c.x * 255;
}

Color MyRender::get(const Mat& img, int i, int j) {
	return Color(img.ptr<uchar>(j)[i * 3 + 2] / 255.0,
		img.ptr<uchar>(j)[i * 3 + 1] / 255.0,
		img.ptr<uchar>(j)[i * 3] / 255.0);
}

/*
 *void MyRender::antialias() {
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

void MyRender::antialias() {
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

void MyRender::gamma_correction() {
	REPL(i, 1, img.size().width - 1) REPL(j, 1, img.size().height - 1) {
		Color col = get(img, i, j);
#define ppp(x) pow((x < EPS) ? EPS : x > 1 ? 1 - EPS : x, 1.0 / 2.2)
		_write(i, j, Color(ppp(col.x), ppp(col.y), ppp(col.z)));
#undef ppp
	}
}

void MyRender::blur() {
	Mat dst = img.clone();
	cv::bilateralFilter(dst, img, -1, 15, 15);
}

void render_and_set(int* line_cnt, View* v, RenderBase* r) {
	static mutex line_cnt_mutex;
	static const int w = r->get_geo().w, h = r->get_geo().h;
	int now_num;
	do {
		{
			std::lock_guard<std::mutex> lock(line_cnt_mutex);
			now_num = (*line_cnt);
			*line_cnt = now_num + 1;
		}
		print_progress(now_num * 100 / h);

		if (now_num >= h) return;
		REP(j, w) {
			Color col = v->render(now_num, j);
			r->write(j, now_num, col);
		}
	} while (true);
}

void CVViewer::render_all() {
	Timer timer;

	// use thread
	int nthread = omp_get_num_procs();
	int render_cnt = 0;
	thread* th = new thread[nthread];
	REP(k, nthread) th[k] = thread(render_and_set, &render_cnt, &v, &r);
	REP(k, nthread) th[k].join();
	delete[] th;

/*	// use openmp
 *#pragma omp parallel for schedule(dynamic)
 *    REP(i, geo.h) {
 *        if (!omp_get_thread_num())
 *            print_progress(i * 100 / geo.h);
 *        REP(j, geo.w) {
 *            Color col = v.render(i, j);
 *            r.write(j, i, col);
 *        }
 *    }
 */


	printf("Render spends %lf seconds\n", timer.get_time());
//	r.antialias();
	if (v.use_dof) r.blur();
	r.gamma_correction();
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
					case KEY_ESC:
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
					v.orbit(-VIEWER_ANGLE);
					break;
				case KEY_RIGHT:
					v.orbit(+VIEWER_ANGLE);
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
					if (!v.use_dof)
						fprintf(stderr, "not using dof!\n");
					else
						v.move_screen(-SHIFT_SCREEN);
					break;
				case KEY_rbracket:
					if (!v.use_dof)
						fprintf(stderr, "not using dof!\n");
					else
						v.move_screen(SHIFT_SCREEN);
					break;
				case KEY_greater:
					v.rotate(-VIEWER_ANGLE);
					break;
				case KEY_smaller:
					v.rotate(+VIEWER_ANGLE);
					break;
				default:
					rerender = false;
					cout << ret << endl;
					break;
			}

		}
	}
}

