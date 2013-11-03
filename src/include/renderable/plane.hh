// File: plane.hh

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

		Plane(const InfPlane& _plane, const shared_ptr<Texture>& _texture):
			Renderable(_texture), plane(_plane) {
				surfdir = surf_dir();
				center = surf_point();
			}

		bool have_inside() const override
		{ return false; }

		bool infinity() const override
		{ return true; }

		shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist) const override;

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

		shared_ptr<Surface> transform_get_property() const override;

		mutable real_t dist_to_plane = std::numeric_limits<real_t>::infinity();
		mutable real_t dir_dot_norm = std::numeric_limits<real_t>::infinity();

	public:
		PlaneTrace(const Plane& _plane, const Ray& _ray):
			Trace(_ray), plane(_plane){};

		const Renderable* get_obj() const override
		{ return &plane; }

		bool intersect() const override;

		real_t intersection_dist() const override;

		Vec normal() const override;
};

