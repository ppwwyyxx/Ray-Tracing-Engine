// File: objreader.hh
// Date: Fri Jun 14 23:42:08 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <string>
#include <fstream>
#include <tuple>
#include "renderable/mesh.hh"

class ObjReader {
	public:
		static void read_in(std::string fname, Mesh* mesh);
};
