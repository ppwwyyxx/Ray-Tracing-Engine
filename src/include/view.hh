// File: view.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <memory>
#include "geometry/geometry.hh"
#include "render/space.hh"
using namespace std;

class View {
	private:
		const Geometry geo;

		inline void normalize_dir_vector()
		{ dir_w.normalize(); dir_h.normalize(); }

		inline void resume_dir_vector() {
			//  we should have:  |dir_w| * geo.w  == size
			//  as well as:		 |dir_w| == |dir_h|
			dir_w = dir_w.get_normalized() * (size / geo.w);
			dir_h = dir_h.get_normalized() * (size / geo.w);
		}

		const Space* sp;

	public:
		Vec view_point;
		Vec mid;
		real_t size;		// length the img cover in the scene
		Vec dir_w, dir_h;

		Vec origin_norm;		// the initial view

		bool use_dof = false;
		bool use_bended_screen = false;

		View(const Space* _sp, const Vec& _view_point,
				const Vec& _mid, real_t w, const Geometry& _geo);

		void zoom(real_t ratio);	// r > 1: zoom in

		void twist(int angle); // -180 ~ 180

		void rotate(int angle); // -180 ~ 180

		void orbit(int angle); // -180 ~ 180

		void shift(real_t dist, bool horiz);

		void move_screen(real_t dist);

		Color render(int i, int j) const;	// i row j column
		Color render_bended(int i, int j) const;

		Color render_antialias(const Vec& dest, int sample) const; // render with n^2 sample at each pixel

		Color render_dof(const Vec& dest) const;

		const Geometry& get_geo() const
		{ return geo; }

};

