// File: kdtree.cc
// Date: Mon Jun 17 14:26:33 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include <algorithm>
#include "lib/kdtree.hh"
#include "lib/debugutils.hh"
using namespace std;

class KDTree::Node {
	public:
		AABB box;
		Node* child[2];
		AAPlane pl;

		Node(const AABB& _box, Node* p1 = nullptr, Node* p2 = nullptr) :
			box(_box), child{p1, p2} { }

		~Node() { delete child[0]; delete child[1]; }

		bool leaf() const
		{ return child[0] == nullptr && child[1] == nullptr; }

		void set_objs(const vector<rdptr>& _objs)
		{ objs = _objs; }

		void add_obj(rdptr obj)
		{ objs.push_back(obj); }

		shared_ptr<Trace> get_trace(const Ray& ray, real_t inter_dist) const {
			/*
			 *print_debug("in get_trace\n");
			 */
			// call when know to intersect
			if (leaf()) {
				// find first obj
				real_t min = numeric_limits<real_t>::max();
				shared_ptr<Trace> ret;

				for (auto & obj : objs) {
					auto tmp = obj->get_trace(ray);
					if (tmp) {
						real_t d = tmp->intersection_dist();
						if (update_min(min, d)) ret = tmp;
					}
				}
				return ret;
			}

			real_t mind = -1, mind2 = -1;
			bool inside;
			real_t pivot = ray.get_dist(inter_dist)[pl.axis];
			int first_met = (int)(pivot > pl.pos);

			Node* ch0 = child[first_met], *ch1 = child[1 - first_met];
			if (!ch0 || !ch0->box.intersect(ray, mind, inside)) ch0 = nullptr;
			if (ch0) {
				auto ret = ch0->get_trace(ray, mind);
				if (ret) {
					Vec inter_p = ret->intersection_point();
					if (ch0->box.contain(inter_p)) return ret;
				}
			}

			if (!ch1 || !ch1->box.intersect(ray, mind2, inside)) ch1 = nullptr;
			m_assert(!(ch0 && ch1 && mind == -1 && mind2 == -1));
			m_assert(!(ch0 && ch1 && mind > mind2));
			if (ch1) {
				auto ret = ch1->get_trace(ray, mind2);
				if (ret) {
					Vec inter_p = ret->intersection_point();
					if (ch1->box.contain(inter_p)) return ret;
				}
			}
			return nullptr;
		}


	private:
		vector<rdptr> objs;

};

KDTree::KDTree(const vector<rdptr>& objs, const AABB& space) {
	vector<RenderWrapper> objlist;
	for (auto & obj : objs)
		objlist.push_back(RenderWrapper(obj, obj->get_aabb()));
	root = build(objlist, space, 0);
}

shared_ptr<Trace> KDTree::get_trace(const Ray& ray) const {
	real_t mind; bool inside;
	if (!(root->box.intersect(ray, mind, inside)))
		return nullptr;
	return root->get_trace(ray, mind);
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

KDTree::Node* KDTree::build(const vector<RenderWrapper>& objs, const AABB& box, int depth) {
	if (objs.size() == 0) return nullptr;

#define ADDOBJ for (auto& obj : objs) ret->add_obj(obj.obj)

	Node* ret = new Node(box);

	if (depth > KDTREE_MAX_DEPTH) return nullptr;
	if (objs.size() < KDTREE_TERMINATE_OBJ_CNT) {
		ADDOBJ;
		return ret;
	}

	AAPlane pl = cut(objs, box, depth);
	pair<AABB, AABB> par;
	try {
		par = box.cut(pl);
	} catch (...) {
		ADDOBJ;
		return ret;		// pl is outside box, cannot go further
	}
	ret->pl = pl;

	vector<RenderWrapper> objl, objr;
	for (auto & obj : objs) {
		if (par.first.intersect(obj.box))
			objl.push_back(obj);
		if (par.second.intersect(obj.box))
			objr.push_back(obj);
	}
	Node *lch = build(objl, par.first, depth + 1),
		 *rch = build(objr, par.second, depth + 1);

	if (lch == nullptr && rch == nullptr) ADDOBJ;		// add obj to leaf node

	ret->child[0] = lch, ret->child[1] = rch;
	print_debug("depth: %d, lsize: %d, rsize: %d\n", depth, (int)objl.size(), (int)objr.size());
	return ret;
#undef ADDOBJ
}

