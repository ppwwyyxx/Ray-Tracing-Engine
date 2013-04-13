// File: viewer.hh
// Date: Tue Apr 09 10:18:58 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "view.hh"
#include "render/cvrender.hh"

class Viewer {
	protected:
		View& v;
		const Geometry& geo;
	public:
		Viewer(View& m_v):
			v(m_v), geo(m_v.get_geo()){}

		virtual ~Viewer(){}

		Viewer(const Viewer&) = delete;

		Viewer & operator = (const Viewer&) = delete;

		virtual void view() = 0;

};

class CVViewer : public Viewer {
	private:
		CVRender r;
	public:
		CVViewer(View& m_v):
			Viewer(m_v), r(v.get_geo()) { }

		void view();

};
