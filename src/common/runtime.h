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

#ifdef _MSC_VER
# define strncasecmp    _strnicmp
# define strcasecmp     _stricmp
# define strtoll        _strtoi64
# define strtoull       _strtoui64
#endif

#ifdef __cplusplus
extern "C" {
#endif

    #define LUAX_LIBNAME    "sevo"

    int luaX_register_module(lua_State *L, const char *name, const luaL_Reg *functions);
    int luaX_register_type(lua_State *L, const char *metaname, const luaL_Reg *functions);
    int luaX_register_funcs(lua_State *L, const luaL_Reg *functions);
    int luaX_register_searcher(lua_State *L, lua_CFunction function);
    int luaX_preload(lua_State *L, const char *name, lua_CFunction function);
    int luaX_require(lua_State *L, const char *name);
    void *luaX_newuserdata(lua_State * L, const char *metaname, int size);
    int luaX_checkint(lua_State *L, int index);

#ifdef __cplusplus
};
#endif

#endif  /* __RUNTIME_H__ */
