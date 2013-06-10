// File: plane.hh
// Date: Mon Jun 10 11:50:10 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <limits>
#include "renderable/renderable.hh"
#include "geometry/infplane.hh"

class Plane : public RenderAble {
	public:
		bool infinity = true;
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

		Plane(const InfPlane& _plane, const std::shared_ptr<Texture>& _texture):
			Plane(_plane)
		{ texture = _texture; }

		void set_finite(real_t radius, Vec center);

		std::shared_ptr<Trace> get_trace(const Ray& ray) const;

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
		std::shared_ptr<const Surface> transform_get_property();

	public:
		PlaneTrace(const Plane& _plane, const Ray& _ray):
			Trace(&_plane, _ray), plane(_plane){};

		bool intersect();

		real_t intersection_dist();

		Vec normal();
};

