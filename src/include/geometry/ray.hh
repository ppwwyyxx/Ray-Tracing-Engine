// File: ray.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <cmath>

#include "geometry/geometry.hh"

class Ray {
	public:
		bool debug = false;

		Vec orig, dir;

		real_t density;

		Ray(){}

		Ray(const Vec & _orig, const Vec& _dir, real_t _density = 1, bool normalize = false):
			orig(_orig), dir(_dir), density(_density) {
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

