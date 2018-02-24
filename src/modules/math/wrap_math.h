/*
*  wrap_math.h
*
*  copyright (c) 2018 Xiongfei Shi
*
*  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
*  license: Apache-2.0
*/

#ifndef __WRAP_MATH_H__
#define __WRAP_MATH_H__

#include "common/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    int luaopen_sevo_math(lua_State* L);

#ifdef __cplusplus
};
#endif

#endif  /* __WRAP_MATH_H__ */
