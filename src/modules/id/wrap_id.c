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

static int mcl_id_init(lua_State * L) {
    long long epoch = -1;
    int nodeid, top;
    mpz_t ep;

    top = lua_gettop(L);
    nodeid = (int)luaL_checkinteger(L, 1);

    if (top > 1) {
        mpz_init(ep);

        luaX_checkmpz(L, 2, ep);
        epoch = mpz_get_ll(ep);

        mpz_clear(ep);
    }

    mc_id_set(epoch, nodeid);

    return 0;
}

static int mcl_id_next(lua_State * L) {
    bigint_t *bi = new_bigint(L);
    mpz_set_ll(bi->x, mc_id(NULL));
    return 1;
}

static int mcl_id_split(lua_State * L) {
    bigint_t *bi = new_bigint(L);
    long long timestamp;
    int sequence, nodeid;
    mpz_t id;

    mpz_init(id);

    luaX_checkmpz(L, 1, id);

    mc_id_split(mpz_get_ll(id), &timestamp, &nodeid, &sequence);

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

    luaX_register_module(L, "id", mod_id);

    return 0;
}
