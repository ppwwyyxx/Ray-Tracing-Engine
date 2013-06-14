// File: aabb.hh
// Date: Fri Jun 14 21:33:30 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <utility>
#include <limits>

#include "geometry/ray.hh"

class AAPlane {
	public:
		AXIS axis;
		real_t pos;
};

// axis-aligned bounding box
class AABB {
	public:
		Vec min = Vec::max(),
			max = -Vec::max();

		AABB(const Vec& v1, const Vec& v2):
			min(Vec(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z))),
			max(Vec(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z))) {}

		void set(const Vec& vmin, const Vec& vmax) { min = vmin, max = vmax; }
		Vec size() const { return max - min; }
		bool empty() const { return (min.x > max.x || min.y > max.y || min.z > max.z); }
		bool contain(const Vec& p) const { return (max - p).is_positive() && (p - min).is_positive(); }

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

		std::pair<AABB, AABB> cut(const AAPlane& pl) const {
			AABB l = *this, r = *this;
			m_assert(BETW(pl.pos, min[pl.axis], max[pl.axis]));
			l.max[pl.axis] = pl.pos;
			r.min[pl.axis] = pl.pos;
			return std::make_pair(l, r);
		}

		// An efficient and robust ray-box intersection algorithm
		// Williams, etc. SIGGRAPH 2005
		bool intersect(const Ray& ray, real_t &mind, real_t &maxd) {
			if (empty())
				return false;
			Vec inv(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
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

};

