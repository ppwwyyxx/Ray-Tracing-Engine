// File: sphere.hh
// Date: Sat Sep 21 01:28:41 2013 +0800
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
		Vec proj = Vec::infinity();
		Vec inter_point = Vec::infinity();
		real_t sqrdiff = std::numeric_limits<real_t>::infinity();
		bool inside = false;

		std::shared_ptr<Surface> transform_get_property() const override;


	public:
		SphereTrace(const Sphere& _sphere, const Ray& _ray):
			Trace(_ray), sphere(_sphere) {
			toward = (ray.dir.dot(sphere.sphere.center - ray.orig) > 0);
			inside = sphere.sphere.contain(ray.orig);
		};

		const Renderable* get_obj() const override
		{ return &sphere; }

		bool intersect() override;

		real_t intersection_dist() override;

		Vec intersection_point() const override;

		Vec normal() override;

		real_t get_forward_density() const override;

		bool contain() const override
		{ return inside; }

};
