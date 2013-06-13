// File: aabb.hh
// Date: Thu Jun 13 12:59:05 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once


#include <limits>

#include "geometry/ray.hh"

// axis-aligned bounding box
class AABB {
	public:
		AABB(){}

		AABB(const Vec& v1, const Vec& v2):
			min(Vec(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z))),
			max(Vec(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z))) {}

		void set(const Vec& vmin, const Vec& vmax)
		{ min = vmin, max = vmax; }

		Vec size() const
		{ return max - min; }

		bool empty()
		{ return (min.x > max.x || min.y > max.y || min.z > max.z); }

		void update(const AABB& b) {
			if (empty()) *this = b;
			else {
				min.update_min(b.min);
				max.update_max(b.max);
			}
		}

		void update(const Vec& v) {
			if (empty()) set(v, v);
			else {
				min.update_min(v);
				max.update_max(v);
			}
		}

		bool contain(const Vec& p) const
		{ return (max - p).positive() && (p - min).positive(); }

		// paper
		// An efficient and robust ray-box intersection algorithm
		// Williams, etc. SIGGRAPH 2005
		bool intersect(const Ray& ray, real_t &mind, real_t &maxd) {
			if (empty())
				return false;
			Vec inv(1 / ray.dir.x, 1 / ray.dir.y, 1 / ray.dir.z);
			real_t t_min = std::numeric_limits<real_t>::max();
			real_t t_max = -t_min;

#define UPDATE(t)\
			do {\
				bool sign = inv.t < 0;\
				real_t tmp_min = ((sign ? max : min).t - ray.orig.t) * inv.t;\
				real_t tmp_max = ((sign ? min : max).t - ray.orig.t) * inv.t;\
				update_min(t_min, tmp_min);\
				update_max(t_max, tmp_max);\
				if (t_min > t_max)\
					return false;\
			} while (0)

			UPDATE(x); UPDATE(y); UPDATE(z);
#undef UPDATE
			if (mind < 0)
				return false;

			mind = t_min, maxd = t_max;
			return true;
		}

	private:
		Vec min = Vec::max(),
			max = -Vec::max();
};

