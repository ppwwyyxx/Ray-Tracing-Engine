// File: texture.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <memory>
#include "material/surface.hh"
#include "lib/imagereader.hh"
using std::shared_ptr;
using std::make_shared;

class Texture {
	public:
		virtual shared_ptr<Surface> get_property(real_t x, real_t y) const = 0;

		virtual shared_ptr<Surface> get_property() const
			// without coordinate, always call this before get_property(x, y)
		{ return nullptr; }

		virtual ~Texture() {};
};

class HomoTexture :public Texture {
	private:
		const Surface& pty;
	public:
		HomoTexture(const Surface& m_pty):
			pty(m_pty){}

		shared_ptr<Surface> get_property(real_t, real_t) const override {
			error_exit("should not be here");
		}

		shared_ptr<Surface> get_property() const
		{ return make_shared<Surface>(pty); }
};

class GridTexture : public Texture {
	private:
		int size;
		const Surface &pty1, &pty2;
	public:
		GridTexture(int m_size, const Surface& m_pty1, const Surface& m_pty2):
			size(m_size), pty1(m_pty1), pty2(m_pty2){}

		shared_ptr<Surface> get_property(real_t x, real_t y) const override {
			bool a1 = (((int)x / size) & 1) == 0;
			if (x < 0) a1 = !a1;
			bool a2 = (((int)y / size) & 1) == 0;
			if (y < 0) a2 = !a2;
			if (a1 ^ a2) return make_shared<Surface>(pty1);
			else return make_shared<Surface>(pty2);
		}

		static const GridTexture BLACK_WHITE_REFL, BLACK_WHITE;
};

class ImgTexture : public Texture {
	private:
		MagickReader img;
		real_t zfactor, illu;

	public:
		ImgTexture(const std::string& fname, real_t _zoom, real_t _illu = 1):
			// _zoom > 1
			img(fname), zfactor(_zoom), illu(_illu) {}

		shared_ptr<Surface> get_property(real_t x, real_t y) const override {
			int int_x = round(zfactor * x) - img.size.h / 2, int_y = (zfactor * y) - img.size.w / 2;
			Color col = img.get((int_x % img.size.h + img.size.h) % img.size.h, (int_y % img.size.w + img.size.w) % img.size.w);
			return make_shared<Surface>(0, 20, 0.7, col, 0);
		}
};
