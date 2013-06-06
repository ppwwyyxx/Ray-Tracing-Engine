// File: surface.hh
// Date: Wed Jun 05 20:34:13 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "color.hh"

class Surface {
	// http://en.wikipedia.org/wiki/Phong_reflection_model
	public:
		real_t transparency = 0,
			   shininess = 0,
			   refractive = 0;
		Color diffuse = Color::BLACK,
			  ambient = Color::BLACK,
			  specular = Color::BLACK;

		Surface(){}

		Surface(real_t m_transparency, real_t m_shininess,
				const Color& m_diffuse, const Color& m_ambient,
				const Color& m_specular, real_t m_refractive = REFRACTIVE_INDEX):
			transparency(m_transparency), shininess(m_shininess),
			diffuse(m_diffuse), ambient(m_ambient), specular(m_specular),
			refractive(m_refractive){}

		static const Surface WHITE_REFL, BLACK_REFL, BLUE_REFL;
};

