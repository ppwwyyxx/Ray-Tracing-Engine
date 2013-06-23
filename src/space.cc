// File: space.cc
// Date: Sun Jun 23 10:53:07 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <limits>
#include <algorithm>
#include "space.hh"
#include "lib/debugutils.hh"

using namespace std;

Color Space::blend(const Color& amb, const Color& phong, const Color& refl, const Color& transm) {
	Color ret = (amb + phong + refl + transm * 2) * 0.25;
	ret.normalize();
	return ret;
}

void Space::add_light(const Light& light) {
	if (use_soft_shadow) {
		real_t delta_theta = 2 * M_PI / SOFT_SHADOW_LIGHT;
		real_t theta = 0;
		REP(k, SOFT_SHADOW_LIGHT) {
			Vec diff = Vec(cos(theta), sin(theta), 0) * SOFT_SHADOW_RADIUS;
			theta += delta_theta;

			Light newlight(light.get_src() + diff, light.color, light.intensity / SOFT_SHADOW_LIGHT);
			lights.push_back(make_shared<Light>(newlight));
		}

	} else
		lights.push_back(make_shared<Light>(light));
}

void Space::add_obj(const rdptr& objptr) {
	if (!objptr->infinity) {
		auto k = objptr->get_aabb();
		bound_min.update_min(k.min - Vec::eps());
		bound_max.update_max(k.max + Vec::eps());
	}
	objs.push_back(objptr);
}

void Space::finish() {		// called from View::View()
	ambient = Color::BLACK;
	m_assert(lights.size());
	for (const auto &i : lights)
		ambient += i->color * i->intensity;
	ambient *= AMBIENT_FACTOR;

	if (use_tree) {
		// the final objs contains a kdtree and all the infinite obj
		vector<rdptr> infinite_obj;
		for (auto &k : objs) if (k->infinity) infinite_obj.push_back(k);
		objs.erase(remove_if(objs.begin(), objs.end(),
					[](const rdptr& p) {
					return p->infinity;
					}), objs.end());

		if (objs.size())
			infinite_obj.push_back(rdptr(new KDTree(objs, AABB(bound_min, bound_max))));
		objs = infinite_obj;
	}
}

Color Space::trace(const Ray& ray, real_t dist, int depth) const {
	if (depth > max_depth) return Color::BLACK;
	m_assert(fabs(ray.dir.sqr() - 1) < EPS);

	auto first_trace = find_first(ray);
	if (!first_trace) { return Color::BLACK; }

	// reach the first object
	real_t inter_dist = first_trace->intersection_dist();
	Vec norm = first_trace->normal(),
		inter_point = first_trace->intersection_point();
	auto surf = first_trace->get_property();
	real_t forward_density = first_trace->get_forward_density();

	m_assert((fabs(norm.sqr() - 1) < EPS));

	if (ray.debug)
		print_debug("debug ray: arrive point (%lf, %lf, %lf) \n", inter_point.x, inter_point.y, inter_point.z);

	// phong model
	// http://en.wikipedia.org/wiki/Phong_reflection_model
	// ambient
	Color now_amb = ambient * (surf->diffuse + Color::WHITE) * surf->ambient * 0.5;

	Color now_col = Color::BLACK;
	for (auto &i : lights) {
		Vec lm = (i->get_src() - inter_point).get_normalized();
		real_t lmn = lm.dot(norm);
		real_t dist_to_light = (i->get_src() - inter_point).mod();

		// shadow if not visible to this light
		// go foward a little
		if (find_any(Ray(inter_point + lm * (2 * EPS), lm), dist_to_light)) {
			// if (lmn > 0) ret += surf->diffuse * ambient * REFL_DECAY;
			continue;
		}

		real_t damping = pow(M_E, -dist_to_light * AIR_BEER_DENSITY);

		// diffuse
		if (lmn > 0)
			now_col += surf->diffuse * i->color * (i->intensity * damping * lmn);		// add beer

		// specular
		real_t rmv = -norm.reflection(lm).dot(ray.dir);
		if (rmv > 0)
			now_col += i->color * (i->intensity * damping * surf->specular * pow(rmv, surf->shininess));
	}

	// Beer-Lambert's Law
	dist += inter_dist;
	now_col = now_col * pow(M_E, -dist * AIR_BEER_DENSITY);

	m_assert(fabs(ray.dir.sqr() - 1) < EPS);
	Color now_refl = Color::BLACK;

	// do reflection if specular > 0
	// inner surface don't reflect
	if (surf->specular > 0 && !(first_trace->contain())) {
		// reflected ray : go back a little, same density
		Ray new_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
		m_assert(fabs((-norm.reflection(ray.dir)).sqr() - 1) < EPS);


		real_t lmn = fabs(new_ray.dir.dot(norm));

		new_ray.debug = ray.debug;
		Color refl = trace(new_ray, dist, depth + 1);
		now_refl = (refl + refl * surf->diffuse * REFL_DIFFUSE_FACTOR) *
			(REFL_DECAY * surf->specular * surf->shininess * lmn);
	}


	// transmission
	Color now_transm = Color::BLACK;
	if (surf->transparency > EPS) {
		Vec tr_dir = norm.transmission(ray.dir, ray.density / forward_density);
		if (isnormal(tr_dir.x)) { // have transmission
			// transmission ray : go forward a little
			Ray new_ray(inter_point + ray.dir * EPS, tr_dir, forward_density);
			new_ray.debug = ray.debug;
			Color transm = trace(new_ray, dist, depth + 1);
			now_transm = (transm + transm * surf->diffuse * TRANSM_DIFFUSE_FACTOR) *
				surf->transparency;
		}
	}

	return Space::blend(now_amb, now_col, now_refl, now_transm);
}

Color Space::global_trace(const Ray& ray, int depth) const {
	if (depth > max_depth) return Color::BLACK;		// TODO add new depth thres
	m_assert(fabs(ray.dir.sqr() - 1) < EPS);

	auto first_trace = find_first(ray, true);
	if (!first_trace) { return Color::BLACK; }

	// reach the first object, could be a light
	real_t inter_dist = first_trace->intersection_dist();
	Vec norm = first_trace->normal(),			// already oriented
		inter_point = first_trace->intersection_point();
	auto surf = first_trace->get_property();
	Color diffu = surf->diffuse;
	real_t forward_density = first_trace->get_forward_density();

	m_assert((fabs(norm.sqr() - 1) < EPS));

	if (ray.debug) print_debug("debug ray: arrive point (%lf, %lf, %lf) \n", inter_point.x, inter_point.y, inter_point.z);

	real_t max_color_comp = diffu.get_max();
	if (depth > 5 || fabs(max_color_comp) < EPS) {		// for light, max_color_comp = 0
		if (drand48() < max_color_comp)		// Russian Roulette
			diffu = diffu * (1.0 / max_color_comp);
		else {
			return surf->emission;
		}
	}

	// diffuse
	real_t r1 = 2 * M_PI * drand48(),
		   r2 = drand48(),
		   r2s = sqrt(r2);

	Vec u = ((fabs(norm.x) > 0.1 ? Vec(0, 1, 0) : Vec(1, 0, 0)).cross(norm)).get_normalized(),
		v = norm.cross(u);
	// generate random reflected ray by sampling unit hemisphere
	Vec d = ((u * cos(r1)) * r2s + v * sin(r1) * r2s + norm * (sqrt(1 - r2))).get_normalized();
	Color now_diffuse = diffu * global_trace(Ray(inter_point - ray.dir * EPS, d), depth + 1);

	// reflection
	Color now_refl = Color::BLACK;
	if (surf->specular > 0 && !(first_trace->contain())) {
		// reflected ray : go back a little, same density
		Ray new_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
		m_assert(fabs((-norm.reflection(ray.dir)).sqr() - 1) < EPS);

//		real_t lmn = fabs(new_ray.dir.dot(norm));
		new_ray.debug = ray.debug;
		Color refl = global_trace(new_ray, depth + 1);
		now_refl = 	surf->diffuse * refl * surf->specular;
	}

	 // transmission
	 /*
	  *Color now_transm = Color::BLACK;
	  *if (surf->transparency > EPS) {
	  *    Vec tr_dir = norm.transmission(ray.dir, forward_density / ray.density);
	  *    if (isnormal(tr_dir.x)) {  //have transmission
	  *         //transmission ray : go forward a little
	  *        Ray new_ray(inter_point + ray.dir * EPS, tr_dir, forward_density);
	  *        new_ray.debug = ray.debug;
	  *        Color transm = trace(new_ray, depth + 1);
	  *        now_transm = (transm + transm * surf->diffuse * TRANSM_DIFFUSE_FACTOR) *
	  *            surf->transparency;
	  *    } else {	// total reflection
	  *        Ray new_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
	  *        new_ray.debug = ray.debug;
	  *        Color refl = global_trace(new_ray, depth + 1);
	  *        now_transm = surf->diffuse * refl * surf->specular;
	  *    }
	  *}
	  */

	 // TODO explicit lighting:

	 return surf->emission + now_diffuse + now_refl;

}

shared_ptr<Trace> Space::find_first(const Ray& ray, bool include_light) const {
	real_t min = numeric_limits<real_t>::max();
	shared_ptr<Trace> ret;

	for (auto & obj : objs) {
		auto tmp = obj->get_trace(ray, min == numeric_limits<real_t>::max() ? -1 : min);
		if (tmp) {
			real_t d = tmp->intersection_dist();
			if (update_min(min, d)) ret = tmp;
		}
	}

	if (include_light) {		// also look for light
		for (auto &l : lights) {
			auto tmp = l->get_trace(ray, min == numeric_limits<real_t>::max() ? -1 : min);
			if (tmp) {
				real_t d = tmp->intersection_dist();
				if (update_min(min, d)) ret = tmp;
			}
		}
	}
	return move(ret);
}

bool Space::find_any(const Ray& ray, real_t max_dist) const {
	for (auto & obj : objs) {
		auto tmp = obj->get_trace(ray, max_dist);
		if (tmp) return true;
	}
	return false;
}


