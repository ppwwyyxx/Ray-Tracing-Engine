// File: sphere.hh
// Date: Sun Jun 23 17:35:28 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "geometry/geometry.hh"

class PureSphere {
	public:
		Vec center;
		real_t r;

		PureSphere(const Vec& m_center, real_t m_r):
			center(m_center), r(m_r) { }

		virtual ~PureSphere(){}

		inline real_t dist(const Vec& p) const
		{ return sqrt(distsqr(p)); }

		inline real_t distsqr(const Vec& p) const
		{ return (center - p).sqr(); }

		inline bool contain(const Vec& p) const
		{ return distsqr(p) < r * r; }

		static const PureSphere TestSphere;

};
