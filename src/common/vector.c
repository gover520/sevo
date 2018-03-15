/*
 *  vector.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "vector.h"

real_t vec2_normalize(vec2_t v, real_t length) {
    real_t ls = vec2_len(v);
    if (!r_equal(ls, r_zero)) {
        vec2_mul(v, v, length / ls);
    }
    return ls;
}

real_t vec3_normalize(vec3_t v, real_t length) {
    real_t ls = vec3_len(v);
    if (!r_equal(ls, r_zero)) {
        vec3_mul(v, v, length / ls);
    }
    return ls;
}
