//File: MCPT.cc
//Date: Sat Sep 28 00:51:27 2013 +0800
//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "render/MCPT.hh"

Color MCPT::do_trace(const Ray& ray, int depth) const {
	if (depth > max_depth) return Color::BLACK;
	m_assert(fabs(ray.dir.sqr() - 1) < EPS);

	auto first_trace = find_first(ray, true);
	if (!first_trace)
		return Color::BLACK;

	// reach the first object, could be a light
	Vec norm = first_trace->normal(),			// already oriented
		inter_point = first_trace->intersection_point();
	auto surf = first_trace->get_property();
	Color diffu = surf->diffuse;
	real_t forward_density = first_trace->get_forward_density();

	m_assert((fabs(norm.sqr() - 1) < EPS));

	if (ray.debug)
		print_debug("debug ray: arrive point (%lf, %lf, %lf) \n", inter_point.x, inter_point.y, inter_point.z);

	real_t max_color_comp = diffu.get_max();
	if (depth > 5 || fabs(max_color_comp) < EPS) {		// for light, max_color_comp = 0
		if (drand48() < max_color_comp)		// Russian Roulette
			diffu = diffu * (1.0 / max_color_comp);
		else
			return surf->emission;
	}

	// diffuse
	real_t r1 = 2 * M_PI * drand48(),
		   r2 = drand48(),
		   r2s = sqrt(r2);

	// norm, u, v -> ortho
	Vec u = ((fabs(norm.x) > 0.1 ? Vec(0, 1, 0) : Vec(1, 0, 0)).cross(norm)).get_normalized(),
		v = norm.cross(u);

	// generate random reflected ray by sampling unit hemisphere
	Vec d = ((u * cos(r1)) * r2s + v * sin(r1) * r2s + norm * (sqrt(1 - r2))).get_normalized();
	Color now_diffuse = do_trace(Ray(inter_point - ray.dir * EPS, d), depth + 1) *
		min(1 - surf->specular, 1 - surf->transparency);

	// reflection
	Color now_refl = Color::BLACK;
	if (surf->specular > 0 && !(first_trace->contain())) {
		// reflected ray : go back a little, same density
		Ray new_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
		m_assert(fabs((-norm.reflection(ray.dir)).sqr() - 1) < EPS);

		new_ray.debug = ray.debug;
		Color refl = do_trace(new_ray, depth + 1);
		now_refl = 	refl * surf->specular;
	}

	// transmission
	Color now_transm = Color::BLACK;
	if (surf->transparency > EPS) {
		Ray refl_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
		refl_ray.debug = ray.debug;

		Vec tr_dir = norm.transmission(ray.dir, ray.density / forward_density);
		if (isfinite(tr_dir.x)) {  //have transmission
			//transmission ray : go forward a little
			Ray new_ray(inter_point + ray.dir * EPS, tr_dir, forward_density);
			new_ray.debug = ray.debug;

			real_t F0 = sqr(0.5) / sqr(ray.density + forward_density);
			real_t theta = first_trace->contain() ? tr_dir.dot(norm) : ray.dir.dot(norm);
			real_t Fr = F0 + (1 - F0) * pow(1 + theta, 5);
			real_t P = 0.25 + Fr * 0.5;
			if (drand48() < P) {
				Color refl = do_trace(refl_ray, depth + 1) * surf->specular;
				now_transm = refl * Fr / P;
			} else {
				Color transm = do_trace(new_ray, depth + 1);
				now_transm = transm * ((1 - Fr) / (1 - P)) * surf->transparency;
			}
		} else {	// total reflection
			Color refl = do_trace(refl_ray, depth + 1) * surf->specular;
			now_transm = refl;
		}
	}

	return surf->emission + diffu * (now_diffuse + now_refl + now_transm);
}

Color MCPT::trace(const Ray& ray) const {
	Color ret = Color::NONE;
	REP(n_samp, GLOBAL_ILLU_SAMPLE_CNT) {
		ret += do_trace_adapter(ray);
	}
	ret *= (1.0 / GLOBAL_ILLU_SAMPLE_CNT);
	ret.normalize();

#define ppp(x) pow((x < EPS) ? EPS : x > 1 ? 1 - EPS : x, 1.0 / 2.2)
	return Color(ppp(ret.x), ppp(ret.y), ppp(ret.z));
#undef ppp
}
