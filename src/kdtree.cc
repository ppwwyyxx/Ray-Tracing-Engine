// File: kdtree.cc
// Date: Sat Sep 21 11:30:14 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include <algorithm>
#include <future>
#include <omp.h>

#include "kdtree.hh"
#include "const.hh"
#include "lib/utils.hh"
#include "lib/debugutils.hh"
#include "lib/Timer.hh"
using namespace std;

class KDTree::Node {
	public:
		AABB box;
		Node* child[2];
		AAPlane pl;

		Node(const AABB& _box, Node* p1 = nullptr, Node* p2 = nullptr) : box(_box), child{p1, p2} { }

		~Node() { delete child[0]; delete child[1]; }

		bool leaf() const
		{ return child[0] == nullptr && child[1] == nullptr; }

		void set_objs(const vector<rdptr>& _objs)
		{ objs = _objs; }

		void add_obj(rdptr obj)
		{ objs.push_back(obj); }

		shared_ptr<Trace> get_trace(const Ray& ray, real_t inter_dist, real_t max_dist = -1) const {
			// call when know to intersect
			if (leaf()) {
				// find first obj
				real_t min = numeric_limits<real_t>::max();
				shared_ptr<Trace> ret;

				for (auto & obj : objs) {
					auto tmp = obj->get_trace(ray, max_dist);
					if (tmp) {
						real_t d = tmp->intersection_dist();
						if (update_min(min, d)) ret = tmp;
					}
				}
				return move(ret);
			}

			real_t mind = -1, mind2 = -1;
			bool inside;
			real_t pivot = ray.get_dist(inter_dist)[pl.axis];
			int first_met = (int)(pivot > pl.pos);

			Node* ch0 = child[first_met], *ch1 = child[1 - first_met];
			if (!ch0 || !ch0->box.intersect(ray, mind, inside)) ch0 = nullptr;
			if (ch0 && (max_dist == -1 || mind < max_dist)) {
				auto ret = ch0->get_trace(ray, mind, max_dist);
				if (ret) {
					if (ray.debug) {
						cout << "ray intersect with box " << ch0->box << endl;
						cout << "interpoint: " << ret->intersection_point() << endl;
						cout << endl;
					}
					if (!ch0->leaf()) return ret;
					Vec inter_p = ret->intersection_point();
					if (ch0->box.contain(inter_p)) return ret;
				} else {
					if (ray.debug)
						cout << "not intersect with box" << ch0->box << endl;
				}
			}

			if (!ch1 || !ch1->box.intersect(ray, mind2, inside)) ch1 = nullptr;

			m_assert(!(ch0 && ch1 && mind == -1 && mind2 == -1));
			/*
			 *if (ch0 && ch1 && mind > mind2 + EPS) {
			 *    print_debug("%lf, %lf\n", mind, mind2);
			 *    m_assert(false);
			 *}
			 */
			if (ch1 && (max_dist == -1 || mind2 < max_dist)) {
				auto ret = ch1->get_trace(ray, mind2, max_dist);
				if (ret) {
					if (ray.debug) {
						cout << "ray intersect with box " << ch0->box << endl;
						cout << "interpoint: " << ret->intersection_point() << endl;
						cout << endl;
					}
					if (!ch1->leaf()) return ret;
					Vec inter_p = ret->intersection_point();
					if (ch1->box.contain(inter_p)) return ret;
				} else {
					if (ray.debug)
						cout << "not intersect with box" << ch0->box << endl;
				}
			}
			if (ray.debug)
				cout << "reaching end of node->get_trace()" << endl;
			return nullptr;
		}

	private:
		vector<rdptr> objs;

};

KDTree::KDTree(const vector<rdptr>& objs, const AABB& space) {
	vector<RenderWrapper> objlist;
	for (auto & obj : objs) {
		m_assert(!obj->infinity());
		AABB aabb = obj->get_aabb();
		objlist.push_back(RenderWrapper(obj, aabb));
		bound_min.update_min(aabb.min), bound_max.update_max(aabb.max);
	}
	Timer timer;
	root = build(move(objlist), space, 0);
	printf("Build tree spends %lf seconds\n", timer.get_time());
}

KDTree::~KDTree() { delete root; }

shared_ptr<Trace> KDTree::get_trace(const Ray& ray, real_t max_dist) const {
	real_t mind; bool inside;
	if (!(root->box.intersect(ray, mind, inside)))
		return nullptr;
	if (mind > max_dist && max_dist != -1) return nullptr;
	return root->get_trace(ray, mind, max_dist);
}

AAPlane KDTree::cut(const vector<RenderWrapper>& objs, int depth) const {
	AAPlane ret(depth % 3, 0);

	vector<real_t> min_list;
	for (auto &obj : objs)
		min_list.push_back(obj.box.min[ret.axis]);
	nth_element(min_list.begin(), min_list.begin() + min_list.size() / 2, min_list.end());
	// partial sort
	//
	ret.pos = min_list[min_list.size() / 2] + 2 * EPS;		// SEE what happen
	return ret;
}

KDTree::Node* KDTree::build(const vector<RenderWrapper>& objs, const AABB& box, int depth) {
	if (objs.size() == 0) return nullptr;

#define ADDOBJ for (auto& obj : objs) ret->add_obj(obj.obj)

	Node* ret = new Node(box);

	if (depth > KDTREE_MAX_DEPTH) return nullptr;
	if (objs.size() <= KDTREE_TERMINATE_OBJ_CNT) {
		ADDOBJ;
		return ret;
	}
	int nobj = objs.size();
	pair<AABB, AABB> par;
	AAPlane best_pl;
	real_t min_cost = numeric_limits<real_t>::max();


/*	// algo 1 (naive kdtree)
 *    best_pl = cut(objs, depth);
 *    try {
 *        par = box.cut(best_pl);
 *    } catch (...) {
 *        ADDOBJ;
 *        return ret;		// pl is outside box, cannot go further
 *    }
 *    ret->pl = best_pl;
 *
 *    vector<RenderWrapper> objl, objr;
 *    for (auto & obj : objs) {
 *        if (obj.box.max[best_pl.axis] >= best_pl.pos - EPS) objr.push_back(obj);
 *        if (obj.box.min[best_pl.axis] <= best_pl.pos + EPS) objl.push_back(obj);
 *    }
 */ // end of algo 1

	// algo 2 (SAH kdtree)
	// On building fast kd-trees for ray tracing, and on doing that in O (N log N)
	// Wald, Ingo and Havran, Vlastimil
	// O(n log^2(n)) build

#define GEN_CAND_LIST \
	do { \
		for (auto & obj: objs) \
			cand_list.push_back(make_pair(obj.box.min[dim] - EPS, true)), \
				cand_list.push_back(make_pair(obj.box.max[dim] + EPS, false)); \
		sort(cand_list.begin(), cand_list.end(), \
				[](const pair<real_t, bool>& a, const pair<real_t, bool>& b) \
					{return a.first < b.first;} \
			); \
	} while (0)

	future<vector<pair<real_t, bool>>> task[3];

	const int THREAD_DEPTH_THRES = 1;
	if (depth < THREAD_DEPTH_THRES) {
		REP(dim, 3)
			task[dim] = async(launch::async,
					[&objs, dim]() {
						vector<pair<real_t, bool>> cand_list;
						GEN_CAND_LIST;
						return move(cand_list);
					}
			);
	}

	REP(dim, 3) {
		vector<pair<real_t, bool>> cand_list;		//// true: min, false: max
		if (depth >= THREAD_DEPTH_THRES)
			GEN_CAND_LIST;
		else
			cand_list = move(task[dim].get());

		int lcnt = 0, rcnt = nobj;
		auto ptr = cand_list.begin();

		do {
			AAPlane pl(dim, ptr->first);
			try {
				auto par = box.cut(pl);
				real_t cost = par.first.area() * lcnt + par.second.area() * rcnt;
				if (lcnt == 0 || rcnt == 0 || (lcnt + rcnt == nobj - 1 && rcnt == 1))
					cost *= 0.8;		// this is a hack
				if (update_min(min_cost, cost)) best_pl = pl;
			} catch (...) {}

			if (ptr->second) lcnt ++; else rcnt --;

			auto old = ptr++;
			while (ptr != cand_list.end() && ptr->first - old->first < EPS) {
				if (ptr->second) lcnt ++;
				else rcnt --;
				old = ptr++;
			}
		} while (ptr != cand_list.end());
	}

	if (best_pl.axis == AAPlane::ERROR) { // didn't find best_pl
		ADDOBJ;
		return ret;
	}

	ret->pl = best_pl;
	par = box.cut(best_pl);
    vector<RenderWrapper> objl, objr;
	for (auto & obj : objs) {
		if (obj.box.max[best_pl.axis] >= best_pl.pos) objr.push_back(obj);
		if (obj.box.min[best_pl.axis] <= best_pl.pos) objl.push_back(obj);
	}
	// end of algo 2

	if (depth < 1) { // parallel when depth is small
		future<Node*> lch_fut = async(launch::async, [&]() {
				return build(move(objl), par.first, depth + 1); });
		future<Node*> rch_fut = async(launch::async, [&]() {
				return build(move(objr), par.second, depth + 1); });

		ret->child[0] = lch_fut.get();
		ret->child[1] = rch_fut.get();
	} else {
		ret->child[0] = build(move(objl), par.first, depth + 1);
		ret->child[1] = build(move(objr), par.second, depth + 1);
	}


	if (ret->leaf()) ADDOBJ;		// add obj to leaf node

	return ret;
#undef ADDOBJ
#undef GET_CAND_LIST
}

