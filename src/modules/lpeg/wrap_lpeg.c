/*
 *  wrap_lpeg.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_lpeg.h"
#include "re.lua.h"

int luaopen_re(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)re_lua, sizeof(re_lua), "re.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}
