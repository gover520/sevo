/*
 *  wrap_vfs.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_vfs.h"

/*
static int loader(lua_State *L) {
    const char *name = lua_tostring(L, 1);
    if (0 != luaL_loadbuffer(L, buffer, size, name)) {
        return luaL_error(L, lua_tostring(L, -1));
    }
    return 1;
}
*/

int luaopen_sevo_vfs(lua_State* L) {
    //luaX_register_searcher(L, loader);
    return 0;
}
