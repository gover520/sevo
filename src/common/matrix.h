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

    #define e11(e)  e[0]
    #define e21(e)  e[1]
    #define e31(e)  e[2]
    #define e12(e)  e[3]
    #define e22(e)  e[4]
    #define e32(e)  e[5]
    #define e13(e)  e[6]
    #define e23(e)  e[7]
    #define e33(e)  e[8]


    #define mat33_identity(e)           do { memset(e, 0, sizeof(real_t) * 9); \
                                            e11(e) = e22(e) = e33(e) = r_one; } while ( 0 )

    #define mat33_add(r, a, b)          do {e11(r) = e11(a)+e11(b); \
                                            e21(r) = e21(a)+e21(b); \
                                            e31(r) = e31(a)+e31(b); \
                                            e12(r) = e12(a)+e12(b); \
                                            e22(r) = e22(a)+e22(b); \
                                            e32(r) = e32(a)+e32(b); \
                                            e13(r) = e13(a)+e13(b); \
                                            e23(r) = e23(a)+e23(b); \
                                            e33(r) = e33(a)+e33(b); } while ( 0 )

    #define mat33_sub(r, a, b)          do {e11(r) = e11(a)-e11(b); \
                                            e21(r) = e21(a)-e21(b); \
                                            e31(r) = e31(a)-e31(b); \
                                            e12(r) = e12(a)-e12(b); \
                                            e22(r) = e22(a)-e22(b); \
                                            e32(r) = e32(a)-e32(b); \
                                            e13(r) = e13(a)-e13(b); \
                                            e23(r) = e23(a)-e23(b); \
                                            e33(r) = e33(a)-e33(b); } while ( 0 )

    #define mat33_mul(r, a, b)          do {e11(r) = e11(a)*e11(b) + e12(a)*e21(b) + e13(a)*e31(b); \
                                            e12(r) = e11(a)*e12(b) + e12(a)*e22(b) + e13(a)*e32(b); \
                                            e13(r) = e11(a)*e13(b) + e12(a)*e23(b) + e13(a)*e33(b); \
                                            e21(r) = e21(a)*e11(b) + e22(a)*e21(b) + e23(a)*e31(b); \
                                            e22(r) = e21(a)*e12(b) + e22(a)*e22(b) + e23(a)*e32(b); \
                                            e23(r) = e21(a)*e13(b) + e22(a)*e23(b) + e23(a)*e33(b); \
                                            e31(r) = e31(a)*e11(b) + e32(a)*e21(b) + e33(a)*e31(b); \
                                            e32(r) = e31(a)*e12(b) + e32(a)*e22(b) + e33(a)*e32(b); \
                                            e33(r) = e31(a)*e13(b) + e32(a)*e23(b) + e33(a)*e33(b); } while ( 0 )

    #define mat33_determinant(e)        (e11(e) * (e22(e)*e33(e) - e32(e)*e23(e)) - \
                                         e21(e) * (e12(e)*e33(e) - e32(e)*e13(e)) + \
                                         e31(e) * (e12(e)*e23(e) - e22(e)*e13(e)))

    #define mat33_inverse(r, e)         do { real_t det = mat33_determinant(e); \
                                            det = r_one / det; \
                                            e11(r) =  det * (e22(e)*e33(e) - e32(e)*e23(e)); \
                                            e21(r) = -det * (e21(e)*e33(e) - e31(e)*e23(e)); \
                                            e31(r) =  det * (e21(e)*e32(e) - e31(e)*e22(e)); \
                                            e12(r) = -det * (e12(e)*e33(e) - e32(e)*e13(e)); \
                                            e22(r) =  det * (e11(e)*e33(e) - e31(e)*e13(e)); \
                                            e32(r) = -det * (e11(e)*e32(e) - e31(e)*e12(e)); \
                                            e13(r) =  det * (e12(e)*e23(e) - e22(e)*e13(e)); \
                                            e23(r) = -det * (e11(e)*e23(e) - e21(e)*e13(e)); \
                                            e33(r) =  det * (e11(e)*e22(e) - e21(e)*e12(e)); } while ( 0 )

    #define mat33_transpose(r, e)       do {e11(r) = e11(e); \
                                            e21(r) = e12(e); \
                                            e31(r) = e13(e); \
                                            e12(r) = e21(e); \
                                            e22(r) = e22(e); \
                                            e32(r) = e23(e); \
                                            e13(r) = e31(e); \
                                            e23(r) = e32(e); \
                                            e33(r) = e33(e); } while ( 0 )

    #define mat33_transformxy(r, e, v)  do {vx(r) = e11(e)*vx(v) + e12(e)*vy(v) + e13(e); \
                                            vy(r) = e21(e)*vx(v) + e22(e)*vy(v) + e23(e); } while ( 0 )

    #define mat33_transformxyz(r, e, v) do {vx(r) = e11(e)*vx(v) + e12(e)*vy(v) + e13(e)*vz(v); \
                                            vy(r) = e21(e)*vx(v) + e22(e)*vy(v) + e23(e)*vz(v); \
                                            vz(r) = e31(e)*vx(v) + e32(e)*vy(v) + e33(e)*vz(v); } while ( 0 )

    void mat33_transformation(mat33_t r, real_t x, real_t y, real_t theta,
        real_t sx, real_t sy, real_t ox, real_t oy, real_t kx, real_t ky);

#ifdef __cplusplus
};
#endif

#endif  /* __MATRIX_H__ */
