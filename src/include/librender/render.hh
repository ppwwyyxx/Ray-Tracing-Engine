// File: render.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>


#pragma once

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
		}

		void write(Matrix<Color>& m) {
			REP(i, geo.w) REP(j, geo.h)
				write(i, j, m[j][i]);
		}

		void write(ImageReader* r) {
			m_assert((r->size.h == geo.h) && (r->size.w == geo.w));
			Color *dest = r->pixel;
			REP(i, geo.h) REP(j, geo.w) {
				write(j, i, *dest);
				dest ++;
			}
		}

		const Geometry& get_geo() const
		{ return geo; }

	protected:
		Geometry geo;
		virtual void _write(int x, int y, const Color &c) = 0;

};


