// File: sphere.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "geometry/geometry.hh"

class PureSphere {
	public:
		Vec center;
		real_t r;

		PureSphere(const Vec& _center, real_t _r):
			center(_center), r(_r) { }

		virtual ~PureSphere(){}

		inline real_t dist(const Vec& p) const
		{ return sqrt(distsqr(p)); }

		inline real_t distsqr(const Vec& p) const
		{ return (center - p).sqr(); }

		inline bool contain(const Vec& p) const
		{ return distsqr(p) < r * r; }

		static const PureSphere TestSphere;

};
