// File: mesh_simplifier.cc
// Date: Wed Jun 19 21:54:02 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <list>
#include <limits>
#include "mesh_simplifier.hh"
using namespace std;

void MeshSimplifier::Face::delete_from(Vertex*& u, Vertex*& v) {
	m_assert(contain(v));
	REP(k, 3)
		if (vtx[k] != u && vtx[k] != v) {
			vtx[k]->adj_face.erase(this);
			break;
		}
	v->adj_face.erase(this);
	vtx[0] = vtx[1] = vtx[2] = nullptr;
}

void MeshSimplifier::Face::change_to(Vertex*& u, Vertex*& v) {
	m_assert(count(v) == 0);
	m_assert(count(u) == 1);
	REP(k, 3) if (vtx[k] == u) vtx[k] = v;
	m_assert(vtx[0] != vtx[1] && vtx[1] != vtx[2] && vtx[2] != vtx[0]);
	norm = (vtx[2]->pos - vtx[0]->pos).cross((vtx[1]->pos - vtx[0]->pos)).get_normalized();
	v->adj_face.insert(this);
}

void MeshSimplifier::Vertex::change_to(Vertex* u, Vertex* v) {
	m_assert(this != v);
	m_assert(adj_vtx.find(u) != adj_vtx.end());
	adj_vtx.erase(u);
	adj_vtx.insert(v);
}

MeshSimplifier::MeshSimplifier(Mesh& _mesh, real_t ratio): mesh(_mesh) {
	int nvtx = mesh.vtxs.size();
	int nface = mesh.face_ids.size();
	target_num = nface * ratio;

	vtxs.reserve(nvtx);		// in case it change size
	faces.reserve(nface);

	REP(k, nvtx)
		vtxs.push_back(Vertex(mesh.vtxs[k].pos, k));

	REP(k, nface) {
		int a, b, c;
		tie(a, b, c) = mesh.face_ids[k];
		faces.push_back(Face(&vtxs[a], &vtxs[b], &vtxs[c]));
		vtxs[a].add_face(&faces[k]);
		vtxs[b].add_face(&faces[k]);
		vtxs[c].add_face(&faces[k]);
	}

	for (auto & k : vtxs) update_cost(&k);
	for (auto & k : vtxs) heap.push(Elem(&k));
}

real_t MeshSimplifier::cost(Vertex* u, Vertex* v) const {
	list<Face*> common_faces;
	for (const auto & uface : u->adj_face)
		if (v->adj_face.find(uface) != v->adj_face.end())
			// a common adj triangle
			common_faces.push_back(uface);

	real_t max = 0;
	for (const auto & uface : u->adj_face) {
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
	if (u->adj_vtx.size() == 0) {
		u->erased = true;
		return;
	}

	for (auto & uvtx : u->adj_vtx) {
		if (uvtx->erased) m_assert(false);
		if (update_min(min, cost(u, uvtx)))
			u->candidate = uvtx;
	}
	m_assert(u->candidate != nullptr);
	u->cost = min;
	u->cost_timestamp ++;
	heap.push(u);
}

int MeshSimplifier::collapse(Vertex* u, Vertex* v) {
	// move u onto v
	//	print_debug("collapse from %d to %d\n", u->id, v->id);
	u->erased = true;
	int ret = 0;

	list<Face*> to_delete;		// can't modify u inside loop
	for (auto & uface : u->adj_face) {
		m_assert(uface->contain(u));
		if (v->adj_face.find(uface) != v->adj_face.end()) {
			uface->delete_from(u, v);
			ret ++;
			to_delete.push_back(uface);
		} else
			uface->change_to(u, v);
	}
	for (auto & f : to_delete) u->adj_face.erase(f);

	for (auto & uvtx : u->adj_vtx)
		if (!uvtx->erased && uvtx != v) {
			uvtx->change_to(u, v);
			v->adj_vtx.insert(uvtx);
		}
	v->adj_vtx.erase(u);		// this must be put after the above line

	for (auto & uvtx : u->adj_vtx) update_cost(uvtx);
	return ret;
}

void MeshSimplifier::do_simplify() {
	int nowcnt = faces.size();
	while (nowcnt > target_num && nowcnt > 4) {
		/*
		 *real_t min = numeric_limits<real_t>::max();
		 *Vertex* candidate_u = nullptr;
		 *for (auto & u : vtxs) {
		 *    if (u.erased) continue;
		 *    if (update_min(min, u.cost))
		 *        candidate_u = &u;
		 *}
		 */

		auto & ele = heap.top(); heap.pop();
		if (ele.outofdate()) continue;
		if (ele.v->erased) continue;
		Vertex* candidate_u = ele.v;

		m_assert(candidate_u != nullptr);
		m_assert(candidate_u->candidate != nullptr);
		nowcnt -= collapse(candidate_u, candidate_u->candidate);
		if (nowcnt % 100 == 0)
			print_progress((faces.size() - nowcnt) * 100 / (faces.size() - target_num));
	}
}

void MeshSimplifier::write_back() {
	mesh.clear();
	int cnt = 0;
	for (auto & v : vtxs) {
		if (!v.erased)
			mesh.add_vertex(v.pos), v.id = cnt++;
		else
			v.id = -1;
	}

	for (auto & f : faces) {
		bool dead = false;
		REP(k, 3) if (!f.vtx[k] || f.vtx[k]->erased) dead = true;
		if (dead) continue;
		REP(k, 3) if (f.vtx[k]->id == -1) m_assert(false);

		mesh.add_faceid(f.vtx[0]->id, f.vtx[1]->id, f.vtx[2]->id);
	}
	for (auto & ids : mesh.face_ids) mesh.add_face(ids);
}

void MeshSimplifier::simplify() {
	do_simplify();
	write_back();
}
