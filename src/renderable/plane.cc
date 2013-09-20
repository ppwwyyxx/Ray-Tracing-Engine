// File: plane.cc
// Date: Fri Sep 20 19:32:47 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "renderable/plane.hh"
using namespace std;

shared_ptr<Trace> Plane::get_trace(const Ray& ray, real_t dist) const {
	shared_ptr<Trace> ret = make_shared<PlaneTrace>(*this, ray);
	if (ret->intersect()) {
		if (dist == -1 || ret->intersection_dist() < dist)
			return ret;
	}
	return nullptr;
}

AABB Plane::get_aabb() const { error_exit("shoudl not be here"); }

Vec Plane::surf_dir() const {
	Vec ret(plane.norm.y, -plane.norm.x, 0);
	// possibily get a (0,0,0) !
	// but cannot be all zero
	if (ret == Vec::zero())
		ret = Vec(0, plane.norm.z, -plane.norm.y);
	m_assert(!(ret == Vec::zero()));
	ret.normalize();
	return ret;
}

bool PlaneTrace::intersect() {
	dist_to_plane = plane.plane.dist(ray.orig);
	if (fabs(dist_to_plane) < EPS) // source on the plane
		return false;
	dir_dot_norm = plane.plane.norm.dot(ray.dir);
	if (fabs(dir_dot_norm) < EPS)  // parallel to plane
		return false;
	if ((dist_to_plane > 0) ^ (dir_dot_norm < 0))  // ray leaves plane
		return toward = false;

	if (plane.radius < EPS) 		// infinite
		return true;
	else {
		inter_dist = - dist_to_plane / dir_dot_norm;
		Vec inter_point = ray.get_dist(inter_dist);
		real_t dist = (inter_point - plane.center).mod();
		if (dist >= plane.radius)
			return false;
		else
			return true;
	}
}

real_t PlaneTrace::intersection_dist() {
	if (isfinite(inter_dist)) return inter_dist;
	inter_dist = -dist_to_plane / dir_dot_norm;
	return inter_dist;
}

Vec PlaneTrace::normal() {	// norm to the ray side
	Vec ret = plane.plane.norm;
	if (plane.plane.in_half_space(ray.orig))
		return ret;
	return -ret;
}

shared_ptr<Surface> PlaneTrace::transform_get_property() const {
	Vec diff = intersection_point() - plane.center;
	real_t x = diff.dot(plane.surfdir);
	real_t y = diff.dot(plane.surfdir.cross(plane.plane.norm));
	return plane.get_texture()->get_property(x, y);
}
