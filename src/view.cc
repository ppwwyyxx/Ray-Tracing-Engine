// File: view.cc
// Date: Tue Apr 09 10:31:08 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "view.hh"

using namespace std;

View::View(const std::shared_ptr<Space> m_sp, const Vec& m_view_point,
				const Vec& m_mid, real_t w, const Geometry& m_geo):
			geo(m_geo)
{
	view_point = m_view_point, sp = m_sp, mid = m_mid, size = w;
	Vec norm = (view_point - mid).get_normalized();
	Vec tmp;		// random point outside the ray
	if ((fabs(norm.x) > EPS) || (fabs(norm.y) > EPS))
		tmp = Vec(norm.y, -norm.x, 0);
	else
		tmp = Vec(-norm.z, 0, norm.x);
	dir_w = tmp.get_normalized();
	dir_h = norm.cross(dir_w);
	m_assert(fabs(dir_h.sqr()) - 1 < EPS);

	resume_dir_vector();
	sp->init();
}

Color View::render(int i, int j, bool debug) const {
	// pre-caculate to accelerate this!
	Vec corner = mid - dir_h * (geo.h / 2) - dir_w * (geo.w / 2);
	Vec dest = corner + dir_h * i + dir_w * j;
	Ray ray(view_point, dest - view_point, true);
	if (debug)
		ray.debug = true;
	return sp->trace(ray);
}

void View::twist(int angle) {
	normalize_dir_vector();
	Vec norm = view_point - mid;
	real_t alpha = M_PI * angle / 180;
	dir_w = dir_w * cos(alpha) + dir_h * sin(alpha);
	m_assert(fabs(dir_w.sqr() - 1) < EPS);
	dir_h = norm.cross(dir_w).get_normalized();
	resume_dir_vector();
}


void View::zoom(real_t ratio) {
	ratio = 1 / ratio;
	size *= ratio;
	Vec dir = (view_point - mid);
	view_point = mid + dir * ratio;
	dir_w = dir_w * ratio;
	dir_h = dir_h * ratio;
}

void View::rotate(int angle) {
	normalize_dir_vector();
	real_t alpha = M_PI * angle / 180;
	Vec norm = (view_point - mid).get_normalized();
	norm = norm * cos(alpha) + dir_w * sin(alpha);
	view_point = mid + norm * (view_point - mid).mod();
	m_assert(fabs(dir_w.sqr()) - 1 < EPS);
	dir_w = -norm.cross(dir_h);

	resume_dir_vector();
}
