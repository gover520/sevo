/*
*  wrap_env.h
*
*  copyright (c) 2018 Xiongfei Shi
*
*  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
*  license: Apache-2.0
*/

#ifndef __WRAP_ENV_H__
#define __WRAP_ENV_H__

#include "common/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    int env_init(void);
    void env_deinit(void);

    int luaopen_sevo_env(lua_State* L);

#ifdef __cplusplus
};
#endif

#endif  /* __WRAP_ENV_H__ */
