/*
 *  wrap_json.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_json.h"
#include "dkjson.lua.h"

int luaopen_sevo_dkjson(lua_State* L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)dkjson_lua, sizeof(dkjson_lua), "dkjson.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}
