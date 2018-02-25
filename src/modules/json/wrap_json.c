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
#include "common/logger.h"

int luaopen_sevo_dkjson(lua_State* L) {
    if (0 != luaL_loadbuffer(L, (const char *)dkjson_lua, sizeof(dkjson_lua), "dkjson.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}
