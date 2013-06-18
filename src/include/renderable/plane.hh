// File: plane.hh
// Date: Tue Jun 18 17:14:29 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <limits>
#include "renderable/renderable.hh"
#include "geometry/infplane.hh"
using std::shared_ptr;

class Plane : public RenderAble {
	public:
		real_t radius = 0;
		Vec surfdir = Vec::infinity();
		Vec center = Vec::infinity();

		const InfPlane plane;

		Plane(const InfPlane& _plane):
			plane(_plane) {
			surfdir = surf_dir();
			center = surf_point();
		}
		// can later set_texture

		Plane(const InfPlane& _plane, const shared_ptr<Texture>& _texture):
			Plane(_plane)
		{ texture = _texture; infinity = true; have_inside = false; }

		void set_finite(real_t radius, Vec center);

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist = -1) const;

		AABB get_aabb() const;

	protected:

		friend class PlaneTrace;

		Vec surf_dir() const;

		inline Vec surf_point() const
		{ return plane.norm * plane.offset; }

};

class PlaneTrace : public Trace {
	private:
		const Plane& plane;
		real_t dist_to_plane = std::numeric_limits<real_t>::infinity();
		real_t dir_dot_norm = std::numeric_limits<real_t>::infinity();
		shared_ptr<Surface> transform_get_property() const;

	public:
		PlaneTrace(const Plane& _plane, const Ray& _ray):
			Trace(&_plane, _ray), plane(_plane){};

		bool intersect();

		real_t intersection_dist();

		Vec normal();
};

