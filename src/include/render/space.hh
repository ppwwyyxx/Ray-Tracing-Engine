// File: space.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <vector>
#include <list>
#include <memory>

#include "renderable/renderable.hh"
#include "renderable/light.hh"
#include "kdtree.hh"
#include "const.hh"
using std::vector;
using std::list;
using std::shared_ptr;
using std::make_shared;

class Space {
	protected:
		vector<shared_ptr<Light>>  lights;
		list<rdptr> objs;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		shared_ptr<Trace> find_first(const Ray& ray, bool include_light = false) const;

		bool find_any(const Ray& ray, real_t dist) const; // is there any obj on the ray within the dist?

	public:
		// config
		bool use_tree = true;
		bool use_soft_shadow = false;

		Space(){ }

		virtual ~Space(){}

		void add_light(const Light& light);

		void add_obj(const rdptr& objptr);

		// useful, keep lights
		virtual void clear()
		{ objs.clear(); bound_min = Vec::max(), bound_max = -Vec::max(); }

		virtual void finish();

		void build_tree();

		virtual Color trace(const Ray& ray) const = 0;
};

