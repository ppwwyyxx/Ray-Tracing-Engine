// File: sphere.hh
// Date: Tue Jun 18 19:44:00 2013 +0800
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
		{ return (center - p).mod(); }

		inline bool contain(const Vec& p) const
		{ return dist(p) < r; }

		static const PureSphere TestSphere;

};
