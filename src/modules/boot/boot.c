/*
 *  boot.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "boot.h"
#include "common/logger.h"
#define EMBED_BOOT  0
#if EMBED_BOOT
#include "boot.lua.h"
#include "parallel.lua.h"
#else
#include <string.h>
#endif

#if !EMBED_BOOT
static load_luafile(lua_State* L, const char *filename) {
    char *buffer = NULL;
    int retval, len = (int)mc_file_length(filename);
    FILE *fp = fopen(filename, "r");

    if (fp) {
        char fn[MC_MAX_PATH] = { 0 };

        buffer = (char *)mc_calloc(1, len + 1);
        fread(buffer, len, 1, fp);

        retval = luaL_loadbuffer(L, buffer, (size_t)strlen(buffer), mc_base_name(fn, filename, '/'));

        mc_free(buffer);
        fclose(fp);

        if (0 != retval) {
            LG_ERR("%s", lua_tostring(L, -1));
            return luaL_error(L, lua_tostring(L, -1));
        }
        lua_call(L, 0, LUA_MULTRET);
    }

    return 1;
}
#endif

int luaopen_sevo_boot(lua_State* L) {
#if EMBED_BOOT
    if (0 != luaL_loadbuffer(L, (const char *)boot_lua, sizeof(boot_lua), "boot.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }

    lua_call(L, 0, LUA_MULTRET);
    return 1;
#else
    return load_luafile(L, "src/scripts/boot.lua");
#endif
    
}

int luaopen_sevo_parallel(lua_State* L) {
#if EMBED_BOOT
    if (0 != luaL_loadbuffer(L, (const char *)parallel_lua, sizeof(parallel_lua), "parallel.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }

    lua_call(L, 0, LUA_MULTRET);
    return 1;
#else
    return load_luafile(L, "src/scripts/parallel.lua");
#endif
}
