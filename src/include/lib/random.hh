//File: random.hh

//Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "lib/utils.hh"
#include <random>

template <typename T>
class RandomGen {
	public:
		RandomGen() { }
		virtual ~RandomGen(){ }

		virtual T get() = 0;
};

class LinearDistribution: public RandomGen<real_t> {
	static std::mt19937 gen;
	static std::piecewise_linear_distribution<real_t> dist;
	static bool initialized = false;
	public:
		LinearDistribution() {
			if (!initialized) {
				double range[2] = { 0, 1 };
				double prob[2] = { 0, 1 };
				dist = std::piecewise_linear_distribution<real_t>(begin(range), end(range), begin(prob));
				initialized = true;
			}
		}

		real_t get() override { return dist(gen); }
};
