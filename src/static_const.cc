// File: static_const.cc
// Date: Sun Jun 23 01:27:52 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "color.hh"
#include "material/surface.hh"
#include "material/texture.hh"
#include "renderable/plane.hh"
#include "renderable/sphere.hh"
#include "const.hh"
using namespace std;

const Color Color::NONE(0, 0, 0),
			Color::BLACK(EPS, EPS, EPS),
			Color::WHITE(1, 1, 1),
			Color::RED(1, EPS, EPS),
			Color::BLUE(EPS, EPS, 1),
			Color::GREEN(EPS, 1, EPS),
			Color::YELLOW(1, 1, EPS),
			Color::CYAN(EPS, 1, 1),
			Color::MAGNETA(1, EPS, 1);

const Surface
	Surface::WHITE_REFL(0, 20, 1, Color::WHITE, DEFAULT_SPECULAR),
	Surface::BLACK_REFL(0, 20, 1, Color::BLACK, DEFAULT_SPECULAR),
	Surface::BLUE_REFL(1, 50, 0.5, Color::BLUE, DEFAULT_SPECULAR),
	Surface::CYAN_REFL(0, 20, 0.2, (Color)Color::CYAN * 0.6, DEFAULT_SPECULAR),
	Surface::LIGHT(0, 0, 0, Color::NONE, 0, Color::WHITE * 12);


const GridTexture
	GridTexture::BLACK_WHITE(1, Surface::BLACK_REFL, Surface::WHITE_REFL);

const HomoTexture
	HomoTexture::BLUE(Surface::BLUE_REFL),
	HomoTexture::CYAN(Surface::CYAN_REFL);


const PureSphere PureSphere::TestSphere(Vec(0, 0, 2), 1.99);

const InfPlane InfPlane::XYPLANE(Vec(0, 0, 1), 0),
	  InfPlane::YZPLANE(Vec(1, 0, 0), 0),
	  InfPlane::XZPLANE(Vec(0, 1, 0), 0);
