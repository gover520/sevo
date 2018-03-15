/*
 *  vector.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef real_t  vec2_t[2];
    typedef real_t  vec3_t[3];

    #define vx(v)   v[0]
    #define vy(v)   v[1]
    #define vz(v)   v[2]

    /* vx*vx + vy*vy */
    #define vec2_lensq(v)       (vx(v)*vx(v) + vy(v)*vy(v))
    /* sqrt(vx*vx + vy*vy) */
    #define vec2_len(v)         r_sqrt(vec2_lensq(v))
    /* ax*bx + ay*by */
    #define vec2_dot(a, b)      (vx(a)*vx(b) + vy(a)*vy(b))
    /* ax*by - ay*bx */
    #define vec2_cross(a, b)    (vx(a)*vy(b) - vy(a)*vx(b))
    /* rx,ry = -vx, -vy */
    #define vec2_neg(r, v)      do { vx(r) = -vx(v); vy(r) = -vy(v); } while ( 0 )
    /* rx,ry = ax+bx, ay+by */
    #define vec2_add(r, a, b)   do { vx(r) = vx(a)+vx(b); vy(r) = vy(a)+vy(b); } while ( 0 )
    /* rx,ry = ax-bx, ay-by */
    #define vec2_sub(r, a, b)   do { vx(r) = vx(a)-vx(b); vy(r) = vy(a)-vy(b); } while ( 0 )
    /* rx,ry = vx*s, vy*s */
    #define vec2_mul(r, v, s)   do { vx(r) = vx(v)*s; vy(r) = vy(v)*s; } while ( 0 )
    /* rx,ry = vx/s, vy/s */
    #define vec2_div(r, v, s)   do { real_t inv = r_one / s; vec2_mul(r, v, inv); } while ( 0 )

    /* vx,vy = vx*(length/vlen), vy*(length/vlen) */
    real_t vec2_normalize(vec2_t v, real_t length);

    /* rx, ry = x*cos(theta) - y*sin(theta), x*sin(theta) + y*cos(theta) */
    void vec2_rotate(vec2_t r, const vec2_t v, real_t theta);


    /* vx*vx + vy*vy + vz*vz */
    #define vec3_lensq(v)       (vx(v)*vx(v) + vy(v)*vy(v) + vz(v)*vz(v))
    /* sqrt(vx*vx + vy*vy + vz*vz) */
    #define vec3_len(v)         r_sqrt(vec3_lensq(v))
    /* ax*bx + ay*by + az*bz */
    #define vec3_dot(a, b)      (vx(a)*vx(b) + vy(a)*vy(b) + vz(a)*vz(b))
    /* rx, ry, rz = ay*bz - az*by, az*bx - ax*bz, ax*by - ay*bx */
    #define vec3_cross(r, a, b) do {vx(r) = vy(a)*vz(b) - vz(a)*vy(b);  \
                                    vy(r) = vz(a)*vx(b) - vx(a)*vz(b);  \
                                    vz(r) = vx(a)*vy(b) - vy(a)*vx(b); } while ( 0 )
    /* rx,ry,rz = -vx, -vy, -vz */
    #define vec3_neg(r, v)      do { vx(r) = -vx(v); vy(r) = -vy(v); vz(r) = -vz(v); } while ( 0 )
    /* rx,ry,rz = ax+bx, ay+by, az+bz */
    #define vec3_add(r, a, b)   do { vx(r) = vx(a)+vx(b); vy(r) = vy(a)+vy(b); vz(r) = vz(a)+vz(b); } while ( 0 )
    /* rx,ry,rz = ax-bx, ay-by, az-bz */
    #define vec3_sub(r, a, b)   do { vx(r) = vx(a)-vx(b); vy(r) = vy(a)-vy(b); vz(r) = vz(a)-vz(b); } while ( 0 )
    /* rx,ry,rz = vx*s, vy*s, vz*s */
    #define vec3_mul(r, v, s)   do { vx(r) = vx(v)*s; vy(r) = vy(v)*s; vz(r) = vz(v)*s; } while ( 0 )
    /* rx,ry,rz = vx/s, vy/s, vz/s */
    #define vec3_div(r, v, s)   do { real_t inv = r_one / s; vec3_mul(r, v, inv); } while ( 0 )

    /* vx,vy,vz = vx*(length/vlen), vy*(length/vlen), vz*(length/vlen) */
    real_t vec3_normalize(vec3_t v, real_t length);

    /* ry, rz = vy*cos(theta) - vz*sin(theta), vy*sin(theta) + vz*cos(theta) */
    void vec3_rotate_x(vec3_t r, const vec3_t v, real_t theta);
    /* rx, rz = vz*sin(theta) + vx*cos(theta), vz*cos(theta) - vx*sin(theta) */
    void vec3_rotate_y(vec3_t r, const vec3_t v, real_t theta);
    /* rx, ry = vx*cos(theta) - vy*sin(theta), vx*sin(theta) + vy*cos(theta) */
    void vec3_rotate_z(vec3_t r, const vec3_t v, real_t theta);

#ifdef __cplusplus
};
#endif

#endif  /* __VECTOR_H__ */
