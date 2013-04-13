// File: ray.hh
// Date: Sun Apr 07 17:57:24 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <cmath>

#include "geometry/geometry.hh"

class Ray {
	public:
		bool debug = false;

		Vec orig, dir;

		Ray(){}

		Ray(const Vec & m_orig, const Vec& m_dir, bool normalize = false):
			orig(m_orig), dir(m_dir) {
			if (normalize)
				dir.normalize();
		}

		virtual ~Ray(){}

		Vec get_dist(real_t d) const
		{ return orig + dir * d; }

		real_t distance(const Vec& p) const
		{ return sqrt(sqrdistance(p)); }

		real_t sqrdistance(const Vec& p) const
		{ return (project(p) - p).sqr(); }

		Vec project(const Vec& p) const {
			real_t t = (p - orig).dot(dir);
			return get_dist(t);
		}

		bool contains(const Vec& p) const
		{ return (fabs(sqrdistance(p)) < EPS) && ((p.x - orig.x) * (dir.x) >= -EPS); }

		friend std::ostream& operator << (std::ostream & os, const Ray& ray)
		{ return os << "orig: " << ray.orig << " , dir:" << ray.dir; }
};

