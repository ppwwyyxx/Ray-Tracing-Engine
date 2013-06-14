// File: mesh.hh
// Date: Fri Jun 14 11:23:23 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <tuple>
#include <memory>
#include <vector>

#include "lib/debugutils.hh"
#include "renderable/renderable.hh"
#include "geometry/geometry.hh"
using std::tuple;
using std::vector;
using std::shared_ptr;

class Triangle {
	public:
		Vec v, e1, e2;
		Vec norm;

		Triangle(const Vec& a, const Vec& b, const Vec& c):
			v(a), e1(b - a), e2(c - a),norm(e1.cross(e2))
		{}

		tuple<real_t, real_t, real_t> get_intersect(const Ray& ray) const;
};

class Vertex {
	public:
		Vec pos;
		int id;

		Vec norm = Vec::infinity();		// can be defined by obj
		Vec2D mapcoor = Vec2D::infinity();

		Vertex(const Vec& _pos, int _id):
			pos(_pos), id(_id) {}
};

class Face : public RenderAble {
	public:
		Triangle tri;
		Vec norm; // can later be re-calculated, can be assigned other than tri.normal

		tuple<int, int, int> vtxid;
		vector<Face*> adj_faces;

		Face(const vector<Vertex>& vtxs, int a, int b, int c):
			tri(vtxs[a].pos, vtxs[b].pos, vtxs[c].pos),
			norm(tri.norm),
			vtxid{a, b, c} {}

		void add_adj_face(Face* f)
		{ adj_faces.push_back(f); }

		shared_ptr<Trace> get_trace(const Ray& ray) const;

	protected:
		friend class FaceTrace;

};

class FaceTrace : public Trace {
	private:
		const Face& face;
		real_t dist, gx, gy;

		shared_ptr<Surface> transform_get_property() const;

	public:
		FaceTrace(const Face& _face, const Ray& _ray):
			Trace(&_face, _ray), face(_face){};

		bool intersect();

		real_t intersection_dist();

		Vec normal();
};

#define INRANGE(x) (x) < (int)vtxs.size()
class Mesh: public RenderAble {
	public:
		bool have_inside = false;
		bool smooth = true;

		vector<Face> faces;
		vector<Vertex> vtxs;

		void add_vertex(const Vec& p) {
			int id = vtxs.size();
			vtxs.push_back(Vertex(p, id));
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
};

#undef INRANGE
