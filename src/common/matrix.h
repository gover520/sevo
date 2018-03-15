/*
 *  matrix.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "vector.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * | e0  e3  e6 |
     * | e1  e4  e7 |
     * | e2  e5  e8 |
     **/
    typedef real_t  mat33_t[9];

    #define mat33_identity(e)           do { memset(e, 0, sizeof(real_t) * 9); \
                                            e[0] = e[4] = e[8] = r_one; } while ( 0 )

    #define mat33_add(r, a, b)          do {r[0] = a[0]+b[0]; \
                                            r[1] = a[1]+b[1]; \
                                            r[2] = a[2]+b[2]; \
                                            r[3] = a[3]+b[3]; \
                                            r[4] = a[4]+b[4]; \
                                            r[5] = a[5]+b[5]; \
                                            r[6] = a[6]+b[6]; \
                                            r[7] = a[7]+b[7]; \
                                            r[8] = a[8]+b[8]; } while ( 0 )

    #define mat33_sub(r, a, b)          do {r[0] = a[0]-b[0]; \
                                            r[1] = a[1]-b[1]; \
                                            r[2] = a[2]-b[2]; \
                                            r[3] = a[3]-b[3]; \
                                            r[4] = a[4]-b[4]; \
                                            r[5] = a[5]-b[5]; \
                                            r[6] = a[6]-b[6]; \
                                            r[7] = a[7]-b[7]; \
                                            r[8] = a[8]-b[8]; } while ( 0 )

    #define mat33_mul(r, a, b)          do {r[0] = a[0]*b[0] + a[3]*b[1] + a[6]*b[2]; \
                                            r[3] = a[0]*b[3] + a[3]*b[4] + a[6]*b[5]; \
                                            r[6] = a[0]*b[6] + a[3]*b[7] + a[6]*b[8]; \
                                            r[1] = a[1]*b[0] + a[4]*b[1] + a[7]*b[2]; \
                                            r[4] = a[1]*b[3] + a[4]*b[4] + a[7]*b[5]; \
                                            r[7] = a[1]*b[6] + a[4]*b[7] + a[7]*b[8]; \
                                            r[2] = a[2]*b[0] + a[5]*b[1] + a[8]*b[2]; \
                                            r[5] = a[2]*b[3] + a[5]*b[4] + a[8]*b[5]; \
                                            r[8] = a[2]*b[6] + a[5]*b[7] + a[8]*b[8]; } while ( 0 )

    #define mat33_determinant(e)        (e[0]*(e[4]*e[8] - e[5]*e[7]) - e[1]*(e[3]*e[8] - e[5]*e[6]) + e[2]*(e[3]*e[7] - e[4]*e[6]))

    #define mat33_inverse(r, e)         do { real_t det = mat33_determinant(e); \
                                            det = r_one / det; \
                                            r[0] =  det * (e[4]*e[8] - e[5]*e[7]); \
                                            r[1] = -det * (e[1]*e[8] - e[2]*e[7]); \
                                            r[2] =  det * (e[1]*e[5] - e[2]*e[4]); \
                                            r[3] = -det * (e[3]*e[8] - e[5]*e[6]); \
                                            r[4] =  det * (e[0]*e[8] - e[2]*e[6]); \
                                            r[5] = -det * (e[0]*e[5] - e[2]*e[3]); \
                                            r[6] =  det * (e[3]*e[7] - e[4]*e[6]); \
                                            r[7] = -det * (e[0]*e[7] - e[1]*e[6]); \
                                            r[8] =  det * (e[0]*e[4] - e[1]*e[3]); } while ( 0 )

    #define mat33_transpose(r, e)       do {r[0] = e[0]; \
                                            r[1] = e[3]; \
                                            r[2] = e[6]; \
                                            r[3] = e[1]; \
                                            r[4] = e[4]; \
                                            r[5] = e[7]; \
                                            r[6] = e[2]; \
                                            r[7] = e[5]; \
                                            r[8] = e[8]; } while ( 0 )

    #define mat33_transformxy(r, e, v)  do {vx(r) = e[0]*vx(v) + e[3]*vy(v) + e[6]; \
                                            vy(r) = e[1]*vx(v) + e[4]*vy(v) + e[7]; } while ( 0 )

    #define mat33_transformxyz(r, e, v) do {vx(r) = e[0]*vx(v) + e[3]*vy(v) + e[6]*vz(v); \
                                            vy(r) = e[1]*vx(v) + e[4]*vy(v) + e[7]*vz(v); \
                                            vz(r) = e[2]*vx(v) + e[5]*vy(v) + e[8]*vz(v); } while ( 0 )

    void mat33_transformation(mat33_t r, real_t x, real_t y, real_t theta,
        real_t sx, real_t sy, real_t ox, real_t oy, real_t kx, real_t ky);

#ifdef __cplusplus
};
#endif

#endif  /* __MATRIX_H__ */
