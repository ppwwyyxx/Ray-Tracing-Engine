//File: MCPT.cc

//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "render/MCPT.hh"
#include <random>

Color MCPT::do_trace(const Ray& ray, int depth) const {
	if (depth > max_depth) return Color::BLACK;
	m_assert(fabs(ray.dir.sqr() - 1) < EPS);

	auto first_trace = find_first(ray, true);
	if (!first_trace) return Color::BLACK;

	// reach the first object, could be a light
	auto intersect_info = first_trace->get_intersect_info();
	Color diffu = intersect_info.surf->diffuse,
		  emission = intersect_info.surf->emission;

	m_assert((fabs(intersect_info.norm.sqr() - 1) < EPS));
	if (ray.debug)
		print_debug("debug ray: arrive point (%lf, %lf, %lf) \n", intersect_info.inter_point.x, intersect_info.inter_point.y, intersect_info.inter_point.z);

	real_t max_color_comp = diffu.get_max();
	if (depth > 5 || fabs(max_color_comp) < EPS) {		// for light, max_color_comp = 0
		if (drand48() < max_color_comp)		// Russian Roulette
			diffu = diffu * (1.0 / max_color_comp);
		else
			return emission;
	}

	// diffuse
	Color now_diffuse = diffuse(intersect_info, ray, depth);
	// reflection
	Color now_refl = reflection(intersect_info, ray, depth);
	// transmission
	Color now_transm = transmission(intersect_info, ray, depth);
	return emission + diffu * (now_diffuse + now_refl + now_transm);
}

Color MCPT::trace(const Ray& ray) const {
	Color ret = Color::NONE;
	REP(n_samp, GLOBAL_ILLU_SAMPLE_CNT) {
		ret += do_trace_adapter(ray);
	}
	ret *= (1.0 / GLOBAL_ILLU_SAMPLE_CNT);
	ret.normalize();
	return ret;
}

Color MCPT::diffuse(const IntersectInfo& info, const Ray& ray, int depth) const {
	real_t diffuse_weight = min(1 - info.surf->specular, 1 - info.surf->transparency);
	if (diffuse_weight > EPS) {
		real_t r1 = 2 * M_PI * drand48(),
			   r2 = drand48();
		// need smaller r2, sample better (cosine weighted sample)
		while (drand48() < r2) r2 = drand48();

		// norm, u, v -> ortho
		Vec u = ((fabs(info.norm.x) > 0.1 ? Vec(0, 1, 0) : Vec(1, 0, 0)).cross(info.norm)).get_normalized(),
			v = info.norm.cross(u);

		// generate random reflected ray by sampling unit hemisphere (cosine(r2) weighted)
		Vec sample_dir = ((u * cos(r1) + v * sin(r1)) * sqrt(r2) + info.norm * sqrt(1 - r2)).get_normalized();
		return do_trace(Ray(info.inter_point - ray.dir * EPS, sample_dir), depth + 1) * diffuse_weight;
	}
	return Color::BLACK;
}

Color MCPT::reflection(const IntersectInfo& info, const Ray& ray, int depth) const {
	if (info.surf->specular > EPS && !(info.contain)) {
		// reflected ray : go back a little, same density
		Ray new_ray(info.inter_point - ray.dir * EPS, -info.norm.reflection(ray.dir), ray.density);
		m_assert(fabs((-info.norm.reflection(ray.dir)).sqr() - 1) < EPS);

		new_ray.debug = ray.debug;
		Color refl = do_trace(new_ray, depth + 1);
		return refl * info.surf->specular;
	}
	return Color::BLACK;
}

Color MCPT::transmission(const IntersectInfo& info, const Ray& ray, int depth) const {
	const Vec & inter_point = info.inter_point;
	const Vec & norm = info.norm;
	if (info.surf->transparency > EPS) {
		Ray refl_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
		refl_ray.debug = ray.debug;

		Vec tr_dir = norm.transmission(ray.dir, ray.density / info.forward_density);
		if (isfinite(tr_dir.x)) {  //have transmission
			//transmission ray : go forward a little
			Ray new_ray(inter_point + ray.dir * EPS, tr_dir, info.forward_density);
			new_ray.debug = ray.debug;

			// Fresnel's Law
			real_t F0 = sqr(0.5) / sqr(ray.density + info.forward_density);
			real_t theta = info.contain ? tr_dir.dot(norm) : ray.dir.dot(norm);
			real_t Fr = F0 + (1 - F0) * pow(1 + theta, 5);
			real_t P = 0.25 + Fr * 0.5;
			if (drand48() < P) {
				Color refl = do_trace(refl_ray, depth + 1) * info.surf->specular;
				return refl * Fr / P;
			} else {
				Color transm = do_trace(new_ray, depth + 1);
				return transm * ((1 - Fr) / (1 - P)) * info.surf->transparency;
			}
		} else {	// total reflection
			Color refl = do_trace(refl_ray, depth + 1) * info.surf->specular;
			return refl;
		}
	}
	return Color::BLACK;
}
