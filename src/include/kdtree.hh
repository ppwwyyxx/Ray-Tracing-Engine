// File: kdtree.hh
// Date: Wed Sep 11 20:28:08 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
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

		KDTree(const vector<rdptr>& objs, const AABB& space);

		~KDTree();

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist = -1) const override;

		AABB get_aabb() const override
		{ return AABB(bound_min, bound_max); }

	private:
		Node* build(const vector<RenderWrapper>& objs, const AABB& box, int depth);

		AAPlane cut(const vector<RenderWrapper>& objs, int depth) const;

};
