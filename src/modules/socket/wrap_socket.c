/*
 *  wrap_socket.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_socket.h"
#include "socket.lua.h"
#include "ftp.lua.h"
#include "http.lua.h"
#include "ltn12.lua.h"
#include "mime.lua.h"
#include "smtp.lua.h"
#include "tp.lua.h"
#include "url.lua.h"
#include "headers.lua.h"
#include "mbox.lua.h"
#include "common/logger.h"

int luaopen_socket(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)socket_lua, sizeof(socket_lua), "socket.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_socket_ftp(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)ftp_lua, sizeof(ftp_lua), "ftp.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_socket_http(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)http_lua, sizeof(http_lua), "http.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_ltn12(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)ltn12_lua, sizeof(ltn12_lua), "ltn12.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_mime(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)mime_lua, sizeof(mime_lua), "mime.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_socket_smtp(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)smtp_lua, sizeof(smtp_lua), "smtp.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_socket_tp(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)tp_lua, sizeof(tp_lua), "tp.lua")) {
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_socket_url(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)url_lua, sizeof(url_lua), "url.lua")) {
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_socket_headers(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)headers_lua, sizeof(headers_lua), "headers.lua")) {
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}

int luaopen_mbox(lua_State *L) {
    if (0 != luaL_loadbuffer(L, (const char *)mbox_lua, sizeof(mbox_lua), "mbox.lua")) {
        return luaL_error(L, lua_tostring(L, -1));
    }
    lua_call(L, 0, LUA_MULTRET);
    return 1;
}
