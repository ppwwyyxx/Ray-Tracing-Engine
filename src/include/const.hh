// File: const.hh
// Date: Fri Sep 27 20:08:57 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <utility>
#include <limits>

#include "lib/utils.hh"

const real_t DEFAULT_TRACING_WEIGHT_THRESHOLD = 0.001;
const real_t DEFAULT_REFRACTIVE_INDEX = 1.2;
const real_t AIR_REFRACTIVE_INDEX = 1;
const int ANTIALIAS_SAMPLE_CNT = 2;
const int MAX_PHONG_DEPTH = 4;
const int MAX_MCPT_DEPTH = 4;

// color blending
const real_t AMBIENT_FACTOR = 0.01;
const real_t AIR_BEER_DENSITY = 0.03;
const real_t DEFAULT_SPECULAR = 0.4;
const real_t REFL_DIFFUSE_FACTOR = 0.5;
const real_t TRANSM_DIFFUSE_FACTOR = 0.3;
const real_t REFL_DECAY = 0.04;


const int DOF_SAMPLE_CNT = 15;
const real_t DOF_SAMPLE_RADIUS = 0.4;

const real_t DOF_SCREEN_DIST_FACTOR = 0.1;
const int SOFT_SHADOW_LIGHT = 30;
const real_t SOFT_SHADOW_RADIUS = 0.8;

const int KDTREE_MAX_DEPTH = 100;
const int KDTREE_TERMINATE_OBJ_CNT = 20;

const int GLOBAL_ILLU_SAMPLE_CNT = 20;
