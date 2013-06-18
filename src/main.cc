// File: main.cc
// Date: Tue Jun 18 11:48:55 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include "viewer.hh"
#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
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
	int w, h;
	w = h = 500;
	Space s;
	s.add_light(Light(Vec(0, -10, 8), Color::WHITE, 2.0));
	s.add_light(Light(Vec(0, 10, 8), Color::WHITE, 2.0));
	s.add_light(Light(Vec(-10, 0, 8), Color::WHITE, 2.0));
	s.add_light(Light(Vec(10, 0, 8), Color::WHITE, 2.0));
	s.add_light(Light(Vec(0, 0, 9), Color::WHITE, 1.5));
	s.add_light(Light(Vec(0, -3, -4), Color::WHITE, 8));


	shared_ptr<Texture> t1(new GridTexture(GridTexture::BLACK_WHITE));
	shared_ptr<Texture> t2(new HomoTexture(HomoTexture::BLUE));
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	Plane plane1(InfPlane::XYPLANE, t1);

	//Mesh mesh("../res/models/Buddha.obj", Vec(0, 0, 2), 5);
	Mesh mesh("sx.obj", Vec(0, 0, 2), 5);
	mesh.set_texture(tred);
	cout << mesh.get_aabb() << endl;
	mesh.finish_add();
	cout << mesh.get_aabb() << endl;
	s.add_obj(new Mesh(mesh));
	/*
	 *s.add_obj(new Face(face));
	 */
	s.add_obj(new Plane(plane1));
	Sphere sphere(PureSphere(Vec(1, 0, 5), 0.5), t2);
	/*
	 *Sphere sphere(PureSphere::TestSphere, t2);
	 */
	/*
	 *s.add_obj(new Sphere(sphere));
	 */

	View v(make_shared<Space>(s), Vec(0.2, 0, 6), Vec(0, 0, 2), 6, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

