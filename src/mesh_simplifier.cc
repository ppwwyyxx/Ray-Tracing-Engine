// File: mesh_simplifier.cc

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <list>
#include <limits>
#include <algorithm>
#include <omp.h>
#include "lib/Timer.hh"
#include "mesh_simplifier.hh"
using namespace std;

// A simple, fast, and effective polygon reduction algorithm
// Melax, Stan, 1998

void MeshSimplifier::Face::delete_from(Vertex* u, Vertex* v) {
	m_assert(contain(v));
	REP(k, 3)
		if (vtx[k] != u && vtx[k] != v) {
			vtx[k]->adj_face.erase(this);
			break;
		}
	v->adj_face.erase(this);
	vtx[0] = vtx[1] = vtx[2] = nullptr;
}

void MeshSimplifier::Face::change_to(Vertex* u, Vertex* v) {
	m_assert(count(v) == 0);
	m_assert(count(u) == 1);
	REP(k, 3) if (vtx[k] == u) vtx[k] = v;
	m_assert(vtx[0] != vtx[1] && vtx[1] != vtx[2] && vtx[2] != vtx[0]);
	Vec newnorm = (vtx[2]->pos - vtx[0]->pos).cross((vtx[1]->pos - vtx[0]->pos));
	if (newnorm.sqr() > 0)		// sometimes three vertex will be on the same line
		norm = newnorm.get_normalized();
	v->adj_face.insert(this);
}

void MeshSimplifier::Vertex::change_to(Vertex* u, Vertex* v) {
	m_assert(this != v);
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
		vtxs.emplace_back(mesh.vtxs[k].pos, k);

	REP(k, nface) {
		auto & tmp = mesh.face_ids[k];
		int a = tmp[0], b = tmp[1], c = tmp[2];
		faces.emplace_back(&vtxs[a], &vtxs[b], &vtxs[c]);
		vtxs[a].add_face(&faces[k]);
		vtxs[b].add_face(&faces[k]);
		vtxs[c].add_face(&faces[k]);
	}

#pragma omp parallel for schedule(dynamic)
	REP(k, nvtx) update_cost(&vtxs[k]);
	/*
	 *for (auto& vtx: vtxs) update_cost(&vtx);
	 */
}

real_t MeshSimplifier::cost(Vertex* u, Vertex* v) const {
	list<Face*> common_faces;
	// common adj triangle of u and v
	copy_if(u->adj_face.begin(), u->adj_face.end(),
			back_inserter(common_faces),
			[&v](Face* f) -> bool {
			return v->adj_face.find(f) != v->adj_face.end();
			});

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
	if (u->adj_vtx.size() == 0) {
		u->erased = true;
		return;
	}

	real_t min = numeric_limits<real_t>::max();
	u->candidate = nullptr;

	for (auto & uvtx : u->adj_vtx) {
		if (uvtx->erased) { m_assert(false);}
		if (update_min(min, cost(u, uvtx)))
			u->candidate = uvtx;
	}
	m_assert(u->candidate != nullptr);
	u->cost = min;
	u->cost_timestamp ++;
#pragma omp critical
	heap.push(u);
}

int MeshSimplifier::collapse(Vertex* u, Vertex* v) {
	// move u onto v
	u->erased = true;
	int ret = 0;

	list<Face*> to_delete;		// can't modify a set inside loop
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

	/*
	 *#pragma omp parallel
	 *#pragma omp single
	 *    {
	 *        for (auto itr = u->adj_vtx.begin(); itr != u->adj_vtx.end(); ++itr)
	 *    #pragma omp task firstprivate(itr)
	 *            update_cost(*itr);
	 *    #pragma omp taskwait
	 *    }
	 */
	for_each(u->adj_vtx.begin(), u->adj_vtx.end(),
			[&](Vertex* n) {
			update_cost(n);
			});
	return ret;
}

void MeshSimplifier::do_simplify() {
	int nowcnt = faces.size();
	while (nowcnt > target_num && nowcnt > 4) {
		/*// without heap
		 *real_t min = numeric_limits<real_t>::max();
		 *Vertex* candidate_u = nullptr;
		 *for (auto & u : vtxs) {
		 *    if (u.erased) continue;
		 *    if (update_min(min, u.cost))
		 *        candidate_u = &u;
		 *}
		 */

		// use heap
		auto & ele = heap.top();
		heap.pop();
		if (ele.outofdate()) continue;
		if (ele.v->erased) continue;
		Vertex* candidate_u = ele.v;

		m_assert(candidate_u != nullptr);
		m_assert(candidate_u->candidate != nullptr);
		nowcnt -= collapse(candidate_u, candidate_u->candidate);
		if (nowcnt % 1000 == 0)
			print_progress((faces.size() - nowcnt) * 100 / (faces.size() - target_num));
	}
}

void MeshSimplifier::write_back() {		// write vtxs and face_ids
	mesh.clear();
	int cnt = 0;
	for (auto & v : vtxs) {
		if (!v.erased) mesh.add_vertex(v.pos), v.id = cnt++;
		else v.id = -1;
	}

	for (auto & f : faces) {
		bool erased = false;
		REP(k, 3) if (!f.vtx[k] or f.vtx[k]->erased) erased = true;
		if (erased) continue;
		REP(k, 3) if (f.vtx[k]->id == -1) { m_assert(false);}

		mesh.add_faceid(f.vtx[0]->id, f.vtx[1]->id, f.vtx[2]->id);
	}
}

void MeshSimplifier::simplify() {
	do_simplify();
	write_back();
}
