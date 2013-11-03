// File: objreader.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <cstring>
#include <fstream>
#include "renderable/mesh.hh"

class ObjReader {
		static const int LINE_MAX_LEN = 1000;

	public:
		static void read_in(std::string fname, Mesh* mesh) {
			std::ifstream fin(fname);
			if (!fin.is_open()) { error_exit("obj file not found"); }
			static char input[LINE_MAX_LEN];
			while (fin.getline(input, LINE_MAX_LEN, '\n')) {
				if (input[0] == '#' or strlen(input) <= 1)
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
								std::cout << input << std::endl;
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
						break;
				}
			}
			fin.close();
		}
};
