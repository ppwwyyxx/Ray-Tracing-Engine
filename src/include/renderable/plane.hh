// File: plane.hh
// Date: Fri Sep 20 19:30:25 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <limits>
#include "renderable/renderable.hh"
#include "geometry/infplane.hh"
using std::shared_ptr;

class Plane : public Renderable {
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
			Plane(_plane) {
				set_texture(_texture);
			}

		bool have_inside() const override
		{ return false; }

		bool infinity() const override
		{ return true; }

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist = -1) const override;

		AABB get_aabb() const override;

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

		bool intersect() override;

		real_t intersection_dist() override;

		Vec normal() override;
};

