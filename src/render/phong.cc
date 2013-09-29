//File: phong.cc
//Date: Mon Sep 30 00:35:55 2013 +0800
//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "render/phong.hh"

Color Phong::blend(const Color& amb, const Color& phong, const Color& refl, const Color& transm) {
	Color ret = (amb + phong + refl + transm) / 4;
	ret.normalize();
	return ret;
}

void Phong::finish() {
	// make ambient
	ambient = Color::BLACK;
	m_assert(lights.size());
	for (const auto &i : lights)
		ambient += i->color * i->intensity;
	ambient *= AMBIENT_FACTOR;

	Space::finish();
}

Color Phong::do_trace(const Ray& ray, real_t dist, int depth) const {
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

	if (fabs(norm.sqr() - 1) >= EPS)
		cout << norm << "  " << norm.sqr() << endl;
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
		if (find_any(Ray(inter_point + lm * (2 * EPS), lm), dist_to_light)) { continue; }

		real_t damping = pow(M_E, -dist_to_light * AIR_BEER_DENSITY);

		//	diffuse
		real_t diffuse_weight = min(1 - surf->specular, 1 - surf->transparency);
		if (lmn > 0 && diffuse_weight > EPS)
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
		Color refl = do_trace(new_ray, dist, depth + 1);
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
			Color transm = do_trace(new_ray, dist, depth + 1);
			now_transm = (transm + transm * surf->diffuse * TRANSM_DIFFUSE_FACTOR) *
				surf->transparency;
		}
	}

	return Phong::blend(now_amb, now_col, now_refl, now_transm);
}
