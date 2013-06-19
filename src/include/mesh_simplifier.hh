// File: mesh_simplifier.hh
// Date: Wed Jun 19 21:09:12 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <set>
#include "renderable/mesh.hh"
using std::vector;
using std::set;

class MeshSimplifier {
	private:
		Mesh& mesh;

		struct Vertex;

		struct Face {
			Vertex * vtx[3];
			Vec norm;

			Face(Vertex* a, Vertex* b, Vertex* c): vtx{a, b, c} {
				norm = (c->pos - a->pos).cross((b->pos - a->pos)).get_normalized();
			}

			void delete_from(Vertex*& u, Vertex*& v) {
				m_assert(contain(v));
				REP(k, 3)
					if (vtx[k] != u && vtx[k] != v) {
						vtx[k]->adj_face.erase(this);
						break;
					}
			//	u->adj_face.erase(this);		// XXX can't modify set when inside it
				v->adj_face.erase(this);
				vtx[0] = vtx[1] = vtx[2] = nullptr;
			}

			void change_to(Vertex*& u, Vertex*& v) {
				m_assert(count(v) == 0);
				m_assert(count(u) == 1);
				REP(k, 3) if (vtx[k] == u) vtx[k] = v;
				m_assert(vtx[0] != vtx[1] && vtx[1] != vtx[2] && vtx[2] != vtx[0]);
				norm = (vtx[2]->pos - vtx[0]->pos).cross((vtx[1]->pos - vtx[0]->pos)).get_normalized();
				v->adj_face.insert(this);
			}

#ifdef DEBUG
			bool contain(Vertex* v) const
			{ return (v == vtx[0]) + (v == vtx[1]) + (v == vtx[2]) == 1; }

			int count(Vertex* v) const
			{ return (v == vtx[0]) + (v == vtx[1]) + (v == vtx[2]); }
#endif
		};

		struct Vertex {
			Vec pos;
			int id = -1;
			bool erased = false;
			set<Vertex*> adj_vtx;
			set<Face*> adj_face;

			real_t cost;
			Vertex* candidate = nullptr;

			Vertex(Vec _pos, int _id = -1):pos(_pos), id(_id) {}

			void add_face(Face* f) {
				adj_face.insert(f);
				REP(k, 3) if (f->vtx[k] != this) adj_vtx.insert(f->vtx[k]);
			}

			void change_to(Vertex* u, Vertex* v) {
				m_assert(this != v);
				m_assert(adj_vtx.find(u) != adj_vtx.end());
				adj_vtx.erase(u);
				adj_vtx.insert(v);
			}
		};

		vector<Vertex> vtxs;
		vector<Face> faces;
		int target_num;			// shrink to this number

		real_t cost(Vertex*, Vertex*) const;

		void update_cost(Vertex*);

		int collapse(Vertex*, Vertex*);		// return the number of faces it simplified

		void do_simplify();

		void write_back();

	public:
		MeshSimplifier(Mesh& mesh, real_t _ratio);

		void simplify();
};
