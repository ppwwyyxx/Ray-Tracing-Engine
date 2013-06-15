// File: face.hh
// Date: Sat Jun 15 22:16:41 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
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
			v(a), e1(b - a), e2(c - a),norm(e1.cross(e2)) {}

		Vec get(int i) const {
			if (!i) return v + e1;
			else return v + e2;
		}

		real_t get_intersect(const Ray& ray, real_t& gx, real_t& gy) const;
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

		RenderAble* host;

		tuple<int, int, int> vtxid;
		vector<Face*> adj_faces;
		const vector<Vertex>& vtxs;

		Face(const vector<Vertex>& _vtxs, int a, int b, int c):
			tri(_vtxs[a].pos, _vtxs[b].pos, _vtxs[c].pos),
			norm(tri.norm),
			vtxid{a, b, c},
			vtxs(_vtxs){
			m_assert(!tri.e1.is_zero());
		}

		void add_adj_face(Face* f)
		{ adj_faces.push_back(f); }

		Vec get_norm(int i) const {
			if (i == 0)
				return vtxs[get<0>(vtxid)].norm;
			else if (i == 1)
				return vtxs[get<1>(vtxid)].norm;
			else
				return vtxs[get<2>(vtxid)].norm;
		}

		shared_ptr<Trace> get_trace(const Ray& ray) const;

		AABB get_aabb() const;

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

		shared_ptr<Surface> get_property() const
		{ return face.host->texture->get_property(); }
};
