// File: renderable.hh
// Date: Sun Apr 07 18:55:13 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

#include <memory>
#include "geometry/ray.hh"
#include "material/texture.hh"

class Trace;
class RenderAble;

class RenderAble {
	private:

	public:
		virtual ~RenderAble(){}

		bool infinity = false;
		std::shared_ptr<Texture> texture;

		void set_texture(const std::shared_ptr<Texture>& m_texture)
		{ texture = m_texture; }		// XXX is this ok?

		virtual std::shared_ptr<Trace> get_trace(const Ray& ray) const = 0;
		// judge visibility and return ptr if visible
};

// a combination of renderable object and a ray, to integrate some calculations
class Trace {
	protected:
		const RenderAble* obj;
		const Ray& ray;
		bool toward = true;
		real_t inter_dist = std::numeric_limits<real_t>::infinity();

		virtual	std::shared_ptr<const Surface> transform_get_property() = 0;


	public:
		Trace(const RenderAble* m_obj, const Ray& m_ray):
			obj(m_obj), ray(m_ray){}

		// forbid copy of a Trace
		Trace(const Trace&) = delete;
		Trace& operator = (const Trace&) = delete;

		virtual ~Trace(){ }

		const RenderAble* get_obj()
		{ return obj; }

		virtual inline Vec intersection_point() {
			m_assert(inter_dist > 0);
			return ray.get_dist(inter_dist);
		}

		virtual bool intersect() = 0;

		virtual real_t intersection_dist() = 0;

		virtual Vec normal() = 0;
		// return Vec::zero if no normal exists

		std::shared_ptr<const Surface> get_property() {
			std::shared_ptr<const Surface> ret = obj->texture->get_property();
			if (ret)
				return std::move(ret);
			return std::move(transform_get_property());		// is this working?
		}

		virtual bool contain() const
		{ return false; }
};


