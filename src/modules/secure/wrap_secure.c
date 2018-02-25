/*
 *  wrap_secure.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_secure.h"
#include <string.h>

static const char g_meta_xtea[] = { CODE_NAME "meta.xtea" };

#define luaX_checkxtea(L, idx)  (mc_xtea_t *)luaL_checkudata(L, idx, g_meta_xtea)

static int mcl_encode(lua_State * L) {
    mc_xtea_t *xtea = luaX_checkxtea(L, 1);
    size_t l1 = 0, l2 = 0;
    unsigned char *t_iv, iv[8];
    const char *data;
    char *enc;
    int len;

    t_iv = (unsigned char *)luaL_checklstring(L, 2, &l1);

    if (l1 != 8) {
        return luaL_error(L, "The IV length must be 8 bytes.");
    }

    data = luaL_checklstring(L, 3, &l2);
    len = (int)luaL_optinteger(L, 4, (lua_Integer)l2);

    memcpy(iv, t_iv, sizeof(iv));
    enc = (char *)mc_malloc(mc_xtea_enclen(len));

    len = mc_xtea_encode(xtea, enc, data, len, iv);

    lua_pushlstring(L, enc, len);
    lua_pushlstring(L, (const char *)iv, sizeof(iv));

    mc_free(enc);
    return 2;
}

static int mcl_decode(lua_State * L) {
    mc_xtea_t *xtea = luaX_checkxtea(L, 1);
    size_t l1 = 0, l2 = 0;
    unsigned char *t_iv, iv[8];
    const char *data;
    char *dec;
    int len;

    t_iv = (unsigned char *)luaL_checklstring(L, 2, &l1);

    if (l1 != 8) {
        return luaL_error(L, "The IV length must be 8 bytes.");
    }

    data = luaL_checklstring(L, 3, &l2);
    len = (int)luaL_optinteger(L, 4, (lua_Integer)l2);

    memcpy(iv, t_iv, sizeof(iv));
    dec = (char *)mc_malloc(mc_xtea_enclen(len));

    len = mc_xtea_decode(xtea, dec, data, len, iv);

    lua_pushlstring(L, dec, len);
    lua_pushlstring(L, (const char *)iv, sizeof(iv));

    mc_free(dec);
    return 2;
}

static int mcl_xtea(lua_State * L) {
    size_t l = 0;
    const unsigned char *key = (unsigned char *)luaL_checklstring(L, 1, &l);
    mc_xtea_t *xtea;

    if (l != 16) {
        return luaL_error(L, "The key length must be 16 bytes.");
    }

    xtea = (mc_xtea_t *)luaX_newuserdata(L, g_meta_xtea, sizeof(mc_xtea_t));
    mc_xtea_setkeys(xtea, key);
    return 1;
}

int luaopen_sevo_secure(lua_State* L) {
    luaL_Reg meta_xtea[] = {
        { "encode", mcl_encode },
        { "decode", mcl_decode },
        { NULL, NULL }
    };
    luaL_Reg mod_secure[] = {
        { "xtea", mcl_xtea },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_xtea, meta_xtea);
    luaX_register_module(L, "secure", mod_secure);
    return 0;
}
