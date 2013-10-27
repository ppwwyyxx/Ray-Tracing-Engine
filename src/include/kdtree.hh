// File: kdtree.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <list>
#include "geometry/geometry.hh"
#include "geometry/aabb.hh"
#include "renderable/renderable.hh"

class KDTree : public Renderable {
	public:
		class Node;

		class RenderWrapper {
			public:
				rdptr obj;
				AABB box;
				RenderWrapper(const rdptr& _obj, const AABB& _box):
					obj(_obj), box(_box){}

		};

		Node* root;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		KDTree(const std::list<rdptr>& objs, const AABB& space);

		~KDTree();

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist) const override;

		AABB get_aabb() const override
		{ return AABB(bound_min, bound_max); }

	private:
		Node* build(const std::list<RenderWrapper>& objs, const AABB& box, int depth);

		AAPlane cut(const std::list<RenderWrapper>& objs, int depth) const;

};
