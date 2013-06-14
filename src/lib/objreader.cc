// File: objreader.cc
// Date: Fri Jun 14 13:25:49 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <cstring>
#include "lib/objreader.hh"
using namespace std;

const int LINE_MAX_LEN = 1000;
void ObjReader::read_in(string fname, vector<Vertex>& vtxs, vector<Face>& faces) const {
	ifstream fin(fname);
	static char input[LINE_MAX_LEN];
	int nvtx = 0, nnorm = 0;
	while (fin.getline(input, LINE_MAX_LEN, '\n')) {
		if (input[0] == '#' || strlen(input) <= 1)
			continue;
		switch (input[0]) {
			case 'v':
			{
				if (input[1] == ' ') {	// v x y z
					real_t x, y, z;
					sscanf(input + 2, "%lf %lf %lf", &x, &y, &z);
					vtxs.push_back(Vertex(Vec(x, y, z), nvtx++));
				} else if (input[1] == 'n') {  // vn x y z
					real_t x, y, z;
					sscanf(input + 3, "%lf %lf %lf", &x, &y, &z);
					vtxs[nnorm++].norm = Vec(x, y, z);
				} else {
					cout << input << endl;
					error_exit("unrecognized format");
				}
				break;
			}
			case 'f':
			{				// XXX some model is 'f 1 2 3 4 5 6'
				int x, y, z;
				sscanf(input + 2, "%d %d %d", &x, &y, &z);
				faces.push_back(Face(vtxs, x - 1, y - 1, z - 1));
				break;
			}
			case 'g':
				continue;
				break;
			default:
				cout << input << endl;
				break;
		}
	}
}



