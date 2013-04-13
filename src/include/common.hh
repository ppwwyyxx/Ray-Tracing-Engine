// File: common.hh
// Date: Wed Apr 10 09:06:50 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <utility>
#include <limits>

#include "debugutils.hh"

typedef double real_t;
const real_t EPS = 1e-6;

const real_t DEFAULT_TRACING_WEIGHT_THRESHOLD = EPS;
const real_t AMBIENT_FACTOR = 0.13;
const real_t AIR_DENSITY = 0.02;
const real_t DEFAULT_SPECULAR = 0.4;
const real_t MAX_RECURSIVE_DEPTH = 10;
const real_t REFL_DIFFUSE_FACTOR = 1.2;
const real_t REFL_DECAY = 0.2;

template<typename T>
bool update_min(T &dest, const T &val) {
	if (val < dest) {
		dest = val; return true;
	}
	return false;
}

inline real_t sqr(real_t x)
{ return x * x; }

template<typename T>
bool update_max(T &dest, const T &val) {
	if (dest < val) {
		dest = val; return true;
	}
	return false;
}

