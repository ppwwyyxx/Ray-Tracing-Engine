// File: main.cc
// Date: Wed Apr 10 19:06:56 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
#include "viewer.hh"
#include "image.hh"

using namespace std;

int main(int argc, char* argv[]) {
	int w, h;
	w = h = 500;
	Space s;
	Light l(Vec(5, -2, 15), Color(0.9, 1, 1), 1);
	Light lall(Vec(0, 0, 50), Color(1, 1, 1), 0.2);
	l.size = EPS;
	s.add_light(l);s.add_light(lall);

	shared_ptr<Texture> t1(new GridTexture(GridTexture::BLACK_WHITE));
	Plane plane1(InfPlane::XYPLANE, t1);
	s.add_obj(shared_ptr<RenderAble>(new Plane(plane1)));
	/*
	 *Plane plane2 = Plane::YZPLANE;
	 *s.add_obj(shared_ptr<RenderAble>(new Plane(plane2)));
	 */
	t1 = shared_ptr<Texture>(new HomoTexture(HomoTexture::BLUE));
	Sphere sphere(PureSphere::TestSphere, t1);
	s.add_obj(shared_ptr<RenderAble>(new Sphere(sphere)));

	View v(make_shared<Space>(s), Vec(0, -10, 5), Vec(0, 0, 5), 30, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}
