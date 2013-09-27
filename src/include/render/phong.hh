//File: phong.hh
//Date: Fri Sep 27 19:25:14 2013 +0800
//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "render/space.hh"

class Phong : public Space {
	private:
		Color ambient;		// ambient in this space

		static Color blend(const Color& amb,
				const Color& phong, const Color& refl, const Color& transm);

		Color do_trace(const Ray& ray, real_t dist = 0, int depth = 0) const;

	public:
		int max_depth = MAX_PHONG_DEPTH;

		void finish() override;

		Color trace(const Ray& ray) const override
		{ return do_trace(ray); }

};
