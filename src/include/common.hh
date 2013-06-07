// File: common.hh
// Date: Fri Jun 07 22:17:48 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <utility>
#include <limits>

#include "debugutils.hh"
#include "utils.hh"

const real_t DEFAULT_TRACING_WEIGHT_THRESHOLD = EPS;
const real_t AMBIENT_FACTOR = 0.13;
const real_t AIR_DENSITY = 0.02;
const real_t DEFAULT_SPECULAR = 0.4;
const real_t MAX_RECURSIVE_DEPTH = 10;
const real_t REFL_DIFFUSE_FACTOR = 0.3;
const real_t REFL_DECAY = 0.2;
const real_t TRANSM_BLEND_FACTOR = 0.7;
const real_t DEFAULT_REFRACTIVE_INDEX = 1.1;
const real_t AIR_REFRACTIVE_INDEX = 1;
