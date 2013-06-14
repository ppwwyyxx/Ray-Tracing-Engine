// File: light.hh
// Date: Fri Jun 14 22:05:14 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "geometry/geometry.hh"
#include "renderable/renderable.hh"
#include "geometry/ray.hh"
#include "color.hh"

class Light {

	public:
		Light(const Vec& m_src, const Color& col, real_t intense):
			src(m_src), color(col), intensity(intense){};

		Vec src;
		Color color;
		real_t intensity;
		real_t size = EPS;
};
