/*
 *  wrap_hash.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_hash.h"
#include "modules/gmp/wrap_gmp.h"

static unsigned int g_tbl_crc32[MC_CRC_TABLE_SIZE] = { 0 };
static unsigned long long g_tbl_crc64[MC_CRC_TABLE_SIZE] = { 0 };

static int mcl_hash_h32(lua_State * L) {
    size_t l = 0;
    const char *data = luaL_checklstring(L, 1, &l);
    int len = (int)luaL_optinteger(L, 2, (lua_Integer)l);

    lua_pushinteger(L, (lua_Integer)mc_hash32(data, len));
    return 1;
}

static int mcl_hash_h64(lua_State * L) {
    size_t l = 0;
    const char *data = luaL_checklstring(L, 1, &l);
    int len = (int)luaL_optinteger(L, 2, (lua_Integer)l);
    bigint_t *bi = new_bigint(L);

    mpz_set_ull(bi->x, mc_hash64(data, len));
    return 1;
}

static int mcl_hash_m32(lua_State * L) {
    size_t l = 0;
    unsigned int seed = 0;
    const char *data;
    int len;

    if (LUA_TNUMBER == lua_type(L, 1)) {
        seed = (unsigned int)lua_tointeger(L, 1);
        data = luaL_checklstring(L, 2, &l);
        len = (int)luaL_optinteger(L, 3, (lua_Integer)l);
    } else {
        data = luaL_checklstring(L, 1, &l);
        len = (int)luaL_optinteger(L, 2, (lua_Integer)l);
    }

    lua_pushinteger(L, (lua_Integer)mc_murmur32(data, len, seed));
    return 1;
}

static int mcl_hash_m64(lua_State * L) {
    size_t l = 0;
    unsigned long long seed = 0;
    const char *data;
    int len;
    bigint_t *bi;

    if (LUA_TNUMBER == lua_type(L, 1)) {
        seed = (unsigned long long)lua_tointeger(L, 1);
        data = luaL_checklstring(L, 2, &l);
        len = (int)luaL_optinteger(L, 3, (lua_Integer)l);
    } else {
        data = luaL_checklstring(L, 1, &l);
        len = (int)luaL_optinteger(L, 2, (lua_Integer)l);
    }

    bi = new_bigint(L);
    mpz_set_ull(bi->x, mc_murmur64(data, len, seed));
    return 1;
}

static int mcl_hash_adler32(lua_State * L) {
    size_t l = 0;
    unsigned int a32;
    const char *data;
    int len;

    if (0 == lua_gettop(L)) {
        lua_pushinteger(L, (lua_Integer)mc_adler32(0, NULL, -1));
        return 1;
    }
    if (LUA_TNUMBER == lua_type(L, 1)) {
        a32 = (unsigned int)lua_tointeger(L, 1);
        data = luaL_checklstring(L, 2, &l);
        len = (int)luaL_optinteger(L, 3, (lua_Integer)l);
    } else {
        a32 = mc_adler32(0, NULL, -1);
        data = luaL_checklstring(L, 1, &l);
        len = (int)luaL_optinteger(L, 2, (lua_Integer)l);
    }

    lua_pushinteger(L, (lua_Integer)mc_adler32(a32, data, len));
    return 1;
}

static int mcl_hash_adler32_combine(lua_State * L) {
    unsigned int a1 = (unsigned int)luaL_checkinteger(L, 1);
    unsigned int a2 = (unsigned int)luaL_checkinteger(L, 2);
    int len2 = (int)luaL_checkinteger(L, 3);

    lua_pushinteger(L, (lua_Integer)mc_adler32_combine(a1, a2, len2));
    return 1;
}

static int mcl_hash_adler32_rolling(lua_State * L) {
    unsigned int a32 = (unsigned int)luaL_checkinteger(L, 1);
    unsigned char ib = (unsigned char)luaL_checkinteger(L, 2);
    unsigned char ob = (unsigned char)luaL_checkinteger(L, 3);
    int wndsize = (int)luaL_checkinteger(L, 4);

    lua_pushinteger(L, (lua_Integer)mc_adler32_rolling(a32, ib, ob, wndsize));
    return 1;
}

static int mcl_hash_crc32(lua_State * L) {
    size_t l = 0;
    unsigned int c32;
    const char *data;
    int len;

    if (0 == lua_gettop(L)) {
        lua_pushinteger(L, (lua_Integer)mc_crc32(0, NULL, -1, NULL));
        return 1;
    }

    if (LUA_TNUMBER == lua_type(L, 1)) {
        c32 = (unsigned int)lua_tointeger(L, 1);
        data = luaL_checklstring(L, 2, &l);
        len = (int)luaL_optinteger(L, 3, (lua_Integer)l);
    } else {
        c32 = mc_crc32(0, NULL, -1, NULL);
        data = luaL_checklstring(L, 1, &l);
        len = (int)luaL_optinteger(L, 2, (lua_Integer)l);
    }

    lua_pushinteger(L, (lua_Integer)mc_crc32(c32, data, len, g_tbl_crc32));
    return 1;
}

static int mcl_hash_crc64(lua_State * L) {
    size_t l = 0;
    mpz_t mzi;
    unsigned long long c64;
    const char *data;
    int len, type1;
    bigint_t *bi;

    if (0 == lua_gettop(L)) {
        lua_pushinteger(L, (lua_Integer)mc_crc64(0, NULL, -1, NULL));
        return 1;
    }

    type1 = lua_type(L, 1);

    if ((LUA_TNUMBER == type1) || (LUA_TUSERDATA == type1)) {
        luaX_checkmpz(L, 1, mzi);
        c64 = mpz_get_ull(mzi);
        data = luaL_checklstring(L, 2, &l);
        len = (int)luaL_optinteger(L, 3, (lua_Integer)l);
    } else {
        c64 = mc_crc64(0, NULL, -1, NULL);
        data = luaL_checklstring(L, 1, &l);
        len = (int)luaL_optinteger(L, 2, (lua_Integer)l);
    }

    bi = new_bigint(L);
    mpz_set_ull(bi->x, mc_crc64(c64, data, len, g_tbl_crc64));
    return 1;
}

static int mcl_hash_md5(lua_State * L) {
    return 0;
}

static int mcl_hash_sha1(lua_State * L) {
    return 0;
}

int luaopen_sevo_hash(lua_State* L) {
    luaL_Reg mod_hash[] = {
        { "h32", mcl_hash_h32 },
        { "h64", mcl_hash_h64 },
        { "m32", mcl_hash_m32 },
        { "m64", mcl_hash_m64 },
        { "adler32", mcl_hash_adler32 },
        { "adler32_combine", mcl_hash_adler32_combine },
        { "adler32_rolling", mcl_hash_adler32_rolling },
        { "crc32", mcl_hash_crc32 },
        { "crc64", mcl_hash_crc64 },
        { "md5", mcl_hash_md5 },
        { "sha1", mcl_hash_sha1 },
        { NULL, NULL }
    };

    mc_crc32_table(g_tbl_crc32, MC_CRC32_SEED);
    mc_crc64_table(g_tbl_crc64, MC_CRC64_SEED);

    luaX_register_module(L, "hash", mod_hash);
    return 0;
}
