// File: light.hh
// Date: Fri Sep 20 19:28:59 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/geometry.hh"
#include "renderable/sphere.hh"

#include <cmath>

class Light : public Sphere {
	private:
		Surface surf;

	public:
		Color color;
		real_t intensity;

		Light(const PureSphere& _sphere, const Color& col, real_t _intense):
			Sphere(_sphere,
					make_shared<HomoTexture>(
						Surface(0, 0, 0, Color::NONE, 0, col * intensity))),
			color(col), intensity(_intense) { };

		Light(const Vec& src, const Color& col, real_t _intense):
			Light(PureSphere(src, 0.1), col, _intense) {};

		Vec get_src() const { return sphere.center; }
};
