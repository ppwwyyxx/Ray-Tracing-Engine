//File: phong.cc
//Date: Mon Sep 30 01:26:17 2013 +0800
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
	if (!first_trace) return Color::BLACK;

	auto intersect_info = first_trace->get_intersect_info();

	m_assert((fabs(intersect_info.norm.sqr() - 1) < EPS));
	if (ray.debug)
		print_debug("debug ray: arrive point (%lf, %lf, %lf) \n", intersect_info.inter_point.x, intersect_info.inter_point.y, intersect_info.inter_point.z);

	// phong model
	// http://en.wikipedia.org/wiki/Phong_reflection_model
	// ambient
	Color now_amb = ambient * (intersect_info.surf->diffuse + Color::WHITE) * intersect_info.surf->ambient * 0.5;

	// diffuse + specular
	Color now_col = phong_local(intersect_info, ray, depth);

	// Beer-Lambert's Law
	dist += intersect_info.inter_dist;
	now_col = now_col * pow(M_E, -dist * AIR_BEER_DENSITY);

	m_assert(fabs(ray.dir.sqr() - 1) < EPS);

	// reflection
	Color now_refl = reflection(intersect_info, ray, dist, depth);
	// transmission
	Color now_transm = transmission(intersect_info, ray, dist, depth);
	return Phong::blend(now_amb, now_col, now_refl, now_transm);
}

Color Phong::transmission(const IntersectInfo& info, const Ray& ray, real_t dist, int depth) const {
	if (info.surf->transparency > EPS) {
		Vec tr_dir = info.norm.transmission(ray.dir, ray.density / info.forward_density);
		if (isnormal(tr_dir.x)) { // have transmission
			// transmission ray : go forward a little
			Ray new_ray(info.inter_point + ray.dir * EPS, tr_dir, info.forward_density);
			new_ray.debug = ray.debug;
			Color transm = do_trace(new_ray, dist, depth + 1);
			return (transm + transm * info.surf->diffuse * TRANSM_DIFFUSE_FACTOR) *
				info.surf->transparency;
		}
	}
	return Color::BLACK;
}

Color Phong::reflection(const IntersectInfo& info, const Ray& ray, real_t dist, int depth) const {
	// do reflection if specular > 0
	// inner surface don't reflect
	if (info.surf->specular > 0 && !(info.contain)) {
		// reflected ray : go back a little, same density
		Ray new_ray(info.inter_point - ray.dir * EPS, -info.norm.reflection(ray.dir), ray.density);
		//m_assert(fabs((-info.norm.reflection(ray.dir)).sqr() - 1) < EPS);

		real_t lmn = fabs(new_ray.dir.dot(info.norm));

		new_ray.debug = ray.debug;
		Color refl = do_trace(new_ray, dist, depth + 1);
		return (refl + refl * info.surf->diffuse * REFL_DIFFUSE_FACTOR) *
			(REFL_DECAY * info.surf->specular * info.surf->shininess * lmn);
	}
	return Color::BLACK;
}

Color Phong::phong_local(const IntersectInfo& info, const Ray& ray, int depth) const {
	Color ret(Color::BLACK);
	for (auto &i : lights) {
		Vec lm = (i->get_src() - info.inter_point).get_normalized();
		real_t lmn = lm.dot(info.norm);
		real_t dist_to_light = (i->get_src() - info.inter_point).mod();

		// shadow if not visible to this light
		// go foward a little
		if (find_any(Ray(info.inter_point + lm * (2 * EPS), lm), dist_to_light)) { continue; }

		real_t damping = pow(M_E, -dist_to_light * AIR_BEER_DENSITY);

		//	diffuse
		real_t diffuse_weight = min(1 - info.surf->specular, 1 - info.surf->transparency);
		if (lmn > 0 && diffuse_weight > EPS)
			ret += info.surf->diffuse * i->color * (i->intensity * damping * lmn);		// add beer

		// specular
		real_t rmv = -info.norm.reflection(lm).dot(ray.dir);
		if (rmv > 0)
			ret += i->color * (i->intensity * damping * info.surf->specular * pow(rmv, info.surf->shininess));
	}
	return ret;
}
