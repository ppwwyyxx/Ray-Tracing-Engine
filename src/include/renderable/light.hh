// File: light.hh
// Date: Sun Jun 23 01:25:47 2013 +0800
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
			Sphere(_sphere),
			color(col), intensity(_intense) {
			texture = make_shared<HomoTexture>(Surface::LIGHT);
		};

		Light(const Vec& src, const Color& col, real_t _intense):
			Light(PureSphere(src, 0.1), col, _intense) {};

		Vec get_src() const { return sphere.center; }
};
