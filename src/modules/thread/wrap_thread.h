/*
 *  wrap_thread.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __WRAP_THREAD_H__
#define __WRAP_THREAD_H__

#include "common/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    int luaopen_sevo_thread(lua_State* L);

#ifdef __cplusplus
};
#endif

#endif  /* __WRAP_THREAD_H__ */
