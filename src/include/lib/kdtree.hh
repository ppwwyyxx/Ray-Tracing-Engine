// File: kdtree.hh
// Date: Thu Jun 20 12:54:54 2013 +0800
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

		Node* root;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		KDTree(const vector<shared_ptr<RenderAble>>& objs, const AABB& space);

		~KDTree();

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist = -1) const override;

		AABB get_aabb() const override
		{ return AABB(bound_min, bound_max); }

	private:
		Node* build(const vector<RenderWrapper>& objs, const AABB& box, int depth);

		AAPlane cut(const vector<RenderWrapper>& objs, int depth) const;

};
