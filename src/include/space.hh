// File: space.hh
// Date: Sat Jun 22 23:15:50 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <vector>
#include <memory>

#include "renderable/renderable.hh"
#include "kdtree.hh"
#include "const.hh"
using std::vector;
using std::shared_ptr;
using std::make_shared;

class Space {
	private:
		vector<shared_ptr<Light>>  lights;
		vector<rdptr> objs;

		Color ambient;		// ambient in this space

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		shared_ptr<Trace> find_first(const Ray& ray) const;

		bool find_any(const Ray& ray, real_t dist) const; // is there any obj on the ray within the dist?

		static Color blend(const Color& amb,
				const Color& phong, const Color& refl, const Color& transm);

	public:
		// config
		bool use_tree = true;
		bool use_soft_shadow = false;
		int max_depth = MAX_RECURSIVE_DEPTH;
		real_t weight_threshold = DEFAULT_TRACING_WEIGHT_THRESHOLD;

		Space(){ }

		~Space(){}

		void add_light(const Light& light);

		void add_obj(const rdptr& objptr);

		// useful, keep lights
		void clear()
		{ objs.clear(); bound_min = Vec::max(), bound_max = -Vec::max(); }

		void finish();

		Color trace(const Ray& ray, real_t dist = 0, int depth = 0) const;
};

