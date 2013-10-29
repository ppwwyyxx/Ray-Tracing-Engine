// File: mesh.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <string>
#include "renderable/face.hh"
#include "kdtree.hh"

#define INRANGE(x) (x) < (int)vtxs.size()

class MeshSimplifier;

class Mesh: public Renderable {
	public:
		// config
		bool smooth = true;
		bool mapped = false;
		bool use_tree = true;

		Vec pivot = Vec::infinity();
		real_t zoom_size = 5;

		vector<Vertex> vtxs;
		vector<shared_ptr<Face>> faces;
		vector<array<int, 3>> face_ids;
		shared_ptr<KDTree> tree;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		Mesh() {}

		Mesh(const Mesh& r):Renderable(r.get_texture()) {
			smooth = r.smooth;
			mapped = r.mapped;
			use_tree = r.use_tree;
			vtxs = r.vtxs;
			faces = r.faces;
			face_ids = r.face_ids;
			bound_min = r.bound_min;
			bound_max = r.bound_max;
			tree = r.tree;

			// IMPORTANT!!
			for (auto & k : faces) k->host = this;
		}

		Mesh(std::string fname, const Vec& _pivot,
				real_t _zsize, const shared_ptr<Texture>& _texture = nullptr);

		void add_vertex(const Vec& p) {
			vtxs.push_back(Vertex(p));
			bound_min.update_min(p - Vec::eps()), bound_max.update_max(p + Vec::eps());
		}

		/*
		 *void set_mapcoor(int t, const Vec2D& mapped) {
		 *    m_assert(INRANGE(t));
		 *    vtxs[t].mapcoor = mapped;
		 *}
		 */

		void add_faceid(int a, int b, int c) {
			m_assert(INRANGE(std::max(a, std::max(b, c))));
			face_ids.push_back(array<int, 3>{a, b, c});
		}

		void add_face(const array<int, 3>& t)
		{ add_face(t[0], t[1], t[2]); }

		void transform_vtxs();

		void finish();

		void simplify(real_t ratio);

		void clear() {
			vtxs.clear(); faces.clear(); face_ids.clear();
			bound_min = Vec::max(), bound_max = -Vec::max();
			tree = nullptr;
		}

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist) const override;

		AABB get_aabb() const override
		{ return AABB(bound_min, bound_max); }

	protected:
		void add_face(int a, int b, int c) {
			m_assert(INRANGE(std::max(a, std::max(b, c))));
			Face f(vtxs, a, b, c);
			f.host = this;
			faces.push_back(make_shared<Face>(f));
		}
};
