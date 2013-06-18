// File: kdtree.hh
// Date: Tue Jun 18 11:59:01 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "geometry/geometry.hh"
#include "geometry/aabb.hh"
#include "renderable/renderable.hh"

class KDTree {
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

		KDTree(const vector<shared_ptr<RenderAble>>& objs, const AABB& space);

		shared_ptr<Trace> get_trace(const Ray& ray) const;

	private:
		Node* build(const vector<RenderWrapper>& objs, const AABB& box, int depth);

		AAPlane cut(const vector<RenderWrapper>& objs, int depth) const;

};
