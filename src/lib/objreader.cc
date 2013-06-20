// File: objreader.cc
// Date: Fri Jun 21 00:21:10 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <cstring>
#include "lib/objreader.hh"
using namespace std;

const int LINE_MAX_LEN = 1000;
// assume that all vertex appears before face
void ObjReader::read_in(string fname, Mesh* mesh) {
	ifstream fin(fname);
	if (!fin.is_open()) { error_exit("obj file not found"); }
	static char input[LINE_MAX_LEN];
	while (fin.getline(input, LINE_MAX_LEN, '\n')) {
		if (input[0] == '#' || strlen(input) <= 1)
			continue;
		switch (input[0]) {
			case 'v':
			{
				if (input[1] == ' ') {	// v x y z
					real_t x, y, z;
					sscanf(input + 2, "%lf %lf %lf", &x, &y, &z);
					mesh->add_vertex(Vec(x, y, z));
				} else if (input[1] == 'n') {  // vn x y z
					//real_t x, y, z;
					//sscanf(input + 3, "%lf %lf %lf", &x, &y, &z);
					//mesh->set_norm(nnorm++, Vec(x, y, z));
					//don't use vn
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
				if (x != y && y != z && x != z) mesh->add_faceid(x - 1, y - 1, z - 1);
				break;
			}
			case 'g':
			case 'u':
			case 'm':
				break;
			default:
//				cout << input << endl;
				break;
		}
	}
	fin.close();
}



