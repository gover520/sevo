/*
 *  matrix.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "matrix.h"

void mat33_transformation(mat33_t r, real_t x, real_t y, real_t theta,
    real_t sx, real_t sy, real_t ox, real_t oy, real_t kx, real_t ky) {
    real_t c = r_cos(theta);
    real_t s = r_sin(theta);

    /**
     * |1    x| |c -s  | |sx     | | 1 ky  | |1   -ox|
     * |  1  y| |s  c  | |   sy  | |kx  1  | |  1 -oy|
     * |     1| |     1| |      1| |      1| |     1 |
     *   move    rotate    scale     skew      origin
     **/

    e11(r) = c * sx - ky * s * sy;    /* = a */
    e21(r) = s * sx + ky * c * sy;    /* = b */
    e12(r) = kx * c * sx - s * sy;    /* = c */
    e22(r) = kx * s * sx + c * sy;    /* = d */
    e13(r) = x - ox * e11(r) - oy * e12(r);
    e23(r) = y - ox * e21(r) - oy * e22(r);
    e31(r) = e32(r) = r_zero;
    e33(r) = r_one;
}
