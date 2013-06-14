// File: kdtree.cc
// Date: Fri Jun 14 23:16:10 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <algorithm>
#include "lib/kdtree.hh"
using namespace std;

class KDTree::Node {
	public:
		AABB box;
		Node* child[2];

		Node(const AABB& _box, Node* p1 = nullptr, Node* p2 = nullptr) :
			box(_box), child{p1, p2} { }

		bool leaf() const
		{ return child[0] == nullptr && child[1] == nullptr; }

		void set_objs(const vector<shared_ptr<RenderAble>>& _objs)
		{ objs = _objs; }

		void add_obj(shared_ptr<RenderAble> obj)
		{ objs.push_back(obj); }


	private:
		vector<shared_ptr<RenderAble>> objs;

};

KDTree::KDTree(const vector<shared_ptr<RenderAble>>& objs, const AABB& space) {
	vector<RenderWrapper> objlist;
	for (auto & obj : objs)
		objlist.push_back(RenderWrapper(obj, obj->get_aabb()));
	root = build(objlist, space, 0);
}

AAPlane KDTree::cut(const vector<RenderWrapper>& objs, const AABB& box, int depth) const {
	AAPlane ret;
	ret.axis = static_cast<AXIS>(depth % 3);

	vector<real_t> min_list;
	for (auto &obj : objs)
		min_list.push_back(obj.box.min[ret.axis]);
	nth_element(min_list.begin(), min_list.begin() + min_list.size() / 2, min_list.end());
	// partial sort
	ret.pos = min_list[min_list.size() / 2] + 2 * EPS;		// SEE what happen
	return ret;
}


KDTree::Node* KDTree::build(const vector<RenderWrapper>& objs,
		const AABB& box, int depth) {

	AAPlane pl = cut(objs, box, depth);
	Node* ret = new Node(box);

	for (auto & obj : objs)
		ret->add_obj(obj.obj);

	pair<AABB, AABB> par;
	try {
		par = box.cut(pl);
	} catch (...) {
		return ret;		// pl is outside box, cannot go further
	}

	vector<RenderWrapper> objl, objr;
	for (auto & obj : objs) {
		if (par.first.intersect(obj.box)) objl.push_back(obj);
		if (par.second.intersect(obj.box)) objr.push_back(obj);
	}
	Node *lch = build(objl, par.first, depth + 1),
		 *rch = build(objr, par.second, depth + 1);
	ret->child[0] = lch, ret->child[1] = rch;
	return ret;
}

