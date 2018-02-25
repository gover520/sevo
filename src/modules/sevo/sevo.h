/*
 *  sevo.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __SEVO_H__
#define __SEVO_H__

#include "common/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    int luaopen_sevo(lua_State* L);

#ifdef __cplusplus
};
#endif

#endif  /* __SEVO_H__ */
