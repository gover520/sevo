/*
 *  typedef.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include <mclib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stddef.h>
#include <math.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef lua_Number      real_t;

    #define r_zero          0.0
    #define r_one           1.0
    #define r_epsilon       DBL_EPSILON

    #define r_sqrt(x)       sqrt(x)
    #define r_abs(x)        fabs(x)
    #define r_sin(x)        sin(x)
    #define r_cos(x)        cos(x)
    #define r_acos(x)       acos(x)
    #define r_equal(a, b)   (r_abs((a) - (b)) < r_epsilon)

#ifdef __cplusplus
};
#endif

#endif  /* __TYPEDEF_H__ */
