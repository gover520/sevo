/*
 *  boot.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "boot.h"
#define EMBED_BOOT  1
#if EMBED_BOOT
#include "boot.lua.h"
#include "parallel.lua.h"
#else
#include <string.h>
#endif

#if !EMBED_BOOT
static int load_luafile(lua_State* L, const char *filename) {
    char *buffer = NULL;
    int len = (int)mc_file_length(filename);
    FILE *fp = fopen(filename, "rb");

    if (fp) {
        char fn[MC_MAX_PATH] = { 0 };

        buffer = (char *)mc_calloc(1, len + 2);
        fread(buffer, len, 1, fp);

        if (LUA_OK == luaX_loadbuffer(L, buffer, len, mc_base_name(fn, filename, '/'))) {
            lua_call(L, 0, LUA_MULTRET);
        }

        mc_free(buffer);
        fclose(fp);
    }

    return 1;
}
#endif

int luaopen_sevo_boot(lua_State* L) {
#if EMBED_BOOT
    if (LUA_OK == luaX_loadbuffer(L, boot_lua, sizeof(boot_lua), "boot.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
#else
    return load_luafile(L, "src/scripts/boot.lua");
#endif
}

int luaopen_sevo_parallel(lua_State* L) {
#if EMBED_BOOT
    if (LUA_OK == luaX_loadbuffer(L, parallel_lua, sizeof(parallel_lua), "parallel.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }
    return 1;
#else
    return load_luafile(L, "src/scripts/parallel.lua");
#endif
}
