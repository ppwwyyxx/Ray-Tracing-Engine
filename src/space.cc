// File: space.cc
// Date: Wed Apr 10 09:05:29 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <limits>
#include "space.hh"
using namespace std;

void Space::init() {		// called from View::View()
	ambient = Color::BLACK;
	m_assert(lights.size());
	for (const auto &i : lights)
		ambient += i->color * i->intensity;
	ambient *= AMBIENT_FACTOR;
}

Color Space::trace(const Ray& ray, real_t dist, int depth) {
	static int now_max_depth = 0;
	update_max(now_max_depth, depth);
	if (depth > max_depth)
		return Color::BLACK;
	m_assert(fabs(ray.dir.sqr() - 1) < EPS);
	// XXX delete later

	auto first_trace = find_first(ray);
	if (!first_trace)
	{ return Color::BLACK; }
	// reach the first object

	real_t inter_dist = first_trace->intersection_dist();
	Vec norm = first_trace->normal(),
		inter_point = first_trace->intersection_point();
	auto surf = first_trace->get_property();
	m_assert((fabs(norm.sqr() - 1) < EPS));

	if (ray.debug)
		cout << inter_point << endl;

	// phong model
	// http://en.wikipedia.org/wiki/Phong_reflection_model
	// ambient
	Color ret = surf->ambient * ambient;

	for (const shared_ptr<Light> &i : lights) {
		Vec lm = (i->src - inter_point).get_normalized();
		real_t lmn = lm.dot(norm);
		real_t dist_to_light = (i->src - inter_point).mod();

		// shadow if not visible to this light
		// go foward a little
		if (find_any(Ray(inter_point + lm * EPS, lm), dist_to_light)) {
			if (lmn > 0)
				ret += surf->diffuse * ambient * lmn;
			continue;
		}

		real_t damping = pow(M_E, -dist_to_light * AIR_DENSITY);

		// diffuse
		if (lmn > 0)
			ret += surf->diffuse * lmn * i->color * i->intensity * damping;		// add beer

		// specular
		real_t rmv = -norm.reflection(lm).dot(ray.dir);
		if (rmv > 0)
			ret += surf->specular * pow(rmv, surf->shininess) * i->color * i->intensity * damping;
	}

	// Beer-Lambert's Law
	dist += inter_dist;
	ret *= pow(M_E, -dist * AIR_DENSITY);

	// reflected ray : go back a little
	Ray new_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir));
	new_ray.debug = ray.debug;
	Color refl = trace(new_ray, dist, depth + 1);

	// use (specular + FACTOR * diffuse) * refl
	ret += refl * surf->specular * REFL_DECAY;
	/*
	 *ret += refl * surf->diffuse * REFL_DIFFUSE_FACTOR * REFL_DECAY;
	 */


	// transmission
	if (surf->transparency > 0) {
		new_ray = Ray(inter_point + ray.dir * EPS, ray.dir);	// transmission dir
		new_ray.debug = ray.debug;
		Color transm = trace(new_ray, dist, depth + 1);
		ret += transm * surf->transparency;
	}

	ret.normalize();
	return move(ret);
}

shared_ptr<Trace> Space::find_first(const Ray& ray) const {
	real_t min = numeric_limits<real_t>::max();
	shared_ptr<Trace> ret, tmp;
	for (const auto& obj : objs) {
		tmp = obj->get_trace(ray);
		if (tmp) {
			real_t d = tmp->intersection_dist();
			if (d < min) {
				min = d;
				ret = tmp;
			}
		}
	}
	return move(ret);
}

bool Space::find_any(const Ray& ray, real_t dist) const {
	for (const auto& obj : objs) {
		shared_ptr<Trace> tmp = obj->get_trace(ray);
		if (tmp && (tmp->intersection_dist() < dist))
			return true;
	}
	return false;
}


