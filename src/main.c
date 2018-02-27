/*
 *  main.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "common/runtime.h"
#include "common/stacktrace.h"
#include "common/logger.h"
#include "common/vfs.h"
#include "common/handle.h"
#include "modules/sevo/sevo.h"
#include "modules/event/event.h"
#include "modules/thread/wrap_thread.h"
#include <string.h>

#define DONE_QUIT       0
#define DONE_RESTART    1

static void createargtable(lua_State * L, int argc, char *argv[]) {
    int i;
    lua_createtable(L, argc, 0);
    for (i = 0; i < argc; ++i) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, "arg");
}

static int sevo_run(int argc, char *argv[], int *retval) {
    lua_State *L;
    int done, stack;

    if (0 != handle_init()) {
        LG_ERR("Handle map init failed.");
        done = DONE_QUIT;
        goto clean;
    }

    if (0 != vfs_init(argv[0])) {
        LG_ERR("VFS init failed.");
        done = DONE_QUIT;
        goto clean;
    }

    L = luaX_newstate();

    luaL_checkversion(L);
    luaL_openlibs(L);

    createargtable(L, argc, argv);
    luaX_preload(L, CODE_NAME, luaopen_sevo);

    luaX_require(L, CODE_NAME);
    lua_pop(L, 1);

    luaX_require(L, CODE_NAME ".boot");

    /* run boot returned function into a coroutine */
    lua_newthread(L);
    lua_pushvalue(L, -2);

    stack = lua_gettop(L);

    while (LUA_YIELD == lua_resume(L, NULL, 0)) {
        lua_pop(L, lua_gettop(L) - stack);
    }

    *retval = 0;
    done = DONE_QUIT;

    if (lua_isstring(L, -1) && (0 == strcmp(lua_tostring(L, -1), "restart"))) {
        done = DONE_RESTART;
    }

    if (lua_isinteger(L, -1)) {
        *retval = (int)lua_tointeger(L, -1);
    }

    lua_close(L);

clean:
    event_deinit();
    vfs_deinit();
    handle_deinit();

    return done;
}

int main(int argc, char *argv[]) {
    int done, retval;

    if ((argc > 1) && (0 == strcmp("--version", argv[1]))) {
        printf("Sevo %s\n", VERSION);
        return 0;
    }

    mc_init();
    install_stacktrace();
    logger_init();
    thread_init();

    do {
        retval = -1;
        done = sevo_run(argc, argv, &retval);
    } while (DONE_QUIT != done);

    thread_deinit();
    logger_deinit();
    mc_destroy();

    return retval;
}
