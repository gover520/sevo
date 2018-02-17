/*
 *  wrap_socket.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __WRAP_SOCKET_H__
#define __WRAP_SOCKET_H__

#include "common/runtime.h"
#include <luasocket.h>
#include <mime.h>

#ifdef __cplusplus
extern "C" {
#endif

    int luaopen_socket(lua_State *L);
    int luaopen_socket_ftp(lua_State *L);
    int luaopen_socket_http(lua_State *L);
    int luaopen_ltn12(lua_State *L);
    int luaopen_mime(lua_State *L);
    int luaopen_socket_smtp(lua_State *L);
    int luaopen_socket_tp(lua_State *L);
    int luaopen_socket_url(lua_State *L);
    int luaopen_socket_headers(lua_State *L);
    int luaopen_mbox(lua_State *L);

#ifdef __cplusplus
};
#endif

#endif  /* __WRAP_SOCKET_H__ */
