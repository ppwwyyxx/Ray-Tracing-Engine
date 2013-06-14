// File: color.hh
// Date: Fri Jun 14 22:07:26 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/geometry.hh"

#include <cmath>

class Color: public Vector {
	public:
		Color(real_t r = 0, real_t g = 0, real_t b = 0):
			Vector(r, g, b){}

		static constexpr real_t C_EPS = 1e-4;

		bool black() const
		{ return is_zero(C_EPS); }

		void check() const {
			m_assert(x >= 0 && x <= 1);
			m_assert(y >= 0 && y <= 1);
			m_assert(z >= 0 && z <= 1);
		}

		void normalize();

		Color operator * (real_t p) const
		{ return Color(x * p, y * p, z * p); }

		Color operator * (const Vector& c) const
		{ return Color(x * c.x, y * c.y, z * c.z); }

		static const Color WHITE, BLACK, RED, BLUE;

};


class Light {

	public:
		Light(const Vec& m_src, const Color& col, real_t intense):
			src(m_src), color(col), intensity(intense){};

		Vec src;
		Color color;
		real_t intensity;
		real_t size = EPS;
};
