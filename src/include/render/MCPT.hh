//File: MCPT.hh
//Date: Fri Sep 27 19:40:03 2013 +0800
//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "render/space.hh"

// Monte Carlo Path Tracing
class MCPT : public Space {
	protected:
		Color do_trace(const Ray& ray, int depth = 0) const;

	public:
		int max_depth = MAX_MCPT_DEPTH;

		Color trace(const Ray& ray) const override
		{ return do_trace(ray); }

};
