// File: color.hh
// Date: Sun Jun 23 11:55:08 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <iostream>
#include "geometry/geometry.hh"
#include "lib/debugutils.hh"

#include <cmath>

class Color: public Vector {
	public:
		Color(real_t r = 0, real_t g = 0, real_t b = 0):
			Vector(r, g, b){}

		Color(const Vec& v):
			Vector(v.x, v.y, v.z){}

		static constexpr real_t C_EPS = 1e-4;

		bool black() const
		{ return is_zero(C_EPS); }

		void check() const {
			if (x < 0 || x > 1 || y < 0 || y > 1 || z < 0 || z > 1)
				std::cout << *this << std::endl;
			m_assert(x >= 0 && x <= 1 + EPS);
			m_assert(y >= 0 && y <= 1 + EPS);
			m_assert(z >= 0 && z <= 1 + EPS);
		}

		void normalize() {
			real_t max = get_max();
			if (max > 1) {
				x /= max;
				y /= max;
				z /= max;
			}
		}

		Color operator * (real_t p) const
		{ return Color(x * p, y * p, z * p); }

		Color operator * (const Vector& c) const
		{ return Color(x * c.x, y * c.y, z * c.z); }

		static const Color NONE, WHITE, BLACK, RED, BLUE, GREEN, YELLOW, MAGNETA, CYAN;

};

