// File: main.cc
// Date: Sun Jun 23 12:01:51 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include "viewer.hh"
#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
#include "renderable/mesh.hh"

using namespace std;
const string texture_fname = "../resource/texture.jpg";

void test_shadow() {
	int w = 500, h = 500;
	Space s; s.use_soft_shadow = true;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));

	shared_ptr<Texture> t2 = make_shared<HomoTexture>(HomoTexture::BLUE);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t2));
	s.finish();
	View v(s, Vec(-8.7, 4.58, 3.75), Vec(-0.30, -0.31, 1.5), 8, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

void dof_ball_scene() {
	int w = 500, h = 500;
	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(9, 2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(-9, 2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(-9, -2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(9, -2, 50), Color::WHITE, 6.0));

	Surface surf(0, 40, 0.5, Color::CYAN * 0.9, DEFAULT_SPECULAR);

	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	shared_ptr<Texture> tball(make_shared<HomoTexture>(surf));

	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));

	REP(i, 10) REP(j, 2) s.add_obj(make_shared<Sphere>(PureSphere(Vec(j * 6, 1, i * 3), 1), tball));
	s.finish();
	View v(s, Vec(11, -13.3, 39.75), Vec(5.4, -1, 22.8), 16, Geometry(w, h));
	v.use_dof = true;
	CVViewer viewer(v);
	viewer.view();
}

void generate_dof_video() {
	int w = 1000, h = 1000;
	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(9, 2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(-9, 2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(-9, -2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(9, -2, 50), Color::WHITE, 6.0));

	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));

	Surface surf(0, 40, 0.5, Color::CYAN * 0.9, DEFAULT_SPECULAR);
	shared_ptr<Texture> tball(make_shared<HomoTexture>(surf));

	REP(i, 10) REP(j, 2) s.add_obj(make_shared<Sphere>(PureSphere(Vec(j * 6, 1, i * 3), 1), tball));
	s.finish();
	View v(s, Vec(11, -13.3, 39.75), Vec(5.4, -1, 22.8), 16, Geometry(w, h));
	v.use_dof = true;


	v.move_screen(-3);
	REP(k, 400) {
		CVViewer viewer(v, "output/" + string_format("%03d", k) + ".png");
		printf("finish %d\n", k);
		v.move_screen(0.04);
	}
}

void test_kdtree() {
	int w = 500, h = 500;
	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 6.0));
	const char* fname = "../resource/teapot_nonorm.obj";
	Mesh mesh(fname, Vec(0, 0, 2), 5);

	mesh.smooth = true;
	mesh.set_texture(make_shared<HomoTexture>(HomoTexture::CYAN));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));
	shared_ptr<Texture> t1 = make_shared<GridTexture>(GridTexture::BLACK_WHITE);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, t1));
	s.finish();
	View v(s, Vec(0, 0, 12), Vec(0, 0, 2), 15, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

void test_simplify() {
	int w = 500, h = 500;
	const char* fname = "../resource/models/fixed.perfect.dragon.100K.0.07.obj";
	Mesh mesh(fname, Vec(0, 0, 2), 5);
	mesh.smooth = true;
	mesh.set_texture(make_shared<HomoTexture>(HomoTexture::CYAN));
	mesh.simplify(0.2);
	mesh.finish();

	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 6.0));
	s.add_obj(make_shared<Mesh>(mesh));
	s.finish();
	View v(s, Vec(0, 0, 12), Vec(0, 0, 2), 15, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

void ball() {
	int w = 500, h = 500;
	Space s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 7));

	shared_ptr<Texture> t2 = make_shared<HomoTexture>(HomoTexture::BLUE);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));//make_shared<Plane>(Plane())?
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t2));

	s.finish();
	View v(s, Vec(-5, -1, 4), Vec(0, 0, 2), 8, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

void global_illu() {
	int w = 500, h = 500;
	Space s;
	s.add_light(Light(PureSphere(Vec(+10, 10, 10), 4), Color::WHITE, 12));

	shared_ptr<Texture> t2 = make_shared<HomoTexture>(Surface::GOOD);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t2));

	const char* fname = "../resource/models/fixed.perfect.dragon.100K.0.07.obj";
	Mesh mesh(fname, Vec(-3, +5, 2), 5);
	mesh.smooth = true;
	mesh.set_texture(make_shared<HomoTexture>(HomoTexture::CYAN));
	mesh.simplify(0.4);
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	REP(i, 5) REP(j, 2) s.add_obj(make_shared<Sphere>(PureSphere(Vec(j * 3 + 3, i * 3 + 3, 1), 1), t2));

	s.finish();
	View v(s, Vec(-5.6, -1.6, 10.1), Vec(-0.8, 1.35, 2.5), 13, Geometry(w, h));
	v.use_global = true;
	CVViewer viewer(v, "output.png");

}

int main() {
	global_illu();
	return 0;
}

