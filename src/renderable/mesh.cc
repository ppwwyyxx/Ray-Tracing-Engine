// File: mesh.cc

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <algorithm>
#include "mesh_simplifier.hh"
#include "renderable/mesh.hh"
#include "lib/objreader.hh"
#include "lib/Timer.hh"
using namespace std;

Mesh::Mesh(std::string fname, const Vec& _pivot, real_t _zsize, const shared_ptr<Texture>& _texture):
	Renderable(_texture), pivot(_pivot), zoom_size(_zsize) {
	ObjReader::read_in(fname, this);
	cout << "nvtx = " << vtxs.size() << ", nface = " << face_ids.size() << endl;
	transform_vtxs();
}

void Mesh::simplify(real_t ratio) {		// only require face_ids
	Timer timer;
	MeshSimplifier s(*this, ratio);
	s.simplify();
	printf("Simplification spends %lf seconds\n", timer.get_time());
	cout << "after simplification: nvtx = " << vtxs.size() << ", nface = " << face_ids.size() << endl;
}

void Mesh::transform_vtxs() {
	Vec sum;
	real_t zfactor = zoom_size / (bound_max - bound_min).get_max();		// * factor
	for (auto &k : vtxs) sum = sum + k.pos;
	sum = pivot - sum / vtxs.size();

	for_each(begin(vtxs), end(vtxs),
		[&](Vertex &v) {
			v.pos = pivot + (v.pos + sum - pivot) * zfactor;
	});
	bound_min = pivot + (bound_min + sum - pivot) * zfactor;
	bound_max = pivot + (bound_max + sum - pivot) * zfactor;
}

void Mesh::finish() {		// build tree, calculate smooth norm
	if (smooth) {		// calculate vtx norm
		if (face_ids.size() < 30) {
			printf("Number of faces is too small, cannot use smooth!\n");
			smooth = false;
		} else {
			int nvtx = vtxs.size();

			struct NormSum {
				Vec sum = Vec(0, 0, 0);
				int cnt = 0;
				void add(const Vec& v) { sum = sum + v, cnt ++; }
			};

			NormSum* norm_sum = new NormSum[nvtx];
			for (auto & t : face_ids) {
				int a = t[0], b = t[1], c = t[2];
				Vec tmp_norm = Triangle(vtxs[a].pos, vtxs[b].pos, vtxs[c].pos).norm;
				// norms might be in opposite direction ? obj gives vertexes in correct order
				norm_sum[a].add(tmp_norm);
				norm_sum[b].add(tmp_norm);
				norm_sum[c].add(tmp_norm);
			}
			REP(k, nvtx)
				vtxs[k].norm = norm_sum[k].sum / norm_sum[k].cnt;
			delete[] norm_sum;
		}
	}

	for (auto &ids : face_ids) add_face(ids);
	if (use_tree)
		tree = make_shared<KDTree>(list<rdptr>(begin(faces), end(faces)), get_aabb());
}

shared_ptr<Trace> Mesh::get_trace(const Ray& ray, real_t dist) const {
	if (use_tree)
		return tree->get_trace(ray, dist);

	shared_ptr<Trace> ret;
	real_t min = numeric_limits<real_t>::max();

	for (auto & face : faces) {
		auto tmp = face->get_trace(ray, dist);
		if (tmp && update_min(min, tmp->intersection_dist()))
			ret = tmp;
	}
	return ret;
}
