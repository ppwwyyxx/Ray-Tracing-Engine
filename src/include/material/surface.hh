// File: surface.hh
// Date: Fri Jun 21 00:26:56 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "color.hh"

class Surface {
	// http://en.wikipedia.org/wiki/Phong_reflection_model
	public:
		real_t transparency = 0,
			   shininess = 0,
			   ambient = 0;
		Color diffuse = Color::BLACK,
			  specular = Color::BLACK;

		Surface(){}

		Surface(real_t _transparency, real_t _shininess,
				real_t _ambient, const Color& _diffuse, const Color& _specular):
			transparency(_transparency), shininess(_shininess),
			ambient(_ambient), diffuse(_diffuse), specular(_specular) {}

		static const Surface WHITE_REFL, BLACK_REFL, BLUE_REFL, CYAN_REFL;
};

