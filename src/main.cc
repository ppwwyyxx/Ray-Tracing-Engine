// File: main.cc
// Date: Sat Jun 15 16:09:35 2013 +0800
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
	s.add_light(Light(Vec(0, -100, 50), Color(0.9, 1, 1), 2));
	s.add_light(Light(Vec(0, 0, 80), Color::WHITE, 1));
	s.add_light(Light(Vec(0, 0, -50), Color::WHITE, 0.6));


	shared_ptr<Texture> t1(new GridTexture(GridTexture::BLACK_WHITE));
	shared_ptr<Texture> t2(new HomoTexture(HomoTexture::BLUE));
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	Plane plane1(InfPlane::XYPLANE, t1);

	Mesh mesh("../res/teapot.obj");
	mesh.set_texture(tred);
	mesh.finish_add();
	cout << mesh.get_aabb() << endl;
	s.add_obj(new Mesh(mesh));
	/*
	 *s.add_obj(new Face(face));
	 */
	/*
	 *s.add_obj(new Plane(plane1));
	 */
	Sphere sphere(PureSphere::TestSphere, t2);
	/*
	 *s.add_obj(new Sphere(sphere));
	 */

	View v(make_shared<Space>(s), Vec(0, -50, 5), Vec(0, 0, 5), 300, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

