/*
 *  wrap_mc.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __SONIC_WRAP_MC_H__
#define __SONIC_WRAP_MC_H__

#include "common/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    int luaopen_sonic_mc(lua_State* L);

#ifdef __cplusplus
};
#endif

#endif  /* __SONIC_WRAP_MC_H__ */
