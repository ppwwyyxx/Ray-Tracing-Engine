// File: sphere.hh
// Date: Mon Jun 10 00:49:59 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/sphere.hh"
#include "renderable/renderable.hh"

class Sphere : public RenderAble {
	public:
		Vec north = Vec(0, 0, 1);

		PureSphere sphere;

		Sphere(const PureSphere& m_sphere):
			sphere(m_sphere) {
			// calculate a new north
		}

		Sphere(const PureSphere &m_sphere, std::shared_ptr<Texture> m_texture):
			Sphere(m_sphere)
		{ texture = m_texture;	}

		std::shared_ptr<Trace> get_trace(const Ray& ray) const;

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

		std::shared_ptr<const Surface> transform_get_property();


	public:
		SphereTrace(const Sphere& m_sphere, const Ray& m_ray):
			Trace(&m_sphere, m_ray), sphere(m_sphere) {
			toward = (ray.dir.dot(sphere.sphere.center - ray.orig) > 0) ;
			inside = sphere.sphere.contain(ray.orig);
		};

		bool intersect();

		real_t intersection_dist();

		Vec intersection_point();

		Vec normal();

		real_t get_forward_density() const;

		bool contain() const
		{ return inside; }

};
