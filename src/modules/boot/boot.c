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
#define EMBED_BOOT  1
#if EMBED_BOOT
#include "boot.lua.h"
#else
#include <string.h>
#endif

int luaopen_sevo_boot(lua_State* L) {
#if EMBED_BOOT
    if (0 != luaL_loadbuffer(L, (const char *)boot_lua, sizeof(boot_lua), "boot.lua")) {
        LG_ERR("%s", lua_tostring(L, -1));
        return luaL_error(L, lua_tostring(L, -1));
    }

    lua_pcall(L, 0, 1, LUA_MULTRET);
#else
    static const char boot_file[] = { "src/scripts/boot.lua" };

    char *buffer = NULL;
    int retval, len = (int)mc_file_length(boot_file);
    FILE *fp = fopen(boot_file, "r");

    if (fp) {
        buffer = (char *)mc_calloc(1, len + 1);
        fread(buffer, len, 1, fp);

        retval = luaL_loadbuffer(L, buffer, (size_t)strlen(buffer), "boot.lua");

        mc_free(buffer);
        fclose(fp);

        if (0 != retval) {
            LG_ERR("%s", lua_tostring(L, -1));
            return luaL_error(L, lua_tostring(L, -1));
        }
        lua_pcall(L, 0, 1, LUA_MULTRET);
    }
#endif
    return 1;
}
