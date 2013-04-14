// File: plane.cc
// Date: Sun Apr 14 23:36:30 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "renderable/plane.hh"
using namespace std;

const InfPlane InfPlane::XYPLANE(Vec(1,0,0), Vec(0,1,0), Vec(1,1,0)),
	  InfPlane::YZPLANE(Vec(0,1,1), Vec(0,1,0), Vec(0,0,1));

shared_ptr<Trace> Plane::get_trace(const Ray& ray) const {
	shared_ptr<Trace> ret(new PlaneTrace(*this, ray));
	if (ret->intersect()) {
		return ret;
	}
	return nullptr;
}

Vec Plane::surf_dir() const {
	Vec ret(plane.norm.y, -plane.norm.x, 0);
	// possibily get a (0,0,0) !
	// but cannot be all zero
	if (ret == Vec::get_zero())
		ret = Vec(0, plane.norm.z, -plane.norm.y);
	m_assert(!(ret == Vec::get_zero()));
	ret.normalize();
	return ret;
}

bool PlaneTrace::intersect() {
	dist_to_plane = plane.plane.dist(ray.orig);
	if (fabs(dist_to_plane) < EPS) { // source on the plane
		return false;
}
	dir_dot_norm = plane.plane.norm.dot(ray.dir);
	if (fabs(dir_dot_norm) < EPS)  // parallel to plane
		return false;
	if ((dist_to_plane > 0) ^ (dir_dot_norm < 0))  // ray leaves plane
		return toward = false;
	return true;
}

real_t PlaneTrace::intersection_dist() {
	if (isfinite(inter_dist))
		return inter_dist;
	inter_dist = -dist_to_plane / dir_dot_norm;
	return inter_dist;
}

Vec PlaneTrace::normal() {	// norm to the ray side
	Vec ret = plane.plane.norm;
	if (plane.plane.in_half_space(ray.orig))
		return ret;
	return -ret;
}

shared_ptr<const Surface> PlaneTrace::transform_get_property() {
	Vec diff = intersection_point() - plane.surfp;
	real_t x = diff.dot(plane.surfdir);
	real_t y = diff.dot(plane.surfdir.cross(plane.plane.norm));
	return plane.texture->get_property(x, y);
}
