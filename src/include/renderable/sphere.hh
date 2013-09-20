// File: sphere.hh
// Date: Sat Sep 21 01:55:34 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/sphere.hh"
#include "renderable/renderable.hh"

class Sphere : public Renderable {
	public:
		Vec north = Vec(0, 0, 1);

		const PureSphere sphere;

		Sphere(const PureSphere& _sphere):
			sphere(_sphere) {
			// calculate a new north
		}

		Sphere(const PureSphere &_sphere, std::shared_ptr<Texture> _texture):
			Renderable(_texture), sphere(_sphere) {}

		std::shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist) const override;

		AABB get_aabb() const override;

	protected:
		friend class SphereTrace;

};

class SphereTrace : public Trace {
	private:
		const Sphere& sphere;

		std::shared_ptr<Surface> transform_get_property() const override;

		mutable Vec proj = Vec::infinity();
		mutable Vec inter_point = Vec::infinity();
		mutable real_t sqrdiff = std::numeric_limits<real_t>::infinity();
		bool inside = false;

	public:
		SphereTrace(const Sphere& _sphere, const Ray& _ray):
			Trace(_ray), sphere(_sphere) {
			toward = (ray.dir.dot(sphere.sphere.center - ray.orig) > 0);
			inside = sphere.sphere.contain(ray.orig);
		};

		const Renderable* get_obj() const override
		{ return &sphere; }

		bool intersect() const override;

		Vec intersection_point() const override;

		real_t intersection_dist() const override;

		Vec normal() const override;

		real_t get_forward_density() const override;

		bool contain() const override
		{ return inside; }

};
