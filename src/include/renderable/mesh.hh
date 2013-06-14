// File: mesh.hh
// Date: Fri Jun 14 22:24:34 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <string>
#include "renderable/face.hh"

#define INRANGE(x) (x) < (int)vtxs.size()
class Mesh: public RenderAble {
	public:
		bool have_inside = false;
		bool smooth = true;
		bool mapped = false;

		vector<Face> faces;
		vector<Vertex> vtxs;

		Vec bound_min = Vec::max(), bound_max = -Vec::max();

		Mesh() {}

		Mesh(std::string fname, const shared_ptr<Texture>& _texture = nullptr);

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

		void add_face(int a, int b, int c) {
			m_assert(INRANGE(max(a, max(b, c))));
			faces.push_back(Face(vtxs, a, b, c));
		}

		void finish_add();

		shared_ptr<Trace> get_trace(const Ray& ray) const;

		AABB get_aabb() const;

	protected:
		friend class MeshTrace;
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
