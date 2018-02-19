/*
 *  runtime.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "runtime.h"
#include "version.h"

int luaX_register_module(lua_State *L, const char *name, const luaL_Reg *functions) {
    const luaL_Reg *l;

    lua_getglobal(L, CODE_NAME);
    lua_newtable(L);

    for (l = functions; NULL != l->name; ++l) {
        lua_pushcfunction(L, l->func);
        lua_setfield(L, -2, l->name);
    }

    lua_setfield(L, -2, name);
    lua_pop(L, 1);

    return 0;
}

int luaX_register_type(lua_State *L, const char *metaname, const luaL_Reg *functions) {
    const luaL_Reg *l;

    luaL_newmetatable(L, metaname);

    for (l = functions; NULL != l->name; ++l) {
        lua_pushfstring(L, l->name);
        lua_pushcfunction(L, l->func);
        lua_rawset(L, -3);
    }

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1);

    return 0;
}

int luaX_register_funcs(lua_State *L, const luaL_Reg *functions) {
    const luaL_Reg *l;

    lua_getglobal(L, CODE_NAME);

    for (l = functions; NULL != l->name; ++l) {
        lua_pushcfunction(L, l->func);
        lua_setfield(L, -2, l->name);
    }

    lua_pop(L, 1);

    return 0;
}

int luaX_register_searcher(lua_State *L, lua_CFunction function) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers");

    lua_pushcfunction(L, function);
    lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);

    lua_pop(L, 2);

    return 0;
}

int luaX_preload(lua_State *L, const char *name, lua_CFunction function) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");

    lua_pushcfunction(L, function);
    lua_setfield(L, -2, name);

    lua_pop(L, 2);

    return 0;
}

int luaX_require(lua_State *L, const char *name) {
    lua_getglobal(L, "require");
    lua_pushstring(L, name);
    lua_call(L, 1, 1);
    return 1;
}

void *luaX_newuserdata(lua_State * L, const char *metaname, int size) {
    void *ud = lua_newuserdata(L, size);

    luaL_getmetatable(L, metaname);
    lua_setmetatable(L, -2);

    return ud;
}

int luaX_checkboolean(lua_State *L, int index) {
    luaL_checktype(L, index, LUA_TBOOLEAN);
    return lua_toboolean(L, index);
}

int luaX_optboolean(lua_State *L, int index, int opt) {
    if (lua_isboolean(L, index) ) {
        return lua_toboolean(L, index);
    }
    return opt;
}
