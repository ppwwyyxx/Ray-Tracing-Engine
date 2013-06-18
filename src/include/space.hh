// File: space.hh
// Date: Tue Jun 18 14:28:05 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <vector>
#include <memory>

#include "renderable/renderable.hh"
#include "lib/kdtree.hh"
#include "common.hh"
using std::vector;
using std::shared_ptr;
using std::make_shared;

class Space {
	private:
		vector<shared_ptr<Light>>  lights;
		vector<rdptr> objs;

		int max_depth = MAX_RECURSIVE_DEPTH;
		int now_ray_count;
		Color ambient;		// ambient in this space

		shared_ptr<KDTree> tree;
		Vec bound_min = Vec::max(), bound_max = -Vec::max();

	public:
		Space(){ }

		~Space(){}

		void add_light(const Light& light)
		{ lights.push_back(make_shared<Light>(light)); }

		void add_obj(const rdptr& objptr) {
			objs.push_back(objptr);
			auto k = objptr->get_aabb();
			bound_min.update_min(k.min);
			bound_max.update_max(k.max);
		}

		void add_obj(RenderAble* objptr)
		{ add_obj(rdptr(objptr)); }

		// useful, keep lights
		void clean_obj()
		{ objs.clear(); }

		void init();

		Color trace(const Ray& ray, real_t dist = 0, int depth = 0);

		real_t weight_threshold = DEFAULT_TRACING_WEIGHT_THRESHOLD;

	protected:
		shared_ptr<Trace> find_first(const Ray& ray) const;

		bool find_any(const Ray& ray, real_t dist) const; // is there any obj on the ray within the dist?
};

