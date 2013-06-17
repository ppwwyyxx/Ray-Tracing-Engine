// File: aabb.hh
// Date: Mon Jun 17 15:40:05 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <utility>
#include <iostream>
#include <limits>

#include "geometry/ray.hh"

class AAPlane {
	public:
		AXIS axis;
		real_t pos;

		AAPlane():
			axis(ERROR) {}
		AAPlane(int _axis, real_t _pos):
			axis(static_cast<AXIS>(_axis)), pos(_pos)
		{}
};

// axis-aligned bounding box
class AABB {
	public:
		Vec min = Vec::max(),
			max = -Vec::max();

		AABB(){}

		AABB(const Vec& _min, const Vec& _max):
			min(_min), max(_max) {}

		void set(const Vec& vmin, const Vec& vmax) { min = vmin, max = vmax; }
		Vec size() const { return max - min; }
		bool empty() const { return (min.x >= max.x || min.y >= max.y || min.z >= max.z); }
		real_t area() const { return (max - min).area(); }
		bool contain(const Vec& p) const { return p < max && min < p; }
		// override >
		//
		friend std::ostream& operator << (std::ostream & os, const AABB& b)
		{ return os << "min: " << b.min << " , max:" << b.max; }

		bool intersect(const AABB& b) const {
			Vec mid2 = (min + max),
				bmid2 = (b.min + b.max);
			if ((mid2 - bmid2).abs() < (max - min + b.max - b.min)) return true;
			return false;
		}

		static AABB get_inf() {
			AABB ret;
			swap(ret.min, ret.max);
			return ret;
		}

		void update(const AABB& b) {
			min.update_min(b.min);
			max.update_max(b.max);
		}

		inline void update(const Vec& v) {
			update_min(v);
			update_max(v);
		}

		inline void update_min(const Vec& v) {min.update_min(v); }
		inline void update_max(const Vec& v) {max.update_max(v); }

		std::pair<AABB, AABB> cut(const AAPlane& pl) const {
			AABB l = *this, r = *this;
			if (!BETW(pl.pos, min[pl.axis], max[pl.axis]))
				throw "Outside";
			l.max[pl.axis] = pl.pos + EPS;		// to loose
			r.min[pl.axis] = pl.pos - EPS;
			return std::make_pair(l, r);
		}

		// An efficient and robust ray-box intersection algorithm
		// Williams, etc. SIGGRAPH 2005
		// loose
		bool intersect(const Ray& ray, real_t &mind, bool &inside) {
			if (empty())
				return false;
			Vec inv(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
			real_t t_max = std::numeric_limits<real_t>::max(),
				   t_min = -t_max;

#define UPDATE(t) \
			do { \
				if (ray.dir.t > EPS) { \
					bool sign = (inv.t < 0); \
					real_t tmp_min = ((sign ? max : min).t - ray.orig.t) * inv.t; \
					real_t tmp_max = ((sign ? min : max).t - ray.orig.t) * inv.t; \
					::update_max(t_min, tmp_min); \
					::update_min(t_max, tmp_max); \
					if (t_min + EPS > t_max) return false; \
				} \
			} while (0)
			UPDATE(x); UPDATE(y); UPDATE(z);
#undef UPDATE

			if (t_max < 0) return false;

			if (t_min < 0) mind = t_max, inside = true;
			else mind = t_min, inside = false;
			return true;
		}

};

