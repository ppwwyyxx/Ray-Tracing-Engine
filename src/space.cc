// File: space.cc
// Date: Mon Jun 17 16:40:45 2013 +0800
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

//	KDTree tree(objs, AABB::get_inf());
}

Color Space::trace(const Ray& ray, real_t dist, int depth) {
	static int now_max_depth = 0;
	update_max(now_max_depth, depth);
	if (depth > max_depth)
		return Color::BLACK;

	m_assert(fabs(ray.dir.sqr() - 1) < EPS);
	// XXX delete later

	auto first_trace = find_first(ray);
	if (!first_trace) { return Color::BLACK; }

	// reach the first object
	real_t inter_dist = first_trace->intersection_dist();
	Vec norm = first_trace->normal(),
		inter_point = first_trace->intersection_point();
	auto surf = first_trace->get_property();
	real_t density = first_trace->get_forward_density();

	m_assert((fabs(norm.sqr() - 1) < EPS));

	if (ray.debug)
		print_debug("debug ray: arrive point (%lf, %lf, %lf) \n", inter_point.x, inter_point.y, inter_point.z);

	// phong model
	// http://en.wikipedia.org/wiki/Phong_reflection_model
	// ambient
	Color ret = ambient * (surf->diffuse + Color::WHITE) * surf->ambient * 0.5;

	for (const shared_ptr<Light> &i : lights) {
		Vec lm = (i->src - inter_point).get_normalized();
		real_t lmn = lm.dot(norm);
		real_t dist_to_light = (i->src - inter_point).mod();

		// shadow if not visible to this light
		// go foward a little
		if (find_any(Ray(inter_point + lm * EPS, lm), dist_to_light)) {
			// if (lmn > 0) ret += surf->diffuse * ambient * REFL_DECAY;
			continue;
		}

		real_t damping = pow(M_E, -dist_to_light * AIR_BEER_DENSITY);

		// diffuse
		if (lmn > 0)
			ret += surf->diffuse * i->color * (i->intensity * damping * lmn);		// add beer

		// specular
		real_t rmv = -norm.reflection(lm).dot(ray.dir);
		if (rmv > 0)
			ret += surf->specular * pow(rmv, surf->shininess) * i->color * i->intensity * damping;
	}

	// Beer-Lambert's Law
	dist += inter_dist;
	ret *= pow(M_E, -dist * AIR_BEER_DENSITY);

	// reflected ray : go back a little, same density
	m_assert(fabs(ray.dir.sqr() - 1) < EPS);
	if (surf->ambient < 1 - EPS) {		// do reflection if ambient is small
		print_debug("Here2\n");
		Ray new_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
		m_assert(fabs((-norm.reflection(ray.dir)).sqr() - 1) < EPS);

		new_ray.debug = ray.debug;
		real_t lmn = (new_ray.dir.dot(norm));
		Color refl = trace(new_ray, dist, depth + 1);
		ret += refl * surf->diffuse * (lmn * REFL_DECAY * (1 - surf->ambient) * surf->shininess);
	}


	// transmission
	if (surf->transparency > EPS) {
		print_debug("Here\n");
		Vec tr_dir = norm.transmission(ray.dir, density / ray.density);
		if (isnormal(tr_dir.x)) { // have transmission
			// transmission ray : go forward a little
			Ray new_ray(inter_point + ray.dir * EPS, tr_dir, density);
			new_ray.debug = ray.debug;
			Color transm = trace(new_ray, dist, depth + 1);
			ret += transm * surf->transparency;
		}
	}

	ret.normalize();
	return ret;
}

shared_ptr<Trace> Space::find_first(const Ray& ray) const {
	real_t min = numeric_limits<real_t>::max();
	shared_ptr<Trace> ret;

	for (auto & obj : objs) {
		auto tmp = obj->get_trace(ray);
		if (tmp) {
			real_t d = tmp->intersection_dist();
			if (update_min(min, d))
				ret = tmp;
		}
	}
	return ret;
}

bool Space::find_any(const Ray& ray, real_t dist) const {
	for (auto & obj : objs) {
		shared_ptr<Trace> tmp = obj->get_trace(ray);
		if (tmp && (tmp->intersection_dist() < dist))
			return true;
	}
	return false;
}


