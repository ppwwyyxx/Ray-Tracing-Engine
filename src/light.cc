// File: light.cc
// Date: Fri Jun 14 20:32:35 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "renderable/light.hh"
using namespace std;

shared_ptr<Trace> Light::get_trace(const Ray& ray) const {
	shared_ptr<Trace> ret(new LightTrace(*this, ray));		// share problem?
	if (ret->intersect())
		return ret;
	return nullptr;
};

bool LightTrace::intersect()
{ return toward && (ray.sqrdistance(light.src) < light.size * light.size); }

real_t LightTrace::intersection_dist() {
	inter_dist = (light.src - ray.orig).mod();
	return inter_dist;
}

Vec LightTrace::normal()
{ return Vector::zero(); }

shared_ptr<Surface> LightTrace::transform_get_property() const {
	return shared_ptr<Surface>(new Surface(0, 0,
				Color::BLACK, light.color * light.intensity, Color::BLACK));
}
