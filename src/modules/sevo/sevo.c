/*
 *  sevo.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "sevo.h"
#include "sevo.lua.h"
#include "modules/gmp/wrap_gmp.h"
#include "modules/boot/boot.h"
#include "modules/lpeg/wrap_lpeg.h"
#include "modules/vfs/wrap_vfs.h"
#include "modules/logger/wrap_logger.h"
#include "modules/id/wrap_id.h"
#include "modules/time/wrap_time.h"
#include "modules/hash/wrap_hash.h"
#include "modules/rand/wrap_rand.h"
#include "modules/net/wrap_net.h"
#include "modules/secure/wrap_secure.h"
#include "modules/socket/wrap_socket.h"
#include "modules/thread/wrap_thread.h"
#include "modules/math/wrap_math.h"
#include "modules/dbm/wrap_dbm.h"
#include "modules/env/wrap_env.h"
#include <ffi.h>

int luaopen_sevo(lua_State * L) {
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
        { CODE_NAME ".id", luaopen_sevo_id },
        { CODE_NAME ".time", luaopen_sevo_time },
        { CODE_NAME ".hash", luaopen_sevo_hash },
        { CODE_NAME ".rand", luaopen_sevo_rand },
        { CODE_NAME ".net", luaopen_sevo_net },
        { CODE_NAME ".secure", luaopen_sevo_secure },
        { CODE_NAME ".thread", luaopen_sevo_thread },
        /* vfs */
        { CODE_NAME ".vfs", luaopen_sevo_vfs },
        /* logger */
        { CODE_NAME ".logger", luaopen_sevo_logger },
        /* math */
        { CODE_NAME ".math", luaopen_sevo_math },
        /* dbm */
        { CODE_NAME ".dbm", luaopen_sevo_dbm },
        /* env */
        { CODE_NAME ".env", luaopen_sevo_env },
        /* boot loader */
        { CODE_NAME ".boot", luaopen_sevo_boot },
        { CODE_NAME ".parallel", luaopen_sevo_parallel },
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

    lua_pushinteger(L, mc_cpunum());
    lua_setfield(L, -2, "_CPUNUM");

#if defined(_WIN32)
    lua_pushliteral(L, "Windows");
#elif defined(__linux__)
    lua_pushliteral(L, "Linux");
#elif defined(__APPLE__)
    lua_pushliteral(L, "macOS");
#else
    lua_pushliteral(L, "Untested-OS");
#endif
    lua_setfield(L, -2, "_OS");

    /* Preload module loaders */
    for (l = modules; NULL != l->name; ++l) {
        luaX_preload(L, l->name, l->func);
    }

    if (LUA_OK == luaX_loadbuffer(L, (const char *)sevo_lua, sizeof(sevo_lua), "sevo.lua")) {
        lua_call(L, 0, LUA_MULTRET);
    }

    return 1;
}
