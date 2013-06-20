// File: mesh.hh
// Date: Thu Jun 20 13:51:50 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <string>
#include "renderable/face.hh"
#include "lib/kdtree.hh"

#define INRANGE(x) (x) < (int)vtxs.size()

class MeshSimplifier;

class Mesh: public RenderAble {
	public:
		// config
		bool smooth = true;
		bool mapped = false;
		bool use_tree = true;

		Vec pivot = Vec::infinity();
		real_t zoom_size = 5;

		vector<Vertex> vtxs;
		vector<rdptr> faces;
		vector<tuple<int, int, int>> face_ids;
		shared_ptr<KDTree> tree;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		Mesh() {}

		Mesh(const Mesh& r) {
			texture = r.texture;
			smooth = r.smooth;
			mapped = r.mapped;
			use_tree = r.use_tree;
			vtxs = r.vtxs;
			faces = r.faces;
			face_ids = r.face_ids;
			bound_min = r.bound_min;
			bound_max = r.bound_max;
			tree = r.tree;
			for (auto & k : faces) dynamic_pointer_cast<Face>(k)->host = this;
		}

		Mesh(std::string fname, const Vec& _pivot, real_t _zsize, const shared_ptr<Texture>& _texture = nullptr);

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
			m_assert(INRANGE(max(a, max(b, c))));
			face_ids.push_back(tuple<int, int, int>{a, b, c});
		}

		void add_face(const tuple<int, int, int>& t)
		{ add_face(get<0>(t), get<1>(t), get<2>(t));}

		void transform_vtxs();

		void finish();

		void simplify(real_t ratio);

		void clear() {
			vtxs.clear(); faces.clear(); face_ids.clear();
			bound_min = Vec::max(), bound_max = -Vec::max();
			tree = nullptr;
		}

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist = -1) const override;

		AABB get_aabb() const override
		{ return AABB(bound_min, bound_max); }

	protected:
		friend class MeshTrace;

		void add_face(int a, int b, int c) {
			m_assert(INRANGE(max(a, max(b, c))));
			Face f(vtxs, a, b, c);
			f.host = this;
			faces.push_back(rdptr(new Face(f)));
		}
};

class MeshTrace : public Trace {
	private:
		const Mesh& mesh;

		shared_ptr<Trace> nearest_trace = nullptr;

		shared_ptr<Surface> transform_get_property() const;

	public:
		MeshTrace(const Mesh& _mesh, const Ray& _ray):
			Trace(&_mesh, _ray), mesh(_mesh){};

		bool intersect() override;

		real_t intersection_dist() override;

		Vec normal() override;
};

#undef INRANGE
