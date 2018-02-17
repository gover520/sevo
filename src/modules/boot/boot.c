/*
 *  boot.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "boot.h"
#include <string.h>

int luaopen_sevo_boot(lua_State* L) {
    static const char test_file[] = { "test/servo.lua" };

    char *buffer = NULL;
    long long len = mc_file_length(test_file);
    FILE *fp = fopen(test_file, "r");

    if (fp) {
        buffer = (char *)mc_calloc(1, len + 1);
        fread(buffer, len, 1, fp);

        if (0 == luaL_loadbuffer(L, buffer, (size_t)strlen(buffer), "boot.lua")) {
            if (0 != lua_pcall(L, 0, 1, 0)) {
                printf("-----call error-----\n");
                printf("%s\n", lua_tostring(L, -1));
            }
        }
        else {
            printf("-----error-----\n");
            printf("%s\n", lua_tostring(L, -1));
        }

        mc_free(buffer);
        fclose(fp);
    }

    return 1;
}
