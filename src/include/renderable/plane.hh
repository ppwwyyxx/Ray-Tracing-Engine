// File: plane.hh
// Date: Sun Apr 07 19:21:15 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <limits>
#include "renderable/renderable.hh"
#include "geometry/infplane.hh"

class Plane : public RenderAble {
	public:
		bool infinity = true;
		Vec surfdir = Vec::infinity();
		Vec surfp = Vec::infinity();

		const InfPlane& plane;

		Plane(const InfPlane& m_plane):
			plane(m_plane) {
			surfdir = surf_dir();
			surfp = surf_point();
		}

		Plane(const InfPlane& m_plane, const std::shared_ptr<Texture>& m_texture):
			Plane(m_plane)
		{ texture = m_texture; }

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
		PlaneTrace(const Plane& m_plane, const Ray& m_ray):
			Trace(&m_plane, m_ray), plane(m_plane){};

		bool intersect();

		real_t intersection_dist();

		Vec normal();
};

