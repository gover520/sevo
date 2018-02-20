/*
 *  wrap_id.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_id.h"
#include "modules/gmp/wrap_gmp.h"

static mc_id_t g_idgen;
static unsigned long long ID_EPOCH = 1483228800000ULL;  /* 2017-01-01 00:00:00 (UTC) */

static int mcl_id_init(lua_State * L) {
    unsigned long long epoch = ID_EPOCH;
    int nodeid, top;
    mpz_t ep;

    top = lua_gettop(L);
    nodeid = luaL_checkinteger(L, 1);

    if (top > 1) {
        mpz_init(ep);

        luaX_checkmpz(L, 2, ep);
        epoch = mpz_get_ull(ep);

        mpz_clear(ep);
    }

    mc_id_init(&g_idgen, epoch, nodeid);

    return 0;
}

static int mcl_id_next(lua_State * L) {
    bigint_t *bi = new_bigint(L);
    mpz_set_ull(bi->x, mc_id(&g_idgen, NULL));
    return 1;
}

static int mcl_id_split(lua_State * L) {
    bigint_t *bi = new_bigint(L);
    unsigned long long timestamp;
    int sequence, nodeid;
    mpz_t id;

    mpz_init(id);

    luaX_checkmpz(L, 1, id);

    mc_id_split(&g_idgen, mpz_get_ull(id), &timestamp, &nodeid, &sequence);

    mpz_set_ull(bi->x, timestamp);

    lua_pushinteger(L, nodeid);
    lua_pushinteger(L, sequence);

    mpz_clear(id);

    return 3;
}

int luaopen_sevo_id(lua_State* L) {
    luaL_Reg mod_id[] = {
        { "init", mcl_id_init },
        { "next", mcl_id_next },
        { "split", mcl_id_split },
        { NULL, NULL }
    };

    mc_id_init(&g_idgen, ID_EPOCH, -1);
    luaX_register_module(L, "id", mod_id);

    return 0;
}
