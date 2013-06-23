// File: infplane.hh
// Date: Sun Jun 23 20:03:06 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

#include "geometry/geometry.hh"
#include "geometry/ray.hh"

// geometry plane object
class InfPlane {
	public:
		Vec norm;
		real_t offset;

		InfPlane(const Vec& m_norm, real_t d, bool normalize = false):
			norm(m_norm), offset(d) {
				if (normalize)
					norm.normalize();
		}

		// construct from a point on the plane
		InfPlane(const Vec& m_norm, const Vec& p, bool normalize = false):
			norm(m_norm) {
				if (normalize)
					norm.normalize();
				offset = norm.dot(p);
			}

		// construct from three point
		InfPlane(const Vec& v1, const Vec& v2, const Vec& v3) {
			norm = (v2 - v1).cross(v3 - v1);
			norm.normalize();
			offset = norm.dot(v1);
		}

		// ax + by + cz + w == 0
		InfPlane(real_t a, real_t b, real_t c, real_t w) {
			norm = Vec(a, b, c);
			norm.normalize();
			offset = -w;
		}

		virtual ~InfPlane(){}

		inline real_t dist(const Vec& p) const
		{ return p.dot(norm) - offset; }

		inline bool in_half_space(const Vec& p) const
		{ return dist(p) >= EPS; }

		inline bool in_plane(const Vec& p) const
		{ return fabs(dist(p)) < EPS; }

		/*
		 *bool parallel(const Ray& ray) const
		 *{ return fabs(norm.dot(ray.dir)) < EPS; }
		 */

		static const InfPlane XYPLANE;
};

