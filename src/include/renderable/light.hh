// File: light.hh
// Date: Fri Jun 07 21:50:30 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/geometry.hh"
#include "renderable/renderable.hh"
#include "geometry/ray.hh"
#include "color.hh"

class Light : public RenderAble {

	public:
		Light(const Vec& m_src, const Color& col, real_t intense):
			src(m_src), color(col), intensity(intense){};

		Vec src;
		Color color;
		real_t intensity;
		real_t size = EPS;

		std::shared_ptr<Trace> get_trace(const Ray& ray) const;

	protected:
		friend class LightTrace;

};

class LightTrace : public Trace {
	private:
		const Light& light;
		std::shared_ptr<const Surface> transform_get_property();
	public:
		LightTrace(const Light& m_light, const Ray& m_ray):
			Trace(&m_light, m_ray), light(m_light){
			toward = (ray.dir.dot(light.src - ray.orig) > 0);
		};

		real_t intersection_dist();

		Vec normal();

		bool intersect();

};


