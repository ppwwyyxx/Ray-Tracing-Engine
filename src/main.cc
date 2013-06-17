// File: main.cc
// Date: Mon Jun 17 11:09:30 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
#include "viewer.hh"
#include "lib/imagereader.hh"
#include "renderable/mesh.hh"
#include "lib/objreader.hh"

using namespace std;

/*
 *void test_obj() {
 *    ObjReader o;
 *    vector<Vertex> a; vector<shared_ptr<Face>> b;
 *    o.read_in("../res/humanoid_tri.obj", a, b);
 *
 *    exit(0);
 *}
 */

int main(int argc, char* argv[]) {
	/*
	 *blxl();
	 *return 0;
	 */
	int w, h;
	w = h = 500;
	Space s;
	s.add_light(Light(Vec(0, -20, 0), Color(0.9, 1, 1), 1.5));
	s.add_light(Light(Vec(0, -10, 8), Color::WHITE, 1.2));
	s.add_light(Light(Vec(0, -2, -20), Color::WHITE, 0.5));


	shared_ptr<Texture> t1(new GridTexture(GridTexture::BLACK_WHITE));
	shared_ptr<Texture> t2(new HomoTexture(HomoTexture::BLUE));
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	Plane plane1(InfPlane::XYPLANE, t1);

	Mesh mesh("../res/humanoid_tri.obj", Vec(0, 0, 4), 5);
	mesh.set_texture(tred);
	mesh.finish_add();
	cout << mesh.get_aabb() << endl;
	s.add_obj(new Mesh(mesh));
	/*
	 *s.add_obj(new Face(face));
	 */
	s.add_obj(new Plane(plane1));
	Sphere sphere(PureSphere(Vec(1, 0, 5), 0.5), t2);
	s.add_obj(new Sphere(sphere));

	View v(make_shared<Space>(s), Vec(0.2, -5, 5), Vec(0, 0, 5), 10, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

