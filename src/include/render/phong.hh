//File: phong.hh

//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include "render/space.hh"

class DistRay : public Ray {
	public:
		real_t dist = 0;

		DistRay(const Ray& ray): Ray(ray), dist(0) { }

		DistRay(const Vec & _orig, const Vec& _dir, real_t _density = 1, bool normalize = false):
			Ray(_orig, _dir, _density, normalize), dist(0) { }
};

class Phong : public Space {
	private:
		Color ambient;		// ambient in this space

		static Color blend(const Color& amb,
				const Color& phong, const Color& refl, const Color& transm);

		Color do_trace(DistRay& ray, int depth = 0) const;

		Color reflection(const IntersectInfo& info, const DistRay& ray, int depth) const;

		Color transmission(const IntersectInfo& info, const DistRay& ray, int depth) const;

		Color phong_local(const IntersectInfo& info, const DistRay& ray) const;

	public:
		int max_depth = MAX_PHONG_DEPTH;

		void finish() override;

		Color trace(const Ray& ray) const override {
			DistRay r(ray);
			return do_trace(r);
		}

};
