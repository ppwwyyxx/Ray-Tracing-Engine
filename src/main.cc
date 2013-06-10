// File: main.cc
// Date: Tue Jun 11 01:00:16 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
#include "space.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "view.hh"
#include "viewer.hh"
#include "image.hh"

using namespace std;

#define PLANE_SIZE 8
#define N_PLANE 120
#define PLANES_PER_FRAME 5

struct Segment {
	Ray ray;
	real_t len;

	Segment(Vec a, Vec b) {
		ray = Ray(a, b - a, 1, 1);
		len = (b - a).mod();
	}
};

bool intersect(Vector& ret, const Segment& seg, const InfPlane& pl) {
	real_t THRES = 1;
	shared_ptr<Trace> tr(new PlaneTrace(pl, seg.ray));
	if (!tr->intersect())
		return false;
	real_t dist = tr->intersection_dist();
	if (dist < THRES || dist > seg.len + THRES)
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
	real_t t = M_PI / N_PLANE;
	Vec norm(0, 1, 0);
	REP(_t, N_PLANE) {
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
	l.size = EPS;
	s.add_light(l);
	s.add_light(Light(Vec(0, 0, 50), Color::WHITE, 1));
	s.add_light(Light(Vec(50, 0, 50), Color::WHITE, 1));
	s.add_light(Light(Vec(-10, -20, 20), Color::WHITE, 1));
	s.add_light(Light(Vec(0, 10, -50), Color::WHITE, 1));
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	shared_ptr<Texture> t1(new HomoTexture(HomoTexture::BLUE));

	Surface w_trans_surface(2, 50, Color::WHITE, Color::WHITE, Color::WHITE * DEFAULT_SPECULAR);
	Surface b_trans_surface(2, 50, Color::BLACK, Color::WHITE , Color::WHITE * DEFAULT_SPECULAR);
	Surface grey(3, 50, Color(0.5, 0.5, 0.5), Color::WHITE, Color::WHITE * DEFAULT_SPECULAR);
	shared_ptr<Texture> t2(new HomoTexture(grey));


	vector<Segment> segs;
	Vec v1(7, 2, 6),
		v2(-2, 2, -7),
		v3(-7, -3, 3);
	segs.push_back(Segment(v1, v2));
	segs.push_back(Segment(v2, v3));
	segs.push_back(Segment(v1, v3));

	Vec v4(5, -7, -5);
	segs.push_back(Segment(v1, v4));
	segs.push_back(Segment(v2, v4));
	segs.push_back(Segment(v3, v4));
	auto planes = gen_plane();
	REP(k, N_PLANE) {
		if ((k + 1) % PLANES_PER_FRAME == 0) {
			View v(make_shared<Space>(s), Vec(0, -30, 20), Vec(0, 0, 5), 30, Geometry(w, h));
			char fname[32];
			sprintf(fname, "output/%03d.png", k / PLANES_PER_FRAME);
			CVViewer viewer(v, fname);
			s.clean_obj();
		}

		InfPlane& pl = planes[k];
		Plane tmp_plane(pl, t2);
		tmp_plane.set_finite(10, Vec(0, 0, 0));
		if (k % PLANES_PER_FRAME == 0)
			s.add_obj((new Plane(tmp_plane)));


		for (auto & seg : segs) {
			Vec inter;
			if (!intersect(inter, seg, pl)) continue;
			PureSphere sph(inter, 0.5);
			s.add_obj(new Sphere(sph, tred));
		}

	}
}

int main(int argc, char* argv[]) {
	blxlrsmb();
	return 0;

	int w, h;
	w = h = 500;
	Space s;
	s.add_light(Light(Vec(5, -10, 15), Color(0.9, 1, 1), 1));
	s.add_light(Light(Vec(0, 0, 50), Color::WHITE, 0.3));
	s.add_light(Light(Vec(0, 0, -50), Color::WHITE, 0.3));


	shared_ptr<Texture> t1(new GridTexture(GridTexture::BLACK_WHITE));
	Plane plane1(InfPlane::XYPLANE, t1);
	s.add_obj(new Plane(plane1));
	t1 = shared_ptr<Texture>(new HomoTexture(HomoTexture::BLUE));
	Sphere sphere(PureSphere::TestSphere, t1);
	s.add_obj(new Sphere(sphere));

	View v(make_shared<Space>(s), Vec(0, -10, 5), Vec(0, 0, 5), 30, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

