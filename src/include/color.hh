// File: color.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <iostream>
#include "geometry/geometry.hh"
#include "lib/debugutils.hh"

#include <cmath>

class Color {
	public:
		real_t r = 0, g = 0, b = 0;

		Color():
			Color(EPS, EPS, EPS) { }

		explicit Color(real_t _r, real_t _g, real_t _b):
			r(_r), g(_g), b(_b) {}

		explicit Color(const Vec& v):
			r(v.x), g(v.y), b(v.z) {}

		static constexpr real_t C_EPS = 1e-4;

		bool black() const
		{ return fabs(r) < C_EPS && fabs(g) < C_EPS && fabs(b) < C_EPS; }

		bool valid() const {
			return (BETW(r, 0, 1 + EPS)
					&& BETW(g, 0, 1 + EPS)
					&& BETW(b, 0, 1 + EPS));
		}

		void check() const {
			if (!valid()) {
				std::cout << *this << std::endl;
				m_assert(false);
			}
		}

		void normalize() {
			real_t max = get_max();
			if (max > 1) {
				r /= max;
				g /= max;
				b /= max;
			}
		}

		Color operator + (const Color &v) const
		{ return Color(r + v.r, g + v.g, b + v.b); }

		Color& operator += (const Color &v)
		{ r += v.r; g += v.g; b += v.b; return *this; }

		Color operator - (const Color &v) const
		{ return Color(r - v.r, g - v.g, b - v.b); }

		Color operator * (real_t p) const
		{ return Color(r * p, g * p, b * p); }

		Color& operator *= (real_t p)
		{ r *= p; g *= p; b *= p; return *this; }

		Color operator * (const Color& c) const
		{ return Color(r * c.r, g * c.g, b * c.b); }

		Color operator / (real_t p) const
		{ return *this * (1.0 / p); }

		inline real_t get_max()
		{ return std::max(r, std::max(g, b)); }

		friend std::ostream & operator << (std::ostream &os, const Color& vec)
		{ return os << vec.r << " " << vec.g << " " << vec.b;}

		static const Color NONE, WHITE, BLACK, RED, BLUE, GREEN, YELLOW, MAGNETA, CYAN;

};

