/*
 *  wrap_logger.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_logger.h"
#include "common/logger.h"
#include <string.h>

static mc_sstr_t l_buffer_repr(mc_sstr_t s, const char *str, int len) {
    if ((!str) || (len <= 0)) {
        return s;
    }
    while (len--) {
        if (isprint(*str)) {
            s = mc_sstr_cat_buffer(s, str, 1);
        } else {
            char tmp[4] = { '\\', 'x', 0, 0 };
            mc_byte2hex(*str, tmp + 2);
            s = mc_sstr_cat_buffer(s, tmp, sizeof(tmp));
        }
        str += 1;
    }
    return s;
}

static mc_sstr_t l_buffer(lua_State *L) {
    int i, top = lua_gettop(L);
    mc_sstr_t buf = mc_sstr_create(1024);
    const char *s;
    size_t l;

    lua_getglobal(L, "tostring");

    for (i = 1; i <= top; ++i) {
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);

        s = lua_tolstring(L, -1, &l);

        if (!s) {
            mc_sstr_destroy(buf);
            return NULL;
        }

        if (i > 1) {
            buf = mc_sstr_cat_buffer(buf, "\t", 1);
        }
        buf = l_buffer_repr(buf, s, (int)l);

        lua_pop(L, 1);  /* pop result */
    }

    lua_pop(L, 1);      /* pop tostring */

    return buf;
}

static int l_debug(lua_State *L) {
    mc_sstr_t data = l_buffer(L);

    if (!data) {
        return luaL_error(L, "'tostring' must return a string to 'logger'");
    }

    logger(LGT_LUA, LGL_DEBUG, data);
    mc_sstr_destroy(data);

    return 0;
}

static int l_info(lua_State *L) {
    mc_sstr_t data = l_buffer(L);

    if (!data) {
        return luaL_error(L, "'tostring' must return a string to 'logger'");
    }

    logger(LGT_LUA, LGL_INFO, data);
    mc_sstr_destroy(data);

    return 0;
}

static int l_warn(lua_State *L) {
    mc_sstr_t data = l_buffer(L);

    if (!data) {
        return luaL_error(L, "'tostring' must return a string to 'logger'");
    }

    logger(LGT_LUA, LGL_WARN, data);
    mc_sstr_destroy(data);

    return 0;
}

static int l_error(lua_State *L) {
    mc_sstr_t data = l_buffer(L);

    if (!data) {
        return luaL_error(L, "'tostring' must return a string to 'logger'");
    }

    logger(LGT_LUA, LGL_ERROR, data);
    mc_sstr_destroy(data);

    return 0;
}

static int l_loglevel(lua_State *L) {
    const char *ls = luaL_checkstring(L, 1);

    if (0 == strcmp(ls, "debug")) {
        logger_level(LGL_DEBUG);
    } else if (0 == strcmp(ls, "info")) {
        logger_level(LGL_INFO);
    } else if (0 == strcmp(ls, "warn")) {
        logger_level(LGL_WARN);
    } else if (0 == strcmp(ls, "error")) {
        logger_level(LGL_ERROR);
    } else if (0 == strcmp(ls, "disable")) {
        logger_level(LGL_DISABLE);
    } else {
        return luaL_error(L, "parameter error, Expected 'debug', 'info', 'warn', 'error', 'disable'");
    }

    return 0;
}

static int l_logflush(lua_State *L) {
    logger_flush();
    return 0;
}

int luaopen_sevo_logger(lua_State* L) {
    luaL_Reg mod_logger[] = {
        { "loglevel", l_loglevel },
        { "logflush", l_logflush },
        { "debug", l_debug },
        { "info", l_info },
        { "warn", l_warn },
        { "error", l_error },
        { NULL, NULL }
    };

    luaX_register_funcs(L, mod_logger);
    return 0;
}
