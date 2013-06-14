// File: sphere.cc
// Date: Fri Jun 14 10:58:00 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "renderable/sphere.hh"
using namespace std;

const PureSphere PureSphere::TestSphere(Vec(0, 0, 3), 3);

shared_ptr<Trace> Sphere::get_trace(const Ray& ray) const {
	shared_ptr<Trace> ret(new SphereTrace(*this, ray));
	if (ret->intersect())
		return  ret;
	return nullptr;
}


bool SphereTrace::intersect() {		// XXX orig in the center ? center on the line?
	if (!toward && !inside)	// ray leaves sphere
		return false;

	proj = ray.project(sphere.sphere.center);
	m_assert(isfinite(proj.x));
	sqrdiff = sqr(sphere.sphere.r) - (proj - sphere.sphere.center).sqr();
	if (!inside && (sqrdiff < 0))  // dist > r
		return false;
	return true;
}

real_t SphereTrace::intersection_dist() {
	if (!inside)
		inter_dist = (proj - ray.orig).mod() - sqrt(sqrdiff);
	else if (toward)
		inter_dist = (proj - ray.orig).mod() + sqrt(sqrdiff);
	else
		inter_dist = sqrt(sqrdiff) - (proj - ray.orig).mod();

	m_assert(isfinite(proj.x)); // XXX
	m_assert(isfinite(sqrdiff));
	m_assert(inter_dist > 0);

	inter_point = Trace::intersection_point();
	return inter_dist;
}

Vec SphereTrace::intersection_point() {
	m_assert(isfinite(inter_point.x));
	return inter_point;
}

Vec SphereTrace::normal() {
	Vec ret = inter_point - sphere.sphere.center;
	ret.normalize();
	if (!inside) return ret;
	else return -ret;
}

shared_ptr<Surface> SphereTrace::transform_get_property() const {
	m_assert(fabs(sphere.north.sqr() - 1) < EPS);
	Vec norm = (inter_point - sphere.sphere.center);
	Vec projxy = Vec(norm.x, norm.y, 0).get_normalized(),
		projyz = Vec(0, norm.y, norm.z).get_normalized();
	real_t arg1 = acos(projxy.dot(sphere.north)),		// 0 to pi
		   arg2 = acos(projyz.dot(sphere.north));

	return sphere.texture->get_property(arg1 * sphere.sphere.r, arg2 * sphere.sphere.r);
}

real_t SphereTrace::get_forward_density() const {
	if (inside) return AIR_REFRACTIVE_INDEX;
	else return DEFAULT_REFRACTIVE_INDEX;
}
