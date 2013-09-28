// File: light.hh
// Date: Sat Sep 28 18:53:37 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/geometry.hh"
#include "renderable/sphere.hh"

#include <cmath>

class Light : public Sphere {
	private:
		shared_ptr<Surface> surf;
		// surf must stay in the **same** place as long as this light is alive or **copied** !

	public:
		Color color;
		real_t intensity;

		Light(const PureSphere& _sphere, const Color& _col, real_t _intense):
			Sphere(_sphere), color(_col), intensity(_intense)
		{
			surf = make_shared<Surface>(0, 0, 0, Color::NONE, 0, _col * _intense);
			set_texture(make_shared<HomoTexture>(*surf));
		}

		Light(const Vec& src, const Color& _col, real_t _intense):
			Light(PureSphere(src, 0.1), _col, _intense) {};

		Vec get_src() const
		{ return sphere.center; }

		real_t get_size() const
		{ return sphere.r; }
};
