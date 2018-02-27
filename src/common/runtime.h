/*
 *  runtime.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include <mclib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif

    lua_State *luaX_newstate(void);
    int luaX_register_module(lua_State *L, const char *name, const luaL_Reg *functions);
    int luaX_register_type(lua_State *L, const char *metaname, const luaL_Reg *functions);
    int luaX_register_funcs(lua_State *L, const luaL_Reg *functions);
    int luaX_register_searcher(lua_State *L, lua_CFunction function);
    int luaX_preload(lua_State *L, const char *name, lua_CFunction function);
    int luaX_require(lua_State *L, const char *name);
    int luaX_getsevo(lua_State *L, const char *name);
    int luaX_loadbuffer(lua_State *L, const void *buff, int size, const char *name);
    void *luaX_newuserdata(lua_State * L, const char *metaname, int size);
    int luaX_checkboolean(lua_State *L, int index);
    int luaX_optboolean(lua_State *L, int index, int opt);

#ifdef __cplusplus
};
#endif

#endif  /* __RUNTIME_H__ */
