// File: texture.hh
// Date: Sun Apr 07 21:28:17 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <memory>
#include "material/surface.hh"

class Texture {
	public:
		virtual std::shared_ptr<const Surface> get_property(real_t x, real_t y) const = 0;

		virtual std::shared_ptr<const Surface> get_property() // without coordinate, always call this before get_property(x, y)
		{ return nullptr; }

		virtual ~Texture() {};
};


class HomoTexture :public Texture {
	private:
		const Surface& pty;
	public:
		HomoTexture(const Surface& m_pty):
			pty(m_pty){}

		std::shared_ptr<const Surface> get_property(real_t x, real_t y) const {
			print_debug("%lf, %lf\n", x, y);
			error_exit("should not be here");
		}

		std::shared_ptr<const Surface> get_property()
		{ return std::make_shared<const Surface>(pty); }

		static const HomoTexture BLUE;
};

class GridTexture : public Texture {
	private:
		int size;
		const Surface& pty1, &pty2;
	public:
		GridTexture(int m_size, const Surface& m_pty1, const Surface& m_pty2):
			size(m_size), pty1(m_pty1), pty2(m_pty2){}

		std::shared_ptr<const Surface> get_property(real_t x, real_t y) const {
			bool a1 = (((int)x / size) & 1) == 0;
			if (x < 0) a1 = !a1;
			bool a2 = (((int)y / size) & 1) == 0;
			if (y < 0) a2 = !a2;
			if (a1 ^ a2)
				return std::make_shared<const Surface>(pty1);
			else
				return std::make_shared<const Surface>(pty2);
		}

		static const GridTexture BLACK_WHITE;
};
