// File: light.hh
// Date: Sun Jun 23 00:32:34 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/geometry.hh"
#include "renderable/sphere.hh"

#include <cmath>

class Light : public Sphere {
	public:
		Color color;
		real_t intensity;

		Light(const PureSphere& _sphere, const Color& col, real_t _intense):
			Sphere(_sphere, make_shared<HomoTexture>(Surface::LIGHT)),
			color(col), intensity(_intense) { };

		Light(const Vec& src, const Color& col, real_t _intense):
			Light(PureSphere(src, 0.1), col, _intense) {};

		Vec get_src() const { return sphere.center; }
};
