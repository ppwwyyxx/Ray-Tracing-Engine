// File: face.cc
// Date: Fri Jun 14 23:58:12 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "renderable/mesh.hh"
#include "lib/debugutils.hh"
using namespace std;

tuple<real_t, real_t, real_t> Triangle::get_intersect(const Ray& ray) const {
	// Ray-triangle intersection - Brian Curless
	// tuple.get(0)  == -1 means no intersect
	// Return distance and barycentric coordinates
	/*
	 *print_debug("norm: %lf, %lf, %lf", norm.x, norm.y, norm.z);
	 *cout << v << " " << e1 << " "    << e2 << endl;
	 *cout << endl;
	 */
	if (fabs(ray.dir.dot(norm.get_normalized())) < EPS)		// parallel
		return make_tuple(-1, 0, 0);

	real_t inv = 1.0 / ray.dir.dot(norm);

	Vec line(ray.orig, v),
		line_r(ray.dir.cross(line));
	real_t dist = line.dot(norm) * inv;
	if (dist < -EPS) return make_tuple(-1, 0, 0);

	real_t gx = -line_r.dot(e2) * inv;
	if (gx < -EPS || gx > 1 + EPS) return make_tuple(-1, 0, 0);
	real_t gy = line_r.dot(e1) * inv;
	if (gy < -EPS || gx + gy > 1 + EPS) return make_tuple(-1, 0, 0);
	// now definitely inside triangle

	m_assert((ray.get_dist(dist) - (v * (1 - gx - gy) + (v + e1) * gx +
			(v + e2) * gy)).sqr() < EPS);	 // check coordinates
	return make_tuple(dist, gx, gy);
}

shared_ptr<Trace> Face::get_trace(const Ray& ray) const {
	shared_ptr<Trace> ret(new FaceTrace(*this, ray));
	if (ret->intersect()) return ret;
	return nullptr;
}

AABB Face::get_aabb() const {
	AABB ret;
	ret.update(tri.v);
	ret.update(tri.get(0));
	ret.update(tri.get(1));
	return ret;
}

shared_ptr<Surface> FaceTrace::transform_get_property() const {
	real_t x = gx / 0.1, y = gy / 0.1;
	return face.texture->get_property(x, y);
}

bool FaceTrace::intersect() {
	tie(inter_dist, gx, gy) = face.tri.get_intersect(ray);
	if (inter_dist < 0) return false;
	return true;
}

real_t FaceTrace::intersection_dist()
{ return inter_dist; }

Vec FaceTrace::normal() {		// norm to the ray side
	Vec ret = face.norm.get_normalized();
	if (Vec(face.tri.v, ray.orig).dot(ret) < 0)
		return -ret;
	return ret;
}
