// File: surface.cc
// Date: Mon Jun 17 13:57:15 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "material/surface.hh"
using namespace std;

const Surface
	Surface::WHITE_REFL(0, 20, 0, Color::WHITE, Color::WHITE * DEFAULT_SPECULAR),
	Surface::BLACK_REFL(0, 20, 0, Color::BLACK, Color::WHITE * DEFAULT_SPECULAR),
	Surface::BLUE_REFL(1, 50, 1, Color::BLUE, Color::WHITE * DEFAULT_SPECULAR),
	Surface::RED(0, 20, 0.5, Color::RED, Color::WHITE * DEFAULT_SPECULAR);



