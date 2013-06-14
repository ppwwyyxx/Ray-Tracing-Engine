// File: main.cc
// Date: Fri Jun 14 13:09:57 2013 +0800
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

#define PLANE_SIZE 8
#define THRES 0.5
#define N_PLANE 12000
#define PLANES_PER_FRAME 1000
#define N_PIC 120
#define N_VIEW 1
#define THETA_ROTATE 10

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
	real_t t = 2 * M_PI / N_PLANE;
	Vec norm(0, 1, 0);
	REP(_t, N_PLANE) {
		InfPlane pl(norm, 0);
		ret.push_back(pl);
		Vec new_norm(norm.x * cos(t) - norm.y * sin(t), norm.x * sin(t) + norm.y * cos(t), 0);
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
	s.add_light(Light(Vec(-10, -20, 0), Color::WHITE, 1));
	s.add_light(Light(Vec(0, 10, -50), Color::WHITE, 1));
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	shared_ptr<Texture> t1(new HomoTexture(HomoTexture::BLUE));

	Surface w_trans_surface(2, 50, Color::WHITE, Color::WHITE, Color::WHITE * DEFAULT_SPECULAR);
	Surface b_trans_surface(2, 50, Color::BLACK, Color::WHITE , Color::WHITE * DEFAULT_SPECULAR);
	Surface grey(0, 50, Color(0.5, 0.5, 0.5), Color::WHITE, Color::WHITE * DEFAULT_SPECULAR);
	shared_ptr<Texture> t2(new HomoTexture(grey));

	PureSphere xlp(Vec(8, 0, 0), 0.5);
	Sphere xlabel(xlp, t1);


	vector<Segment> segs;
	Vec v1(7, 2, 6),
		v2(-5, -2, -4),
		v3(-7, 3, 3);
	/*
	 *segs.push_back(Segment(v1, v2));
	 */
	segs.push_back(Segment(v2, v3));
	segs.push_back(Segment(v1, v3));

	/*
	 *Vec v4(5, -7, -5);
	 *segs.push_back(Segment(v1, v4));
	 *segs.push_back(Segment(v2, v4));
	 *segs.push_back(Segment(v3, v4));
	 */
	auto planes = gen_plane();

	real_t delta_theta = M_PI / 180 * THETA_ROTATE;
	real_t theta = 0;
	vector<PureSphere> points;
	vector<pair<int, int>> coors[N_PIC];
	REP(t, N_VIEW) {
		REP(k, N_PLANE) {
			if ((k + 1) % PLANES_PER_FRAME == 0) {
				for (auto & sph : points) s.add_obj(new Sphere(sph, tred));

				for (auto & sph: points) {
					Vec center = sph.center;
					Ray z_axis(Vec(0, 0, 0), Vec(0, 0, 1));
					real_t x = fabs(z_axis.distance(center));
					if ((center.x > 0) ^ (planes[k].norm.y < 0))
						x = -x;

					real_t y = 8 - center.z;
					int xx = round(x + 8), yy = round(y);
					if (BETW(xx, 0, 16) && BETW(yy, 0, 16)) {
						coors[k / PLANES_PER_FRAME].push_back({xx, yy});
					}
				}

				View v(make_shared<Space>(s), Vec(20 * sin(theta), -20 * cos(theta), 30), Vec(0, 0, 5), 30, Geometry(w, h));
				char fname[32];
				sprintf(fname, "output/%02d-%03d.png", t, k / PLANES_PER_FRAME);
				CVViewer viewer(v, fname);

				s.clean_obj();
				s.add_obj(new Sphere(xlabel));
				points.clear();
			}

			InfPlane& pl = planes[k];
			Plane tmp_plane(pl, t2);
			tmp_plane.set_finite(10, Vec(0, 0, 0));
			if (k % PLANES_PER_FRAME == 0)
				s.add_obj((new Plane(tmp_plane)));


			for (auto & seg : segs) {
				Vec inter;
				if (!intersect(inter, seg, pl)) continue;
				inter = inter + planes[k].norm;
				PureSphere sph(inter, 0.5);
				bool have = false;
				for (auto& t_sph: points)
					if (t_sph.center == sph.center) {
						have = true;
						break;
					}
				if (!have) points.push_back(sph);
			}
		}
		theta += delta_theta;
	}

	vector<pair<int, int>> newcoor[N_PIC];
	REP(k, N_PIC) {
		int l = (k - 1) % N_PIC, r = (k + 1) % N_PIC;
		for (auto & pair : coors[k]) {
			newcoor[l].push_back(pair);
			newcoor[k].push_back(pair);
			newcoor[r].push_back(pair);
		}
	}
	REP(k, N_PIC) {
		Matrix<int> mat(16, 16);
		for (auto & pair : newcoor[k])
			mat[pair.first][pair.second] = 1;
		REP(xx, 16) REP(yy, 16)
			cout << mat[xx][yy] << (yy == 15 ? "\n" : " ");
		cout << endl;
	}
}

real_t get_dist(const Segment& seg, Vec point) {
	Vec projected = seg.ray.project(point);
	if (seg.ray.contains(projected) && (projected - seg.ray.orig).mod() < seg.len)
		return seg.ray.distance(point);
	return min((seg.ray.orig - point).mod(),
			(seg.ray.orig + seg.ray.dir.get_normalized() * seg.len - point).mod());
}

bool will_intersect(const Segment& seg, Vec point) {
	real_t end_t = 2 * M_PI / N_PIC,
		   start_t = 0;
	int cnt = 0;
	while (end_t - start_t > EPS / 100) {
		cnt ++;
		real_t m1 = start_t + (end_t - start_t) / 3,
			   m2 = start_t + (end_t - start_t) * 2 / 3;
		Vec p1 = Vec(point.x * cos(m1) - point.y * sin(m1), point.x * sin(m1) + point.y * cos(m1), point.z);
		Vec p2 = Vec(point.x * cos(m2) - point.y * sin(m2), point.x * sin(m2) + point.y * cos(m2), point.z);
		real_t d1 = get_dist(seg, p1),
			   d2 = get_dist(seg, p2);
		if (d1 < THRES || d2 < THRES)
			return true;
		if (d1 < d2) end_t = m2; else
			start_t = m1;
	}
	return false;
}

bool judge(const InfPlane& pl, int xx, int yy, const vector<Segment>& segs) {
	Vec norm = pl.norm,
		x_axis = norm.cross(Vec(0, 0, 1));
	real_t realz = 7.5 - xx;
	Vec realxy = x_axis * (yy - 7.5) + norm * 1.6;
	realxy.z = realz;
	for (auto& seg : segs) {
		if (will_intersect(seg, realxy))
			return true;
	}
	return false;

}

void blxl() {
	int w, h;
	w = h = 500;
	Space s;
	Space s2;
	Light l(Vec(5, -2, 15), Color(0.9, 1, 1), 1);
	l.size = EPS;
	s.add_light(l);
	s.add_light(Light(Vec(0, 0, 50), Color::WHITE, 1));
	s.add_light(Light(Vec(50, 0, 50), Color::WHITE, 1));
	s.add_light(Light(Vec(-10, -20, 0), Color::WHITE, 1));
	s.add_light(Light(Vec(0, 10, -50), Color::WHITE, 1));
	s2.add_light(l);
	s2.add_light(Light(Vec(0, 0, 50), Color::WHITE, 1));
	s2.add_light(Light(Vec(50, 0, 50), Color::WHITE, 1));
	s2.add_light(Light(Vec(-10, -20, 0), Color::WHITE, 1));
	s2.add_light(Light(Vec(0, 10, -50), Color::WHITE, 1));
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	shared_ptr<Texture> t1(new HomoTexture(HomoTexture::BLUE));

	Surface w_trans_surface(2, 50, Color::WHITE, Color::WHITE, Color::WHITE * DEFAULT_SPECULAR);
	Surface b_trans_surface(2, 50, Color::BLACK, Color::WHITE , Color::WHITE * DEFAULT_SPECULAR);
	Surface grey(0, 50, Color(0.5, 0.5, 0.5), Color::WHITE, Color::WHITE * DEFAULT_SPECULAR);
	shared_ptr<Texture> t2(new HomoTexture(grey));

	PureSphere xlp(Vec(8, 0, 0), 0.5);
	Sphere xlabel(xlp, t1);



	real_t theta = 0;
	real_t delta = 2 * M_PI / N_PIC;
	Vec norm(0, 1, 0);
	vector<Segment> segs;
	Vec v1(7, 7, 6),
		v2(7, -7, 6),
		v3(-7, 7, -6);
	segs.push_back(Segment(v1, v2));
	segs.push_back(Segment(v2, v3));
	segs.push_back(Segment(v3, v1));

/*
 *    Vec v4(5, -7, -5);
 *    segs.push_back(Segment(v1, v4));
 *    segs.push_back(Segment(v2, v4));
 *    segs.push_back(Segment(v3, v4));
 *
 */

	REP(k, N_PIC) {
		s.clean_obj();

		Vec new_norm(norm.x * cos(k * delta) - norm.y * sin(k * delta), norm.x * sin(k * delta) + norm.y * cos(k * delta), 0);
		new_norm.normalize();
		Vec x_axis = -new_norm.cross(Vec(0, 0, 1));
		InfPlane pl(new_norm, 1.6); // 1.6


		Matrix<int> mat(16, 16);
		REP(xx, 16)	 REP(yy, 16) {
			if (judge(pl, xx, yy, segs)) {
				Vec norm = pl.norm,
					x_axis = norm.cross(Vec(0, 0, 1));
				real_t realz = 7.5 - xx;
				Vec realxy = x_axis * (yy - 7.5) + norm * 1.6;
				realxy.z = realz;

				PureSphere tmp_sph(realxy, 0.1);
				s.add_obj(new Sphere(tmp_sph, tred));
				s2.add_obj(new Sphere(tmp_sph, tred));

				mat[xx][yy] = 1;
			}
		}

		vector<int> out;
		REP(xx, 16) REP(yy, 16) {
			if (mat[xx][yy] == 1)
				out.push_back(16 * (15 - xx) + yy);
			/*
			 *cout << mat[xx][yy] << (yy == 15 ? "\n" : " ");
			 */
		}

		cout << "[";
		for (auto itr = out.begin(); itr != out.end(); itr ++)
			cout << ((itr == out.begin()) ? "" : ", ") << *itr;
		cout << "]";
		cout << endl;
		View v(make_shared<Space>(s), Vec(0, -20, 30), Vec(0, 0, 5), 30, Geometry(w, h));
		char fname[32];
		sprintf(fname, "output/%03d.png", k);
		CVViewer viewer(v, fname);

	}
		View v(make_shared<Space>(s2), Vec(0, -20, 30), Vec(0, 0, 5), 30, Geometry(w, h));
		char fname[32];
		sprintf(fname, "all.png", 1);
		CVViewer viewer(v, fname);
}

void test_obj() {
	ObjReader o;
	vector<Vertex> a; vector<Face> b;
	o.read_in("../res/humanoid_tri.obj", a, b);

	exit(0);
}

int main(int argc, char* argv[]) {
	test_obj();
	/*
	 *blxl();
	 *return 0;
	 */

	int w, h;
	w = h = 500;
	Space s;
	s.add_light(Light(Vec(5, -10, 15), Color(0.9, 1, 1), 1.5));
	s.add_light(Light(Vec(0, 0, 50), Color::WHITE, 0.6));
	s.add_light(Light(Vec(0, 0, -50), Color::WHITE, 0.6));


	shared_ptr<Texture> t1(new GridTexture(GridTexture::BLACK_WHITE));
	shared_ptr<Texture> t2(new HomoTexture(HomoTexture::BLUE));
	shared_ptr<Texture> tred(new HomoTexture(Surface::RED));
	Plane plane1(InfPlane::XYPLANE, t1);
	Face face({Vertex(Vec(5, 0, 0), 0), Vertex(Vec(5, 0, 2), 1), Vertex(Vec(4, 0, 0), 2)},
			0, 1, 2);
	Mesh mesh;
	mesh.add_vertex(Vec(5, 0, 0));
	mesh.add_vertex(Vec(5, 0, 2));
	mesh.add_vertex(Vec(4, 0, 0));
	mesh.add_face(0, 1, 2);
	mesh.set_texture(tred);
	s.add_obj(new Mesh(mesh));
	face.set_texture(tred);
	/*
	 *s.add_obj(new Face(face));
	 */
	s.add_obj(new Plane(plane1));
	Sphere sphere(PureSphere::TestSphere, t2);
	s.add_obj(new Sphere(sphere));

	View v(make_shared<Space>(s), Vec(0, -10, 5), Vec(0, 0, 5), 30, Geometry(w, h));
	CVViewer viewer(v);
	viewer.view();
}

