// File: objreader.hh
// Date: Fri Jun 14 13:01:34 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <string>
#include <fstream>
#include "renderable/mesh.hh"

class ObjReader {
	public:
		void read_in(std::string fname, vector<Vertex>& vtxs, vector<Face>& faces) const;
};
