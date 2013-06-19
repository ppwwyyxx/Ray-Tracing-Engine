// File: main.cc
// Date: Wed Jun 19 14:08:54 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include "viewer.hh"
#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
#include "renderable/mesh.hh"


using namespace std;

void test_shadow() {
	int w = 500, h = 500;
	Space s;
	s.use_soft_shadow = true;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 2.0));

	shared_ptr<Texture> t2(new HomoTexture(HomoTexture::BLUE));
	shared_ptr<Texture> tpic(new ImgTexture("../res/texture.jpg", 100, 0.6));
	s.add_obj(new Plane(InfPlane::XYPLANE, tpic));
	Sphere sphere(PureSphere::TestSphere, t2);
	s.add_obj(new Sphere(sphere));
	View v(make_shared<Space>(s), Vec(-8.7, 4.58, 3.75), Vec(-0.30, -0.31, 1.5), 8, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

void ball_scene() {
	int w = 500, h = 500;
	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 2.0));
	//s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 2.0));
	s.add_light(Light(Vec(9, 2, 50), Color::WHITE, 2.0));
	s.add_light(Light(Vec(-9, 2, 50), Color::WHITE, 2.0));
	s.add_light(Light(Vec(-9, -2, 50), Color::WHITE, 2.0));
	s.add_light(Light(Vec(9, -2, 50), Color::WHITE, 2.0));

	shared_ptr<Texture> tpic(new ImgTexture("../res/texture.jpg", 80, 1));
	s.add_obj(new Plane(InfPlane::XYPLANE, tpic));

	shared_ptr<Texture> tball(new HomoTexture(Surface(0, 40, 0.5, Color::CYAN * 0.9, Color::WHITE * DEFAULT_SPECULAR)));

	REP(i, 10) REP(j, 2) s.add_obj(new Sphere(PureSphere(Vec(j * 6, 1, i * 3), 1), tball));
	View v(make_shared<Space>(s), Vec(11, -13.3, 39.75), Vec(5.4, -1, 22.8), 16, Geometry(w, h));
	v.use_dof = true;
	CVViewer viewer(v);
	viewer.view();
}

void generate_dof_video() {
	int w = 500, h = 500;
	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 2.0));
	s.add_light(Light(Vec(9, 2, 50), Color::WHITE, 2.0));
	s.add_light(Light(Vec(-9, 2, 50), Color::WHITE, 2.0));
	s.add_light(Light(Vec(-9, -2, 50), Color::WHITE, 2.0));
	s.add_light(Light(Vec(9, -2, 50), Color::WHITE, 2.0));

	shared_ptr<Texture> tpic(new ImgTexture("../res/texture.jpg", 80, 1));
	s.add_obj(new Plane(InfPlane::XYPLANE, tpic));

	shared_ptr<Texture> tball(new HomoTexture(Surface(0, 40, 0.5, Color::CYAN * 0.9, Color::WHITE * DEFAULT_SPECULAR)));

	REP(i, 10) REP(j, 2) s.add_obj(new Sphere(PureSphere(Vec(j * 6, 1, i * 3), 1), tball));
	View v(make_shared<Space>(s), Vec(11, -13.3, 39.75), Vec(5.4, -1, 22.8), 16, Geometry(w, h));
	v.use_dof = true;



	REP(k, 300) {
		CVViewer viewer(v, "output/" + string_format("%03d", k) + ".png");
		v.move_screen(0.05);
	}
}

int main(int argc, char* argv[]) {
	//ball_scene();
//	generate_dof_video();
//	return 0;
	int w, h;
	w = h = 500;
	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 2.0));
	s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 2.0));
	s.add_light(Light(Vec(-10, 0, 8), Color::WHITE, 2.0));
	s.add_light(Light(Vec(0, 0, 9), Color::WHITE, 1.5));


	shared_ptr<Texture> t1 = make_shared<GridTexture>(GridTexture::BLACK_WHITE);
	shared_ptr<Texture> t2 = make_shared<HomoTexture>(Surface::BLUE_REFL);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>("../res/texture.jpg", 100, 0.6);
	shared_ptr<Texture> tred = make_shared<HomoTexture>(Surface::RED);
	Plane plane1(InfPlane::XYPLANE, t1);

	const char* fname = "../res/models/fixed.perfect.dragon.100K.0.07.obj";
	Mesh mesh(fname, Vec(0, 0, 2), 5);
	mesh.smooth = false;
	mesh.set_texture(tred);
	mesh.finish_add();
	s.add_obj(make_shared<Mesh>(mesh));
	REP(i, 100) REP(j, 100) s.add_obj(new Sphere(PureSphere(Vec(i * 2, j * 2, 1), 0.5), t2));

	s.add_obj(make_shared<Plane>(plane1));
//	Sphere sphere(PureSphere(Vec(1, 0, 1), 0.5), t2);
//	Sphere sphere(PureSphere::TestSphere, t2);
//	s.add_obj(new Sphere(sphere));

//	View v(make_shared<Space>(s), Vec(0.2, 0, 12), Vec(0, 0, 2), 10, Geometry(w, h));
	View v(make_shared<Space>(s), Vec(-10.3, -34, 7.37), Vec(-1.47, 0, 1.23), 10, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

