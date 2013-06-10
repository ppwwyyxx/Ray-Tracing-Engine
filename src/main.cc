// File: main.cc
// Date: Mon Jun 10 21:13:57 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
#include "viewer.hh"
#include "image.hh"

using namespace std;

#define PLANE_SIZE 8
#define N_PLANE 12

struct Segment {
	Ray ray;
	real_t len;

	Segment(Vec a, Vec b) {
		ray = Ray(a, b - a, 1, 1);
		len = (b - a).mod();
	}
};

bool intersect(Vector& ret, const Segment& seg, const InfPlane& pl) {
	shared_ptr<Trace> tr(new PlaneTrace(pl, seg.ray));
	if (!tr->intersect())
		return false;
	real_t dist = tr->intersection_dist();
	if (dist < 0 || dist > seg.len)
		return false;

	Vec inter_point = seg.ray.get_dist(dist);
	if (inter_point.z > PLANE_SIZE)
		return false;

	Ray z_axis(Vec(0, 0, 0), Vec(0, 0, 1));
	if (z_axis.distance(inter_point) > PLANE_SIZE)
		return false;

	ret = inter_point;
	return true;
}


vector<InfPlane> gen_plane() {
	vector<InfPlane> ret;
	real_t t = 2 * M_PI / N_PLANE;
	Vec norm(0, 1, 0);
	REP(_t, N_PLANE / 2) {
		InfPlane pl(norm, 0);
		ret.push_back(pl);
		Vec new_norm(norm.x * cos(t) - norm.y * sin(t), norm.x * sin(t) + norm.y * cos(t));
		norm = new_norm;
	}
	return move(ret);
}


void blxlrsmb() {
	int w, h;
	w = h = 500;
	Space s;
	Light l(Vec(5, -2, 15), Color(0.9, 1, 1), 1);
	Light lall1(Vec(0, 0, 50), Color(1, 1, 1), 0.2);
	Light lall2(Vec(0, 50, 50), Color(1, 1, 1), 0.2);
	Light lall3(Vec(50, 0, 50), Color(1, 1, 1), 0.2);
	l.size = EPS;
	s.add_light(l);
	s.add_light(lall1);
	s.add_light(lall2);
	s.add_light(lall3);
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	shared_ptr<Texture> t1(new HomoTexture(HomoTexture::BLUE));
	InfPlane ground(Vec(0, 0, 1), -10);
	Plane groundp(ground, t1);
	s.add_obj(shared_ptr<RenderAble>(new Plane(groundp)));

	Surface w_trans_surface(2, 50, Color::WHITE, Color::WHITE, Color::WHITE * DEFAULT_SPECULAR);
	Surface b_trans_surface(2, 50, Color::BLACK, Color::WHITE , Color::WHITE * DEFAULT_SPECULAR);
	shared_ptr<Texture> t2(new GridTexture(1, w_trans_surface, b_trans_surface));



	vector<Segment> segs;
	Vec start_point(5, -5, -5),
		end_point(0, 3, 5);
	segs.push_back(Segment(start_point, end_point));
	auto planes = gen_plane();
	for (auto & pl : planes) {
		Plane tmp_plane(pl, t2);
		tmp_plane.set_finite(10, Vec(0, 0, 0));
		s.add_obj(shared_ptr<RenderAble>(new Plane(tmp_plane)));
		for (auto & seg : segs) {
			Vec inter;
			if (intersect(inter, seg, pl)) {
				cout << "norm: " << pl.norm << endl;
				cout << "inter: " << inter << endl;
				PureSphere sph(inter, 0.5);
				s.add_obj(shared_ptr<RenderAble>(new Sphere(sph, tred)));
			}
		}
	}


	View v(make_shared<Space>(s), Vec(10, -30, 30), Vec(0, 0, 5), 30, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

int main(int argc, char* argv[]) {
	blxlrsmb();
	return 0;

	int w, h;
	w = h = 500;
	Space s;
	Light l(Vec(5, -10, 15), Color(0.9, 1, 1), 1);
	l.size = EPS;
	s.add_light(l);
	Light lall(Vec(0, 0, 50), Color(1, 1, 1), 0.3);
	s.add_light(lall);

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

