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

    r[0] = c * sx - ky * s * sy;    /* = a */
    r[1] = s * sx + ky * c * sy;    /* = b */
    r[3] = kx * c * sx - s * sy;    /* = c */
    r[4] = kx * s * sx + c * sy;    /* = d */
    r[6] = x - ox * r[0] - oy * r[3];
    r[7] = y - ox * r[1] - oy * r[4];
    r[2] = r[5] = r_zero;
    r[8] = r_one;
}
