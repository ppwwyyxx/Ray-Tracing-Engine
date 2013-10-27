//File: MCPT_EL.cc

//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "render/MCPT_EL.hh"
using namespace std;

Color MCPT_EL::do_trace(const Ray& ray, int depth, int use_emission) const {
	if (depth > max_depth) return Color::BLACK;
	m_assert(fabs(ray.dir.sqr() - 1) < EPS);

	auto first_trace = find_first(ray, true);
	if (!first_trace) return Color::BLACK;

	// reach the first object, could be a light
	Vec norm = first_trace->normal(),			// already oriented
		inter_point = first_trace->intersection_point();
	auto surf = first_trace->get_property();
	Color diffu = surf->diffuse;
	real_t forward_density = first_trace->get_forward_density();

	m_assert((fabs(norm.sqr() - 1) < EPS));
	m_assert(norm.dot(ray.dir) <= 0);

	if (ray.debug) {
		print_debug("debug ray: arrive point (%lf, %lf, %lf) \n", inter_point.x, inter_point.y, inter_point.z);
	}

	real_t max_color_comp = diffu.get_max();
	if (depth > 5 || fabs(max_color_comp) < EPS) {		// for light, max_color_comp = 0
		if (drand48() < max_color_comp)		// Russian Roulette
			diffu = diffu * (1.0 / max_color_comp);
		else
			return surf->emission * use_emission;
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
	real_t diffuse_weight = min(1 - surf->specular, 1 - surf->transparency);	// probably negative ?
	Color now_diffuse = do_trace(Ray(inter_point - ray.dir * EPS, d), depth + 1) * diffuse_weight;

	// explicit lighting
	real_t lighting = 0;
	for (auto & light : lights) {
		Vec l_src = light->get_src();
		real_t l_size = light->get_size();

		// sw, su, sv -> ortho
		Vec sw = l_src - inter_point,
			su = (fabs(sw.x) > 0.1 ? Vec(0, 1, 0) : Vec(1, 0, 0)).get_normalized(),
			sv = sw.cross(su);
		real_t cos_a_max = sqrt(1 - ::sqr(l_size) / sw.sqr());
		if (!isnormal(cos_a_max)) cos_a_max = 0;		// l_size^2 might be slightly smaller than sw.sqr()
		real_t eps1 = drand48(),
			   eps2 = drand48();
		real_t cos_a = 1 - eps1 + eps1 * cos_a_max,
			   sin_a = sqrt(1 - ::sqr(cos_a)),
			   phi = 2 * M_PI * eps2;
		Vec dir = su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a;
		if (dir.dot(norm) < 0) continue;
		dir.normalize();
		if (check_shadow_ray(Ray(inter_point + dir * EPS, dir), light)) {
			real_t omega = 2 * M_PI * (1 - cos_a_max);
			lighting += light->intensity * dir.dot(norm) * omega * M_1_PI;
		}
	}
	lighting *= diffuse_weight / 6;
	m_assert(diffuse_weight >= 0);
	m_assert(lighting >= 0);

	// reflection
	Color now_refl = Color::BLACK;
	if (surf->specular > 0 && !(first_trace->contain())) {
		// reflected ray : go back a little, same density
		Ray new_ray(inter_point - ray.dir * EPS, -norm.reflection(ray.dir), ray.density);
		m_assert(fabs((-norm.reflection(ray.dir)).sqr() - 1) < EPS);

		new_ray.debug = ray.debug;
		Color refl = do_trace(new_ray, depth + 1, 0);
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
				Color refl = do_trace(refl_ray, depth + 1, 0) * surf->specular;
				now_transm = refl * Fr / P;
			} else {
				Color transm = do_trace(new_ray, depth + 1, 0);
				now_transm = transm * ((1 - Fr) / (1 - P)) * surf->transparency;
			}
		} else {	// total reflection
			Color refl = do_trace(refl_ray, depth + 1) * surf->specular;
			now_transm = refl;
		}
	}

	return surf->emission * use_emission + diffu * (now_diffuse + now_refl + now_transm) + diffu * lighting;
}

// check if the ray will directly see the light
bool MCPT_EL::check_shadow_ray(const Ray& ray, const shared_ptr<Light>& light) const {
	real_t min = numeric_limits<real_t>::max();

	for (auto & obj : objs) {
		auto tmp = obj->get_trace(ray, min == numeric_limits<real_t>::max() ? -1 : min);
		if (tmp) {
			real_t d = tmp->intersection_dist();
			update_min(min, d);
		}
	}
	bool found = false;
	for (auto &l : lights) {
		auto tmp = l->get_trace(ray, min == numeric_limits<real_t>::max() ? -1 : min);
		if (tmp) {
			real_t d = tmp->intersection_dist();
			if (update_min(min, d)) {
				if (found) return false;
			} else {
				if (l == light) return false;
			}
			if (l == light) found = true;
		}
	}
	return true;
}
