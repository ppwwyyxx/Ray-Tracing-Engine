// File: view.cc
// Date: Fri Sep 27 20:21:34 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "view.hh"

using namespace std;

View::View(const Space* _sp, const Vec& _view_point,
				const Vec& _mid, real_t _size, const Geometry& m_geo):
			geo(m_geo), sp(_sp), view_point(_view_point), mid(_mid), size(_size) {
	Vec norm = (view_point - mid).get_normalized();
	origin_norm = norm;
	Vec tmp;
	if ((fabs(norm.y) > EPS) || (fabs(norm.x) > EPS))
		tmp = Vec(-norm.y, norm.x, 0);
	else
		tmp = Vec(-norm.z, 0, norm.x);
	dir_w = tmp;
	dir_h = norm.cross(dir_w);
	m_assert(fabs(dir_h.sqr()) - 1 < EPS);

	resume_dir_vector();
}

Color View::render_antialias(const Vec& dest, int sample) const {
	real_t unit = 1.0 / sample, unit2 = unit / 2;

	Color ret = Color::NONE;
	REP(dx, sample) REP(dy, sample) {
		Vec new_dest = dest - dir_h * (dx * unit + unit2) + dir_w * (dy * unit + unit2);

		if (!use_dof) {
			Ray ray(view_point, new_dest - view_point, 1, true);
			ret += sp->trace(ray);
		} else {
			ret += render_dof(new_dest);
		}
	}
	ret = ret * (1.0 / ::sqr(sample));
	return ret;
}

Color View::render_dof(const Vec& dest) const {
	Vec intersec = view_point + (dest - view_point) * DOF_SCREEN_DIST_FACTOR;
	real_t theta;
	Color dof_res = Color::NONE;
	REP(k, DOF_SAMPLE_CNT) {
		theta = drand48() * 2 * M_PI;
		Vec diff = dir_w * cos(theta) + dir_w * sin(theta);
		diff.normalize();
		diff = diff * (drand48() * DOF_SAMPLE_RADIUS);

		Vec neworig = intersec + diff;
		Ray ray(neworig, dest - neworig, 1, true);
		dof_res += sp->trace(ray);
	}
	dof_res = dof_res * (1.0 / DOF_SAMPLE_CNT);
	return dof_res;
}

Color View::render(int i, int j) const {
	Vec dest = mid + dir_h * (geo.h - 1 - i - geo.h / 2) + dir_w * (j - geo.w / 2);
	return render_antialias(dest, ANTIALIAS_SAMPLE_CNT);
}

void View::twist(int angle) {
	normalize_dir_vector();
	Vec norm = view_point - mid;
	real_t alpha = M_PI * angle / 180;
	dir_w = dir_w * cos(alpha) + dir_h * sin(alpha);
	m_assert(fabs(dir_w.sqr() - 1) < EPS);
	dir_h = norm.cross(dir_w);
	resume_dir_vector();
}

void View::zoom(real_t ratio) {
	ratio = 1.0 / ratio;
	size *= ratio;
	Vec dir = (view_point - mid);
	view_point = mid + dir * ratio;
	resume_dir_vector();
}

void View::orbit(int angle) {
	normalize_dir_vector();
	real_t alpha = M_PI * angle / 180;

	Vec norm = (view_point - mid).get_normalized();
	norm = norm * cos(alpha) + dir_w * sin(alpha);
	view_point = mid + norm * (view_point - mid).mod();
	m_assert(fabs(dir_w.sqr()) - 1 < EPS);
	dir_w = -norm.cross(dir_h);
	resume_dir_vector();
}

void View::shift(real_t dist, bool horiz) {
	Vec diff = (horiz ? dir_w : dir_h) * dist;
	view_point = view_point + diff;
	mid = mid + diff;
}

void View::move_screen(real_t dist) {
	real_t old_dist_to_screen = (mid - view_point).mod();
	mid = view_point + (mid - view_point).get_normalized() * (old_dist_to_screen + dist);
	size *= (old_dist_to_screen + dist) / old_dist_to_screen;
	resume_dir_vector();
}

void View::rotate(int angle) {
	normalize_dir_vector();
	real_t alpha = M_PI * angle / 180;
	Vec norm = (mid - view_point).get_normalized();
	norm = norm * cos(alpha) - dir_w * sin(alpha);
	mid = view_point + norm * (mid - view_point).mod();
	m_assert(fabs(dir_w.sqr()) - 1 < EPS);
	dir_w = norm.cross(dir_h);
	resume_dir_vector();
}
