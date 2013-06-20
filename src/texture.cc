// File: texture.cc
// Date: Fri Jun 21 00:25:19 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include "material/texture.hh"
using namespace std;

const GridTexture
	GridTexture::BLACK_WHITE(1, Surface::BLACK_REFL, Surface::WHITE_REFL);

const HomoTexture
	HomoTexture::BLUE(Surface::BLUE_REFL),
	HomoTexture::CYAN(Surface::CYAN_REFL);

