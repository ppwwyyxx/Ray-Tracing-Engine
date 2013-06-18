// File: common.hh
// Date: Wed Jun 19 00:29:17 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <utility>
#include <limits>

#include "lib/debugutils.hh"
#include "lib/utils.hh"

const real_t DEFAULT_TRACING_WEIGHT_THRESHOLD = EPS;
const real_t AMBIENT_FACTOR = 0.03;
const real_t AIR_BEER_DENSITY = 0.03;
const real_t DEFAULT_SPECULAR = 0.4;
const real_t REFL_DIFFUSE_FACTOR = 0.3;
const real_t REFL_DECAY = 0.01;
const real_t TRANSM_BLEND_FACTOR = 0.7;
const real_t DEFAULT_REFRACTIVE_INDEX = 1.05;
const real_t AIR_REFRACTIVE_INDEX = 1;

const int MAX_RECURSIVE_DEPTH = 2;
const int DOF_SAMPLE_CNT = 20;
const real_t DOF_SAMPLE_RADIUS = 0.1;

const real_t DOF_SCREEN_DIST_FACTOR = 0.1;
const int SOFT_SHADOW_LIGHT = 20;
const real_t SOFT_SHADOW_RADIUS = 0.8;

const int KDTREE_MAX_DEPTH = 20;
const int KDTREE_TERMINATE_OBJ_CNT = 15;

