// File: mesh.hh
// Date: Tue Jun 18 10:58:10 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <string>
#include "renderable/face.hh"
#include "lib/kdtree.hh"

#define INRANGE(x) (x) < (int)vtxs.size()
class Mesh: public RenderAble {
	public:
		// config
		bool have_inside = false;
		bool smooth = true;
		bool mapped = false;
		bool use_tree = false;
		Vec pivot = Vec::infinity();
		real_t zoom_size = 5;

		vector<Vertex> vtxs;
		vector<rdptr> faces;
		vector<tuple<int, int, int>> face_ids;
		shared_ptr<KDTree> tree;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		Mesh() {}
		Mesh(std::string fname, const Vec& _pivot, real_t _zsize, const shared_ptr<Texture>& _texture = nullptr);

		void add_vertex(const Vec& p) {
			int id = vtxs.size();
			vtxs.push_back(Vertex(p, id));
			bound_min.update_min(p), bound_max.update_max(p);
		}

		void set_mapcoor(int t, const Vec2D& mapped) {
			m_assert(INRANGE(t));
			vtxs[t].mapcoor = mapped;
		}

		void set_norm(int t, const Vec& norm) {
			m_assert(INRANGE(t));
			vtxs[t].norm = norm;
		}

		void add_faceid(int a, int b, int c) {
			m_assert(INRANGE(max(a, max(b, c))));
			face_ids.push_back(tuple<int, int, int>{a, b, c});
		}

		void transform_vtxs();

		void finish_add();

		shared_ptr<Trace> get_trace(const Ray& ray) const;

		AABB get_aabb() const;

	protected:
		friend class MeshTrace;

		void add_face(const tuple<int, int, int>& t)
		{ add_face(get<0>(t), get<1>(t), get<2>(t));}

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

		bool intersect();

		real_t intersection_dist();

		Vec normal();
};

#undef INRANGE
