// File: mesh_simplifier.cc
// Date: Wed Jun 19 17:55:06 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <list>
#include <limits>
#include "mesh_simplifier.hh"
using namespace std;


MeshSimplifier::MeshSimplifier(Mesh& _mesh, real_t ratio): mesh(_mesh) {
	int nvtx = mesh.vtxs.size();
	int nface = mesh.faces.size();
	target_num = nface * ratio;

	vtxs.reserve(nvtx);		// in case it change size
	faces.reserve(nface);

	for (auto & v : mesh.vtxs)
		vtxs.push_back(Vertex(v.pos));

	REP(k, nface) {
		int a, b, c;
		tie(a, b, c) = mesh.face_ids[k];
		faces.push_back(Face(&vtxs[a], &vtxs[b], &vtxs[c]));
		vtxs[a].add_face(&faces[k]);
		vtxs[b].add_face(&faces[k]);
		vtxs[c].add_face(&faces[k]);
	}

}

real_t MeshSimplifier::cost(Vertex* u, Vertex* v) const {
	list<Face*> common_faces;
	for (auto & uface : u->adj_face)
		if (v->adj_face.find(uface) != v->adj_face.end())
			// a common adj triangle
			common_faces.push_back(uface);

	real_t max = 0;
	for (auto & uface : u->adj_face) {
		real_t min = 1;
		for (auto & cf : common_faces) {
			real_t dot = 1 - (uface->norm).dot(cf->norm);
			update_min(min, dot);
		}
		update_max(max, min);
	}
	return (u->pos - v->pos).mod() * max;
}

void MeshSimplifier::update_cost(Vertex* u) {
	// cost of `I collapse to other`
	real_t min = numeric_limits<real_t>::max();
	u->candidate = nullptr;
	for (auto & uvtx : u->adj_vtx) {
		if (uvtx->erased) m_assert(false);
		if (update_min(min, cost(u, uvtx)))
			u->candidate = uvtx;
	}
	m_assert(u->candidate != nullptr);
	u->cost = min;
}

void MeshSimplifier::collapse(Vertex* u, Vertex* v) {
	// move u onto v
	u->erased = true;
	for (auto & uface : u->adj_face) {
		if (v->adj_face.find(uface) != v->adj_face.end())
			uface->delete_from(u, v);
		else
			uface->change_to(u, v);
	}

	for (auto & uvtx : u->adj_vtx)
		if (uvtx != v)
			uvtx->change_to(u, v);

	for (auto & uvtx : u->adj_vtx) update_cost(uvtx);
}

void MeshSimplifier::do_simplify() {
	int nowcnt = faces.size();
	while (nowcnt > target_num) {
		real_t min = numeric_limits<real_t>::max();
		Vertex* candidate_u = nullptr;
		for (auto & u : vtxs) {
			if (u.erased) continue;
			if (update_min(min, u.cost))
				candidate_u = &u;
		}
		m_assert(candidate_u != nullptr);
		collapse(candidate_u, candidate_u->candidate);
	}
}

void MeshSimplifier::write_back() {
	mesh.clear();
	int cnt = 0;
	for (auto & v : vtxs)
		if (!v.erased)
			mesh.add_vertex(v.pos), v.id = cnt++;

	for (auto & f : faces) {
		bool dead = false;
		REP(k, 3) if (f.vtx[k]->erased) dead = true;
		if (dead) continue;
		REP(k, 3) if (f.vtx[k]->id == -1) m_assert(false);

		mesh.add_faceid(f.vtx[0]->id, f.vtx[1]->id, f.vtx[2]->id);
	}
	for (auto & ids : mesh.face_ids) mesh.add_face(ids);
	mesh.finish();
}

void MeshSimplifier::simplify() {
	do_simplify();
	write_back();
}
