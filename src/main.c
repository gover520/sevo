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
#include "modules/gmp/wrap_gmp.h"
#include "modules/mc/wrap_mc.h"
#include "modules/boot/boot.h"
#include "modules/lpeg/wrap_lpeg.h"
#include "modules/socket/wrap_socket.h"
#include <ffi.h>

#define DONE_QUIT       0
#define DONE_RESTART    1

static const luaL_Reg modules[] = {
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
    { LUAX_LIBNAME ".int", luaopen_sevo_int },
    /* mclib */
    { LUAX_LIBNAME ".mc", luaopen_sevo_mc },
    /* boot loader */
    { LUAX_LIBNAME ".boot", luaopen_sevo_boot },
    { NULL, NULL }
};

/*
static int loader(lua_State *L) {
    const char *name = lua_tostring(L, 1);
    if (0 != luaL_loadbuffer(L, buffer, size, name)) {
        return luaL_error(L, lua_tostring(L, -1));
    }
    return 1;
}
*/

static int luaopen_sevo(lua_State * L) {
    const luaL_Reg *l;

    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, LUAX_LIBNAME);

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

    //luaX_register_searcher(L, loader);

    luaX_require(L, LUAX_LIBNAME ".int");
    lua_pop(L, 1);  /* pop returned by require */

    luaX_require(L, LUAX_LIBNAME ".mc");
    lua_pop(L, 1);  /* pop returned by require */

    lua_pop(L, 1);  /* pop table */

    return 0;
}

static int sevo_run(int argc, char *argv[], int *retval) {
    lua_State *L;

    if ((argc > 1) && (0 == strcasecmp("--version", argv[1]))) {
        printf("Sevo %s\n", VERSION);
        *retval = 0;
        return DONE_QUIT;
    }

    L = luaL_newstate();

    luaL_checkversion(L);
    luaL_openlibs(L);

    luaX_preload(L, LUAX_LIBNAME, luaopen_sevo);

    luaX_require(L, LUAX_LIBNAME);
    lua_pop(L, 1);

    luaX_require(L, LUAX_LIBNAME ".boot");
    lua_pop(L, 1);

    lua_close(L);

    *retval = 0;
    return DONE_QUIT;
}

int main(int argc, char *argv[]) {
    int done, retval = 0;

    mc_init();
    install_stacktrace(NULL);

    do {
        done = sevo_run(argc, argv, &retval);
    } while (DONE_QUIT != done);

    mc_destroy();

    return retval;
}
