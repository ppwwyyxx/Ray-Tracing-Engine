// File: main.cc

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include <sys/types.h>
#include <dirent.h>
#include "viewer.hh"
#include "render/phong.hh"
#include "render/MCPT.hh"
#include "render/MCPT_EL.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
#include "renderable/mesh.hh"

using namespace std;
const string texture_fname = "../resource/ground.jpg";
const string watermelon_fname = "../resource/watermelon.jpg";

// Complicated Scene
void all(bool g) {
	int w = 1366, h = 768;
	w = 500, h = 350;
	Space* sp;
	if (g) sp = new MCPT_EL();
	else sp = new Phong();
	sp->add_light(Light(PureSphere(Vec(+10, 10, 30), 3), Color::WHITE, g ? 50 : 12));
	sp->add_light(Light(PureSphere(Vec(-10, -10, 30), 3), Color::WHITE, g ? 40 : 10));

	shared_ptr<Texture> t_diffuse = make_shared<HomoTexture>(Surface::GOOD);
	shared_ptr<Texture> t_refl = make_shared<HomoTexture>(Surface::GOOD_REFL);
	shared_ptr<Texture> t_glass = make_shared<HomoTexture>(Surface::GLASS);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	shared_ptr<Texture> t_wtm = make_shared<ImgTexture>(watermelon_fname, 10, 0.6);
	sp->add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));
	sp->add_obj(make_shared<Plane>(InfPlane(Vec(0, 1, 0), 20), make_shared<GridTexture>(GridTexture::BLACK_WHITE_REFL)));
	sp->add_obj(make_shared<Plane>(InfPlane(Vec(-3, -1, 0), -16, true), make_shared<HomoTexture>(Surface::MIRROR)));
	sp->add_obj(make_shared<Sphere>(PureSphere(Vec(+5, -6, 2.5), 2), t_glass));
	sp->add_obj(make_shared<Sphere>(PureSphere(Vec(-4, 6, 8), 1.5), t_wtm));


	const char* fname = "../resource/models/dinosaur.2k.obj";
	Mesh mesh(fname, Vec(-3, -2, 3), 9);
	mesh.set_texture(make_shared<HomoTexture>(Surface::CYAN));
	mesh.smooth = false;
	mesh.finish();
	sp->add_obj(make_shared<Mesh>(mesh));

	fname = "../resource/models/cube.obj";
	mesh = Mesh(fname, Vec(-12, 7, 4), 4);
	mesh.smooth = false;
	mesh.set_texture(make_shared<HomoTexture>(Surface::BLUE));
	mesh.finish();
	sp->add_obj(make_shared<Mesh>(mesh));

	fname = "../resource/models/horse.fine.90k.obj";
	mesh = Mesh(fname, Vec(-8, 3, 5), 8);
	mesh.smooth = false;
	mesh.set_texture(make_shared<HomoTexture>(Surface::GREEN));
	mesh.finish();
	sp->add_obj(make_shared<Mesh>(mesh));

	REP(i, 2) {
		sp->add_obj(make_shared<Sphere>(PureSphere(Vec(0 + 3, i * 4.2 + 1, 1.5), 1.5), (drand48() < 0.5 ? t_diffuse : t_glass)));
		sp->add_obj(make_shared<Sphere>(PureSphere(Vec(3 + 5, i * 3.2 + 1, 1.5), 1.5), t_refl));
	}

	sp->finish();
//	View v(sp, Vec(-8.19, -14.3, 24.9), Vec(0.79, 3.70, 5.5), 27, Geometry(w, h));
	View v(sp, Vec(-6.59, -22.3, 12.9), Vec(2.12, 3.20, 5.5), 27, Geometry(w, h));
	/*
	 *View v(s, Vec(-16.91, -10.5, 14.1), Vec(-1.63, 3.00, 3.0), 22.5, Geometry(w, h));
	 */
	if (g)
		CVViewer viewer(v, "best.png");
	else {
		CVViewer viewer(v);
		viewer.view();
	}
}

// test soft shadow
void test_shadow() {
	int w = 500, h = 500;
	Phong s; s.use_soft_shadow = true;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));

	shared_ptr<Texture> t2 = make_shared<HomoTexture>(Surface::BLUE);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t2));
	s.finish();
	View v(&s, Vec(-8.7, 4.58, 3.75), Vec(-0.30, -0.31, 1.5), 8, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

// test depth of field
void dof_ball_scene() {
	int w = 500, h = 500;
	w = 300, h = 300;
	Phong s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(9, 2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(-9, 2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(-9, -2, 50), Color::WHITE, 6.0));
	s.add_light(Light(Vec(9, -2, 50), Color::WHITE, 6.0));

	Surface surf(0, 20, 0.5, Color::CYAN * 0.9, DEFAULT_SPECULAR);

	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	shared_ptr<Texture> tball(make_shared<HomoTexture>(surf));

	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));

	REP(i, 10) REP(j, 2) s.add_obj(make_shared<Sphere>(PureSphere(Vec(j * 6, 1, i * 3), 1), tball));
	s.finish();
	View v(&s, Vec(11, -13.3, 39.75), Vec(5.4, -1, 22.8), 16, Geometry(w, h));
	v.use_dof = true;
	CVViewer viewer(v);
	viewer.view();
}

// generate lots of pictures for video
void generate_dof_video() {
	int w = 1000, h = 1000;
	Phong s;
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
	View v(&s, Vec(11, -13.3, 39.75), Vec(5.4, -1, 22.8), 16, Geometry(w, h));
	v.use_dof = true;


	v.move_screen(-3);
	REP(k, 400) {
		CVViewer viewer(v, "output/" + string_format("%03d", k) + ".png");
		printf("finish %d\n", k);
		v.move_screen(0.04);
	}
}

// test kd tree
void test_kdtree() {
	int w = 500, h = 500;
	Phong s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 6.0));
	const char* fname = "../resource/models/fixed.perfect.dragon.100K.0.07.obj";
	Mesh mesh(fname, Vec(0, 0, 2), 5);

	mesh.smooth = true;
	mesh.set_texture(make_shared<HomoTexture>(Surface::CYAN));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));
	shared_ptr<Texture> t1 = make_shared<GridTexture>(GridTexture::BLACK_WHITE_REFL);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, t1));
	s.finish();
	View v(&s, Vec(0, 0, 12), Vec(0, 0, 2), 15, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

// test obj simplify
void test_simplify() {
	int w = 500, h = 500;
	const char* fname = "../resource/models/fixed.perfect.dragon.100K.0.07.obj";
	Mesh mesh(fname, Vec(0, 0, 2), 5);
	mesh.smooth = true;
	mesh.set_texture(make_shared<HomoTexture>(Surface::CYAN));
	mesh.simplify(0.5);
	mesh.finish();

	Phong s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 6.0));
	s.add_obj(make_shared<Mesh>(mesh));
	s.finish();
	View v(&s, Vec(0, 0, 12), Vec(0, 0, 2), 15, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

// simple ball
void ball() {
	int w = 500, h = 500;
	Phong s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 7));

	shared_ptr<Texture> t2 = make_shared<HomoTexture>(Surface::BLUE);
//	shared_ptr<Texture> t2 = make_shared<ImgTexture>(watermelon_fname, 10, 0.6);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));//make_shared<Plane>(Plane())?
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t2));

	s.finish();
	View v(&s, Vec(-5, -1, 4), Vec(0, 0, 2), 8, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

// simple ball
void global_illu_ball() {
	int w = 500, h = 500;
	MCPT_EL s;
	s.add_light(Light(PureSphere(Vec(0, -10, 12), 4), Color::WHITE, 8));

	shared_ptr<Texture> t2 = make_shared<HomoTexture>(Surface::GOOD);
//	shared_ptr<Texture> t2 = make_shared<ImgTexture>(watermelon_fname, 10, 0.6);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));//make_shared<Plane>(Plane())?
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t2));

	s.finish();
	View v(&s, Vec(-5, -1, 4), Vec(0, 0, 2), 8, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

// test global illumination with diffuse and reflection
void global_illu() {
	int w = 500, h = 500;
	MCPT s;
	s.add_light(Light(PureSphere(Vec(+10, 10, 10), 4), Color::WHITE, 12));

	shared_ptr<Texture> t_diffuse = make_shared<HomoTexture>(Surface::GOOD);
	shared_ptr<Texture> t_refl = make_shared<HomoTexture>(Surface::GOOD_REFL);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t_diffuse));

	const char* fname = "../resource/models/fixed.perfect.dragon.100K.0.07.obj";
	Mesh mesh(fname, Vec(-3, +5, 2), 5);
	mesh.smooth = true;
	mesh.set_texture(make_shared<HomoTexture>(Surface::CYAN));
	mesh.simplify(0.4);
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	REP(i, 5) {
		s.add_obj(make_shared<Sphere>(PureSphere(Vec(0 + 3, i * 3 + 3, 1), 1), t_diffuse));
		s.add_obj(make_shared<Sphere>(PureSphere(Vec(3 + 3, i * 3 + 3, 1), 1), t_refl));
	}

	s.finish();
	View v(&s, Vec(-5.6, -1.6, 10.1), Vec(-0.8, 1.35, 2.5), 13, Geometry(w, h));
	CVViewer viewer(v, "global_illu.png");
	viewer.r.finish();
}

// test glass ball effect
void glass() {
	int w = 500, h = 500;
	MCPT s;
	s.add_light(Light(PureSphere(Vec(+5, 0, 13), 4), Color::WHITE, 13));

	shared_ptr<Texture> t1 = make_shared<GridTexture>(GridTexture::BLACK_WHITE);
	shared_ptr<Texture> t2 = make_shared<HomoTexture>(Surface::GLASS);
	shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, t1));
	s.add_obj(make_shared<Plane>(InfPlane(Vec(0, 1, 0), -3), tpic));
	s.add_obj(make_shared<Sphere>(PureSphere::TestSphere, t2));

	const char* fname = "../resource/models/fixed.perfect.dragon.100K.0.07.obj";
	Mesh mesh(fname, Vec(-3, +5, 2), 5);
	mesh.smooth = true;
	mesh.set_texture(make_shared<HomoTexture>(Surface::CYAN));
	mesh.simplify(0.2);
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	REP(i, 5) REP(j, 2) s.add_obj(make_shared<Sphere>(PureSphere(Vec(j * 3 + 3, i * 3 + 3, 1), 1), t2));

	s.finish();
	View v(&s, Vec(-7.6, 6.3, 10.9), Vec(-0.82, 4.32, 2.1), 15.6, Geometry(w, h));
	CVViewer viewer(v, "glass.png");
	viewer.r.finish();
}

// lots of obj
void obj_scene() {
	int w = 500, h = 500;
	Phong s;
	s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
	s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 6.0));
	const char* fname = "../resource/models/Arma.obj";
	Mesh mesh(fname, Vec(0, -8, 2), 5);
	mesh.set_texture(make_shared<HomoTexture>(Surface::CYAN));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	fname = "../resource/models/Buddha.obj";
	mesh = Mesh(fname, Vec(6, 6, 4), 7);
	mesh.set_texture(make_shared<HomoTexture>(Surface::GOOD_REFL));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	fname = "../resource/models/bunny.fine.obj";
	mesh = Mesh(fname, Vec(0, 8, 3), 5);
	mesh.set_texture(make_shared<HomoTexture>(Surface::BLUE));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	fname = "../resource/models/dinosaur.2k.obj";
	mesh = Mesh(fname, Vec(-8, 5, 3), 9);
	mesh.set_texture(make_shared<HomoTexture>(Surface::GOOD));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	fname = "../resource/models/kitten.50k.obj";
	mesh = Mesh(fname, Vec(-10, -9, 3), 9);
	mesh.set_texture(make_shared<HomoTexture>(Surface::GOOD));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	fname = "../resource/models/horse.fine.90k.obj";
	mesh = Mesh(fname, Vec(7, 0, 3), 9);
	mesh.set_texture(make_shared<HomoTexture>(Surface::GOOD));
	mesh.finish();
	s.add_obj(make_shared<Mesh>(mesh));

	shared_ptr<Texture> t1 = make_shared<ImgTexture>(texture_fname, 100, 0.6);
	s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, t1));
	s.finish();
	View v(&s, Vec(0, 0, 2), Vec(0, 5, 3), 15, Geometry(w, h));

	CVViewer viewer(v);
	viewer.view();
}

// generate sequences of pictures by simplifying using different ratio
void generate_simplified_pictures() {
	int w = 500, h = 500;
	auto dirp = opendir("../resource/models");
	dirent* dp;
	const real_t ratio[5] = {0.1, 0.2, 0.4, 0.6, 1};
	while ((dp = readdir(dirp)) != NULL) {
		if (dp->d_name[0] == '.') continue;
		cout << "reading file " << dp->d_name << " ... " << endl;
		string fname(dp->d_name);

		REP(k, 5) {
			Mesh mesh("../resource/models/" + fname, Vec(0, 0, 2), 5);
			mesh.smooth = true;
			mesh.set_texture(make_shared<HomoTexture>(Surface::CYAN));
			mesh.simplify(ratio[k]);
			mesh.finish();

			Phong s;
			s.add_light(Light(Vec(0, -10, 12), Color::WHITE, 6.0));
			s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 6.0));
			s.add_obj(make_shared<Mesh>(mesh));

			shared_ptr<Texture> tpic = make_shared<ImgTexture>(texture_fname, 100, 0.6);
			s.add_obj(make_shared<Plane>(InfPlane::XYPLANE, tpic));
			s.finish();
			View v(&s, Vec(0, 0, 10), Vec(0, 0, 0), 12, Geometry(w, h));
			CVViewer viewer(v, "output/" + string_format("%s_%lf.png", fname.c_str(), ratio[k]));
		}
	}
	closedir(dirp);
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		error_exit("please select a demo !");
	}

	int a = atoi(argv[1]);
	switch (a) {
		case 0:
			ball();
			break;
		case 1:
			dof_ball_scene();
			break;
		case 2:
			test_kdtree();
			break;
		case 3:
			obj_scene();
			break;
		case 4:
			test_simplify();
			break;
		case 5:
			test_shadow();
			break;
		case 6:
			all(false);
			break;
	//  the following are path tracing demo, will take a lot of time
		case 7:
			global_illu_ball();
			break;
		case 8:
			global_illu();
			break;
		case 9:
			glass();
			break;
		case 10:
			all(true);
			break;
	}
	return 0;
}

