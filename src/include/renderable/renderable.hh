// File: renderable.hh
// Date: Thu Jun 20 12:37:19 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

#include <memory>
#include "geometry/ray.hh"
#include "geometry/aabb.hh"
#include "material/texture.hh"
using std::shared_ptr;
using std::make_shared;

class Trace;
class RenderAble;
typedef shared_ptr<RenderAble> rdptr;

class RenderAble {
	private:

	public:
		virtual ~RenderAble(){}

		bool infinity = false;
		bool have_inside = true;
		shared_ptr<Texture> texture;

		void set_texture(const shared_ptr<Texture>& _texture)
		{ texture = _texture; }

		virtual shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist = -1) const = 0;
		// judge visibility and return ptr if visible

		virtual AABB get_aabb() const = 0;
};

// a combination of renderable object and a ray, to integrate some calculations
class Trace {
	protected:
		const RenderAble* obj;
		const Ray& ray;
		bool toward = true;
		real_t inter_dist = std::numeric_limits<real_t>::infinity();

		virtual	shared_ptr<Surface> transform_get_property() const = 0;

	public:
		Trace(const RenderAble* m_obj, const Ray& m_ray):
			obj(m_obj), ray(m_ray){}

		// forbid copy of a Trace
		Trace(const Trace&) = delete;
		Trace& operator = (const Trace&) = delete;

		virtual ~Trace(){ }

		const RenderAble* get_obj()
		{ return obj; }

		virtual Vec intersection_point() const {
			m_assert(isfinite(inter_dist) && inter_dist >= 0);
			return ray.get_dist(inter_dist);
		}

		virtual bool intersect() = 0;

		virtual real_t intersection_dist() = 0;

		virtual Vec normal() = 0;
		// return Vec::zero if no normal exists
		// return a `normalized` normal vector

		virtual real_t get_forward_density() const
		{ return ray.density; }

		virtual shared_ptr<Surface> get_property() const {
			shared_ptr<Surface> ret = obj->texture->get_property();
			if (ret) return move(ret);
			return transform_get_property();		// is this working?
		}

		virtual bool contain() const
		{ return false; }
};


