/*
 *  main.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "common/runtime.h"
#include "common/version.h"
#include "common/stacktrace.h"
#include "common/logger.h"
#include "common/vfs.h"
#include "modules/gmp/wrap_gmp.h"
#include "modules/mc/wrap_mc.h"
#include "modules/boot/boot.h"
#include "modules/lpeg/wrap_lpeg.h"
#include "modules/socket/wrap_socket.h"
#include "modules/vfs/wrap_vfs.h"
#include <ffi.h>

#define DONE_QUIT       0
#define DONE_RESTART    1

static int luaopen_sevo(lua_State * L) {
    luaL_Reg *l, modules[] = {
        /* ffi */
        { "ffi", luaopen_ffi },
        /* lpeg */
        { "lpeg", luaopen_lpeg },
        { "re", luaopen_re },
        /* socket */
        { "socket.core", luaopen_socket_core },
        { "mime.core", luaopen_mime_core },
        { "socket", luaopen_socket },
        { "socket.ftp", luaopen_socket_ftp },
        { "socket.http", luaopen_socket_http },
        { "ltn12", luaopen_ltn12 },
        { "mime", luaopen_mime },
        { "socket.smtp", luaopen_socket_smtp },
        { "socket.tp", luaopen_socket_tp },
        { "socket.url", luaopen_socket_url },
        { "socket.headers", luaopen_socket_headers },
        { "mbox", luaopen_mbox },
        /* gmp */
        { CODE_NAME ".int", luaopen_sevo_int },
        /* mclib */
        { CODE_NAME ".mc", luaopen_sevo_mc },
        /* vfs */
        { CODE_NAME ".vfs", luaopen_sevo_vfs },
        /* boot loader */
        { CODE_NAME ".boot", luaopen_sevo_boot },
        { NULL, NULL }
    };

    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, CODE_NAME);

    lua_pushstring(L, VERSION);
    lua_setfield(L, -2, "_VERSION");

    lua_pushinteger(L, VERSION_NUM);
    lua_setfield(L, -2, "_VERSION_NUM");

    lua_pushinteger(L, VERSION_FULL);
    lua_setfield(L, -2, "_VERSION_FULL");

    lua_pushinteger(L, VERSION_MAJOR);
    lua_setfield(L, -2, "_VERSION_MAJOR");

    lua_pushinteger(L, VERSION_MINOR);
    lua_setfield(L, -2, "_VERSION_MINOR");

    lua_pushinteger(L, VERSION_PATCH);
    lua_setfield(L, -2, "_VERSION_PATCH");

#if defined(_WIN32)
    lua_pushstring(L, "Windows");
#elif defined(__linux__)
    lua_pushstring(L, "Linux");
#elif defined(__APPLE__)
    lua_pushstring(L, "OSX");
#else
    lua_pushstring(L, "Unknown");
#endif
    lua_setfield(L, -2, "_OS");

    /* Preload module loaders */
    for (l = modules; NULL != l->name; ++l) {
        luaX_preload(L, l->name, l->func);
    }

    return 1;
}

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

    if ((argc > 1) && (0 == strcasecmp("--version", argv[1]))) {
        printf("Sevo %s", VERSION);
        *retval = 0;
        return DONE_QUIT;
    }

    if (0 != vfs_init(argv[0])) {
        LG_ERR("VFS init failed.");
        *retval = -1;
        return DONE_QUIT;
    }

    L = luaL_newstate();

    luaL_checkversion(L);
    luaL_openlibs(L);

    createargtable(L, argc, argv);
    luaX_preload(L, CODE_NAME, luaopen_sevo);

    luaX_require(L, CODE_NAME);
    lua_pop(L, 1);

    luaX_require(L, CODE_NAME ".boot");
    lua_pop(L, 1);

    lua_close(L);

    vfs_deinit();

    *retval = 0;
    return DONE_QUIT;
}

int main(int argc, char *argv[]) {
    int done, retval = -1;

    mc_init();
    install_stacktrace(NULL);

    do {
        done = sevo_run(argc, argv, &retval);
    } while (DONE_QUIT != done);

    mc_destroy();

    return retval;
}
