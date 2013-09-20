// File: renderable.hh
// Date: Sat Sep 21 01:36:12 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

#include <memory>
#include "geometry/ray.hh"
#include "geometry/aabb.hh"
#include "material/texture.hh"
using std::shared_ptr;
using std::make_shared;

class Trace;
class Renderable;
typedef shared_ptr<Renderable> rdptr;

class Renderable {
	private:
		shared_ptr<Texture> texture = nullptr;

	public:
		virtual ~Renderable(){};

		Renderable(){ }

		Renderable(const shared_ptr<Texture>& _texture):
			texture(_texture) { }

		virtual bool infinity() const
		{ return false; }

		virtual bool have_inside() const
		{ return true; }

		void set_texture(const shared_ptr<Texture>& _texture)
		{ texture = _texture; }

		shared_ptr<Texture> get_texture() const
		{ return texture; }

		virtual shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist) const = 0;
		// judge visibility and return ptr if visible

		/*
		 *shared_ptr<Trace> get_trace(const Ray& ray) const
		 *{ return get_trace(ray, -1); }
		 */

		virtual AABB get_aabb() const = 0;
};

// a combination of renderable object and a ray, to integrate some calculations
class Trace {
	protected:
		const Ray& ray;			// ray must have longer lifecycle than any trace !

		// two useful internal members
		bool toward = true;
		real_t inter_dist = std::numeric_limits<real_t>::infinity();

		virtual	shared_ptr<Surface> transform_get_property() const = 0;

	public:
		Trace(const Ray& _ray): ray(_ray){}

		// we have `const Ray&, const Renderable*` member, so forbid copy of a Trace
		Trace(const Trace&) = delete;
		Trace& operator = (const Trace&) = delete;

		virtual ~Trace(){ }

		virtual const Renderable* get_obj() const = 0;

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

		virtual bool contain() const
		{ return false; }

		virtual shared_ptr<Surface> get_property() const {
			shared_ptr<Surface> ret = get_obj()->get_texture()->get_property();
			if (ret) return move(ret);
			return transform_get_property();		// is this working?
		}
};


