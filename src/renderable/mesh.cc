// File: mesh.cc
// Date: Wed Jun 19 17:52:33 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <algorithm>
#include "renderable/mesh.hh"
#include "lib/objreader.hh"
using namespace std;

Mesh::Mesh(std::string fname, const Vec& _pivot, real_t _zsize, const shared_ptr<Texture>& _texture):
	pivot(_pivot), zoom_size(_zsize) {
	texture = _texture;
	ObjReader::read_in(fname, this);
	transform_vtxs();
	for (auto &ids : face_ids) add_face(ids);
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

shared_ptr<Trace> Mesh::get_trace(const Ray& ray, real_t dist) const {
	if (use_tree)
		return tree->get_trace(ray, dist);
	shared_ptr<Trace> ret = make_shared<MeshTrace>(*this, ray);
	if (ret->intersect()) {
		if (dist == -1 || ret->intersection_dist() < dist)
			return ret;
	}
	return nullptr;
}

void Mesh::finish() {		// build tree, calculate smooth norm
	if (use_tree)
		tree = make_shared<KDTree>(faces, get_aabb());

	if (smooth) {		// calculate vtx norm
		int nvtx = vtxs.size();

		struct NormSum {
			Vec sum = Vec(0, 0, 0);
			int cnt = 0;
			void add(const Vec& v) { sum = sum + v, cnt ++; }
		};

		NormSum* norm_sum = new NormSum[nvtx];
		for (auto & t : face_ids) {
			Vec tmp_norm = Triangle(vtxs[get<0>(t)].pos, vtxs[get<1>(t)].pos, vtxs[get<2>(t)].pos).norm;
			norm_sum[get<0>(t)].add(tmp_norm);
			norm_sum[get<1>(t)].add(tmp_norm);
			norm_sum[get<2>(t)].add(tmp_norm);
		}
		REP(k, nvtx)
			vtxs[k].norm = norm_sum[k].sum / norm_sum[k].cnt;
		delete[] norm_sum;
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

