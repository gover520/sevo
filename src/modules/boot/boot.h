/*
 *  boot.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __BOOT_H__
#define __BOOT_H__

#include "common/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    int luaopen_sevo_boot(lua_State* L);
    int luaopen_sevo_parallel(lua_State* L);

#ifdef __cplusplus
};
#endif

#endif  /* __BOOT_H__ */
