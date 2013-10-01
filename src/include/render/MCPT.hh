//File: MCPT.hh

//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "render/space.hh"

// Monte Carlo Path Tracing
class MCPT : public Space {
	protected:
		Color do_trace(const Ray& ray, int depth = 0) const;

		virtual Color do_trace_adapter(const Ray& ray) const
		{ return do_trace(ray); }

		virtual Color diffuse(const IntersectInfo& info, const Ray& ray, int depth) const;

		virtual Color reflection(const IntersectInfo& info, const Ray& ray, int depth) const;

		virtual Color transmission(const IntersectInfo& info, const Ray& ray, int depth) const;

	public:
		int max_depth = MAX_MCPT_DEPTH;

		Color trace(const Ray& ray) const override;
};
