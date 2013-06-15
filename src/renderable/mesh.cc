// File: mesh.cc
// Date: Sat Jun 15 20:33:45 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "renderable/mesh.hh"
#include "lib/objreader.hh"
using namespace std;

Mesh::Mesh(std::string fname, const shared_ptr<Texture>& _texture) {
	texture = _texture;
	ObjReader::read_in(fname, this);
}

shared_ptr<Trace> Mesh::get_trace(const Ray& ray) const {
	if (use_tree)
		return tree->get_trace(ray);
	shared_ptr<Trace> ret(new MeshTrace(*this, ray));
	if (ret->intersect()) return ret;
	return nullptr;
}

AABB Mesh::get_aabb() const
{ return AABB(bound_min, bound_max); }

void Mesh::finish_add() {
	if (faces.size() > USE_KDTREE_THRES) {
		use_tree = true;
		tree = shared_ptr<KDTree>(new KDTree(faces, get_aabb()));
	}
}

shared_ptr<Surface> MeshTrace::transform_get_property() const {
	m_assert(mesh.mapped);
	return nullptr;
}

bool MeshTrace::intersect() {
	real_t min = numeric_limits<real_t>::infinity();
	for (auto & face : mesh.faces) {
		shared_ptr<Trace> tmp = face->get_trace(ray);
		if (tmp && update_min(min, tmp->intersection_dist()))
			nearest_trace = tmp;
	}
	if (nearest_trace) {		// find a face
		inter_dist = min;
		return true;
	}
	return false;
}

real_t MeshTrace::intersection_dist()
{ return nearest_trace->intersection_dist(); }

Vec MeshTrace::normal()
{ return nearest_trace->normal(); }

