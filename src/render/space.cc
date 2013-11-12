// File: space.cc

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <limits>
#include <algorithm>
#include "render/space.hh"
#include "lib/debugutils.hh"

using namespace std;

void Space::add_light(const Light& light) {
	if (use_soft_shadow) {
		real_t delta_theta = 2 * M_PI / SOFT_SHADOW_LIGHT;
		real_t theta = 0;
		REP(k, SOFT_SHADOW_LIGHT) {
			Vec diff = Vec(cos(theta), sin(theta), 0) * SOFT_SHADOW_RADIUS;
			theta += delta_theta;
			lights.push_back(make_shared<Light>(
						light.get_src() + diff, light.color, light.intensity / SOFT_SHADOW_LIGHT));
		}
	} else
		lights.push_back(make_shared<Light>(light));
}

void Space::add_obj(const rdptr& objptr) {
	if (!objptr->infinity()) {
		auto k = objptr->get_aabb();
		bound_min.update_min(k.min - Vec::eps());
		bound_max.update_max(k.max + Vec::eps());
	}
	objs.push_back(objptr);
}

void Space::finish() {		// called from View::View()
	if (use_tree)
		build_tree();
}

void Space::build_tree() {
	// the final objs contains a kdtree and all the infinite obj
	list<rdptr> infinite_obj;
	copy_if(objs.begin(), objs.end(), back_inserter(infinite_obj),
			[](const rdptr& p) {
				return p->infinity();
			});

	objs.remove_if(
			[](const rdptr& p) {
				return p->infinity();
			});

	if (objs.size())
		infinite_obj.emplace_back(make_shared<KDTree>(objs, AABB(bound_min, bound_max)));
	objs = move(infinite_obj);
}

shared_ptr<Trace> Space::find_first(const Ray& ray, bool include_light) const {
	real_t min = numeric_limits<real_t>::max();
	shared_ptr<Trace> ret;
	for (auto & obj : objs) {
		auto tmp = obj->get_trace(ray, min == numeric_limits<real_t>::max() ? -1 : min);
		if (tmp) {
			real_t d = tmp->intersection_dist();
			if (update_min(min, d)) ret = move(tmp);
		}
	}
	if (include_light) 		// also look for light
		for (auto &l : lights) {
			auto tmp = l->get_trace(ray, min == numeric_limits<real_t>::max() ? -1 : min);
			if (tmp) {
				real_t d = tmp->intersection_dist();
				if (update_min(min, d)) ret = tmp;
			}
		}
	return ret;
}

bool Space::find_any(const Ray& ray, real_t max_dist) const {
	for (auto & obj : objs) {
		auto tmp = obj->get_trace(ray, max_dist);
		if (tmp) return true;
	}
	return false;
}
