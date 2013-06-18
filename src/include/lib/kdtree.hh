// File: kdtree.hh
// Date: Tue Jun 18 15:20:33 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "geometry/geometry.hh"
#include "geometry/aabb.hh"
#include "renderable/renderable.hh"

class KDTree : public RenderAble {
	public:
		class Node;

		class RenderWrapper {
			public:
				shared_ptr<RenderAble> obj;
				AABB box;
				RenderWrapper(const shared_ptr<RenderAble>& _obj, const AABB& _box):
					obj(_obj), box(_box){}

		};

		int depth = 0;
		Node* root;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		KDTree(const vector<shared_ptr<RenderAble>>& objs, const AABB& space);

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist = -1) const;

		AABB get_aabb() const { return AABB(bound_min, bound_max); }

	private:
		Node* build(const vector<RenderWrapper>& objs, const AABB& box, int depth);

		AAPlane cut(const vector<RenderWrapper>& objs, int depth) const;

};
