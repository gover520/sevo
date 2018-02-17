/*
 *  wrap_lpeg.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __WRAP_LPEG_H__
#define __WRAP_LPEG_H__

#include "common/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    int luaopen_lpeg(lua_State *L);
    int luaopen_re(lua_State *L);

#ifdef __cplusplus
};
#endif

#endif  /* __WRAP_LPEG_H__ */
