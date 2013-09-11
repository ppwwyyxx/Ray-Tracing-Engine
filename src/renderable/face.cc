// File: face.cc
// Date: Wed Sep 11 20:12:04 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "renderable/mesh.hh"
#include "lib/debugutils.hh"
using namespace std;

real_t Triangle::get_intersect(const Ray& ray, real_t& gx, real_t& gy) const {
	// Ray-triangle intersection - Brian Curless
	//  -1 means no intersect
	// Return distance and barycentric coordinates for smoothing
	// code is motivated by http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
	/*
	 *print_debug("norm: %lf, %lf, %lf", norm.x, norm.y, norm.z);
	 *cout << v << " " << e1 << " "    << e2 << endl;
	 *cout << endl;
	 */
	if (fabs(ray.dir.dot(norm)) < EPS)		// parallel
		return -1;

	real_t inv = 1.0 / ray.dir.dot(norm);

	Vec line(ray.orig, v),
		line_r(ray.dir.cross(line));
	real_t dist = line.dot(norm) * inv;
	if (dist < 0) return -1;

	gx = -line_r.dot(e2) * inv;
	if (gx < -EPS || gx > 1 + EPS) return -1;
	gy = line_r.dot(e1) * inv;
	if (gy < -EPS || gx + gy > 1 + EPS) return -1;
	// now definitely inside triangle

	m_assert((ray.get_dist(dist) - (v * (1 - gx - gy) + (v + e1) * gx + (v + e2) * gy)).sqr() < EPS);	 // check coordinates
	return dist;
}

shared_ptr<Trace> Face::get_trace(const Ray& ray, real_t dist) const {
	shared_ptr<Trace> ret = shared_ptr<FaceTrace>(new FaceTrace(*this, ray));
	if (ret->intersect()) {
		if (dist == -1 || ret->intersection_dist() < dist)
			return ret;
	}
	return nullptr;
}

AABB Face::get_aabb() const {
	AABB ret;
	Vec eps = Vec::eps() * 2;		// * 2 two prevent cut on border
	ret.update_min(tri.v - eps);
	ret.update_max(tri.v + eps);
	Vec now = tri.get(0);
	ret.update_min(now - eps);
	ret.update_max(now + eps);
	now = tri.get(1);
	ret.update_min(now - eps);
	ret.update_max(now + eps);
	return ret;
}

Vec Face::get_norm(int i) const
{ return host->vtxs[vtxid[i]].norm; }

Vec Face::get_smooth_norm(real_t gx, real_t gy) const {
	// e1 : gx, e2: gy
	Vec ret;
	ret = ret + get_norm(0) * (1 - gx - gy) + get_norm(1) * gx + get_norm(2) * gy;
	return ret.get_normalized();
}

shared_ptr<Surface> FaceTrace::get_property() const
{ return face.host->texture->get_property(); }

shared_ptr<Surface> FaceTrace::transform_get_property() const {
	m_assert(false);		// XXX don't support texture on mesh
	real_t x = gx / 0.1, y = gy / 0.1;
	return face.texture->get_property(x, y);
}

bool FaceTrace::intersect() {
	inter_dist = face.tri.get_intersect(ray, gx, gy);
	if (inter_dist <= 0) return false;
	return true;
}

real_t FaceTrace::intersection_dist() { return inter_dist; }

Vec FaceTrace::normal() {		// norm to the ray side
	Vec ret;
	if (face.host->smooth)
		ret = face.get_smooth_norm(gx, gy);
	else
		ret = face.norm.get_normalized();

	if (Vec(face.tri.v, ray.orig).dot(ret) < 0)
		return -ret;
	return ret;
}
