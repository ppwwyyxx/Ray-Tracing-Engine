// File: mesh_simplifier.hh
// Date: Wed Jun 19 17:54:21 2013 +0800
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
				Vertex* third = nullptr;
				REP(k, 3)
					if (vtx[k] != u && vtx[k] != v) {
						third = vtx[k];
						break;
					}
				m_assert(third != nullptr);
				third->adj_face.erase(this);
				third->adj_vtx.erase(v);		// XXX for assert, later delete
			}

			void change_to(Vertex*& u, Vertex*& v) {
				REP(k, 3) if (vtx[k] == u) vtx[k] = v;
				norm = (vtx[2]->pos - vtx[0]->pos).cross((vtx[1]->pos - vtx[0]->pos)).get_normalized();
			}
		};

		struct Vertex {
			Vec pos;
			int id = -1;
			bool erased = false;
			set<Vertex*> adj_vtx;
			set<Face*> adj_face;

			real_t cost;
			Vertex* candidate;

			Vertex(Vec _pos):pos(_pos) {}

			void add_face(Face* f) {
				adj_face.insert(f);
				REP(k, 3) adj_vtx.insert(f->vtx[k]);
			}

			void change_to(Vertex*& u, Vertex*& v) {
				m_assert(adj_vtx.find(u) != adj_vtx.end());
				adj_vtx.erase(u);
				m_assert(adj_vtx.find(v) != adj_vtx.end());
				adj_vtx.insert(v);
			}
		};

		vector<Vertex> vtxs;
		vector<Face> faces;
		int target_num;			// shrink to this number

		real_t cost(Vertex*, Vertex*) const;

		void update_cost(Vertex*);

		void collapse(Vertex*, Vertex*);

		void do_simplify();

		void write_back();

	public:
		MeshSimplifier(Mesh& mesh, real_t _ratio);

		void simplify();
};
