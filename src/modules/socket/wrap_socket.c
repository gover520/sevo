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

int luaopen_socket(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)socket_lua, sizeof(socket_lua), "socket.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_socket_ftp(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)ftp_lua, sizeof(ftp_lua), "ftp.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_socket_http(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)http_lua, sizeof(http_lua), "http.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_ltn12(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)ltn12_lua, sizeof(ltn12_lua), "ltn12.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_mime(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)mime_lua, sizeof(mime_lua), "mime.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_socket_smtp(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)smtp_lua, sizeof(smtp_lua), "smtp.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_socket_tp(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)tp_lua, sizeof(tp_lua), "tp.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_socket_url(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)url_lua, sizeof(url_lua), "url.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_socket_headers(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)headers_lua, sizeof(headers_lua), "headers.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}

int luaopen_mbox(lua_State *L) {
    if (LUA_OK == luaX_loadbuffer(L, (const char *)mbox_lua, sizeof(mbox_lua), "mbox.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
}
