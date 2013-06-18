// File: surface.cc
// Date: Tue Jun 18 11:44:11 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "material/surface.hh"
using namespace std;

const Surface
	Surface::WHITE_REFL(0, 20, 1, Color::WHITE, Color::WHITE * DEFAULT_SPECULAR),
	Surface::BLACK_REFL(0, 20, 1, Color::BLACK, Color::WHITE * DEFAULT_SPECULAR),
	Surface::BLUE_REFL(1, 50, 0.5, Color::BLUE, Color::WHITE * DEFAULT_SPECULAR),
	Surface::RED(0.2, 20, 0.5, (Color)(Color::RED + Color::BLUE) * 0.5, Color::WHITE * DEFAULT_SPECULAR);



