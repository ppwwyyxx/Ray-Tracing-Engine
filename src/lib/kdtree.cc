// File: kdtree.cc
// Date: Fri Jun 14 20:21:29 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

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

	private:
		vector<shared_ptr<RenderAble>> objs;

};


KDTree::Node* KDTree::build(const vector<shared_ptr<RenderAble>>& objs,
		const AABB& box, int depth) {


}

