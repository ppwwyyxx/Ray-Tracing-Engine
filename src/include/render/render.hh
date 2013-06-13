// File: render.hh
// Date: Thu Jun 13 13:23:41 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

#include "common.hh"
#include "matrix.hh"
#include "color.hh"
#include "lib/imagereader.hh"

class RenderBase {
	public:
		RenderBase(const Geometry &m_g):
			geo(m_g){}

		virtual ~RenderBase(){};

		virtual void init() {}
		// execute before write

		virtual int finish() { return 0; }
		// execute after write

		void write(int x, int y, const Color &c) {
			m_assert(x >= 0 && x < geo.w && y >= 0 && y < geo.h);
			c.check();

			_write(x, y, c);
			render_cnt ++;
		}

		void write(Matrix<Color>& m) {
			for (int i = 0; i < geo.w; i ++)
				for (int j = 0; j < geo.h; j ++)
					write(i, j, m[j][i]);
		}

		void write(ImageReader* r) {
			m_assert((r->h == geo.h) && (r->w == geo.w));
			Color *dest = r->pixel;
			for (int i = 0; i < geo.h; i ++)
				for (int j = 0; j < geo.w; j ++) {
					write(j, i, *dest);
					dest ++;
				}
		}

		const Geometry& get_geo() const
		{ return geo; }

		int get_cnt() const
		{ return render_cnt; }

	private:
		int render_cnt = 0;

	protected:
		Geometry geo;
		virtual void _write(int x, int y, const Color &c) = 0;

};


