// File: mesh.cc
// Date: Mon Jun 17 10:55:58 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <algorithm>
#include "renderable/mesh.hh"
#include "lib/objreader.hh"
using namespace std;

Mesh::Mesh(std::string fname, const Vec& _pivot, real_t _zsize, const shared_ptr<Texture>& _texture):
	pivot(_pivot), zoom_size(_zsize) {
	texture = _texture;
	ObjReader::read_in(fname, this);
}

void Mesh::transform_vtxs() {
	Vec sum;
	real_t zfactor = zoom_size / (bound_max - bound_min).get_max();		// * factor
	for (auto &k : vtxs) sum = sum + k.pos;
	sum = pivot - sum / vtxs.size();

	for_each(vtxs.begin(), vtxs.end(),
		[&](Vertex &v) {
			v.pos = pivot + (v.pos + sum - pivot) * zfactor;
	});
	bound_min = pivot + (bound_min + sum - pivot) * zfactor;
	bound_max = pivot + (bound_max + sum - pivot) * zfactor;
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

