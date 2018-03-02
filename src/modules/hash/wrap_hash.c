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

static const char g_meta_md5[] = { CODE_NAME "meta.md5" };
static const char g_meta_sha1[] = { CODE_NAME "meta.sha1" };

#define luaX_checkmd5(L, idx)   (mc_md5_ctx *)luaL_checkudata(L, idx, g_meta_md5)
#define luaX_checksha1(L, idx)  (mc_sha1_ctx *)luaL_checkudata(L, idx, g_meta_sha1)

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

static int mcl_md5_update(lua_State * L) {
    size_t l = 0;
    mc_md5_ctx *md5 = luaX_checkmd5(L, 1);
    const char *data = luaL_checklstring(L, 2, &l);
    unsigned int len = (unsigned int)luaL_optinteger(L, 3, (lua_Integer)l);

    mc_md5_update(md5, (const unsigned char *)data, len);
    return 0;
}

static int mcl_md5_digest(lua_State * L) {
    mc_md5_ctx *md5 = luaX_checkmd5(L, 1);
    unsigned char digest[16];

    mc_md5_final(digest, md5);

    lua_pushlstring(L, (const char *)digest, sizeof(digest));
    return 1;
}

static int mcl_md5_hexdigest(lua_State * L) {
    mc_md5_ctx *md5 = luaX_checkmd5(L, 1);
    unsigned char digest[16];
    char *p, hexdigest[sizeof(digest) * 2 + 1];
    int i;

    mc_md5_final(digest, md5);

    for (i = 0, p = hexdigest; i < (int)sizeof(digest); ++i, p+=2) {
        mc_byte2hex(digest[i], p);
    }
    *p = 0;

    lua_pushstring(L, hexdigest);
    return 1;
}

static int mcl_hash_md5(lua_State * L) {
    mc_md5_ctx *md5 = (mc_md5_ctx *)luaX_newuserdata(L, g_meta_md5, sizeof(mc_md5_ctx));
    mc_md5_init(md5);
    return 1;
}

static int mcl_sha1_update(lua_State * L) {
    size_t l = 0;
    mc_sha1_ctx *sha1 = luaX_checksha1(L, 1);
    const unsigned char *data = (unsigned char *)luaL_checklstring(L, 2, &l);
    unsigned int len = (unsigned int)luaL_optinteger(L, 3, (lua_Integer)l);

    mc_sha1_update(sha1, data, len);
    return 0;
}

static int mcl_sha1_digest(lua_State * L) {
    mc_sha1_ctx *sha1 = luaX_checksha1(L, 1);
    unsigned char digest[20];

    mc_sha1_final(digest, sha1);

    lua_pushlstring(L, (const char *)digest, sizeof(digest));
    return 1;
}

static int mcl_sha1_hexdigest(lua_State * L) {
    mc_sha1_ctx *sha1 = luaX_checksha1(L, 1);
    unsigned char digest[20];
    char *p, hexdigest[sizeof(digest) * 2 + 1];
    int i;

    mc_sha1_final(digest, sha1);

    for (i = 0, p = hexdigest; i < (int)sizeof(digest); ++i, p += 2) {
        mc_byte2hex(digest[i], p);
    }
    *p = 0;

    lua_pushstring(L, hexdigest);
    return 1;
}

static int mcl_hash_sha1(lua_State * L) {
    mc_sha1_ctx *sha1 = (mc_sha1_ctx *)luaX_newuserdata(L, g_meta_sha1, sizeof(mc_sha1_ctx));
    mc_sha1_init(sha1);
    return 1;
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
    luaL_Reg meta_md5[] = {
        { "update", mcl_md5_update },
        { "digest", mcl_md5_digest },
        { "hexdigest", mcl_md5_hexdigest },
        { NULL, NULL }
    };
    luaL_Reg meta_sha1[] = {
        { "update", mcl_sha1_update },
        { "digest", mcl_sha1_digest },
        { "hexdigest", mcl_sha1_hexdigest },
        { NULL, NULL }
    };

    mc_crc32_table(g_tbl_crc32, MC_CRC32_SEED);
    mc_crc64_table(g_tbl_crc64, MC_CRC64_SEED);

    luaX_register_type(L, g_meta_md5, meta_md5);
    luaX_register_type(L, g_meta_sha1, meta_sha1);
    luaX_register_module(L, "hash", mod_hash);
    return 0;
}
