/*
 *  wrap_mc.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_mc.h"
#include "common/version.h"
#include "modules/gmp/wrap_gmp.h"

static mc_id_t g_idgen;
static unsigned long long ID_EPOCH = 1483228800000ULL;  /* 2017-01-01 00:00:00 (UTC) */
static const char g_meta_timer[] = { CODE_NAME "meta.timer" };

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

static mc_timer_t *luaX_checktimer(lua_State *L, int index) {
    static const char errmsg[] = { "Invalid operand. Expected 'timer'" };

    if (lua_isuserdata(L, index) && luaL_checkudata(L, index, g_meta_timer)) {
        return (mc_timer_t *)lua_touserdata(L, index);
    }

    luaL_error(L, errmsg);
    return NULL;
}

static int mcl_timer_set(lua_State * L) {
    mc_timer_t *tmr = luaX_checktimer(L, 1);
    mc_timer_set(tmr, NULL, (unsigned int)luaL_checkinteger(L, 2));
    return 0;
}

static int mcl_timer_expired(lua_State * L) {
    mc_timer_t *tmr = luaX_checktimer(L, 1);
    lua_pushboolean(L, 1 == mc_timer_expired(tmr, NULL));
    return 1;
}

static int mcl_timer_new(lua_State * L) {
    mc_timer_t *tmr = (mc_timer_t *)luaX_newuserdata(L, g_meta_timer, sizeof(mc_timer_t));
    unsigned int n = (unsigned int)luaL_optinteger(L, 1, 0);
    mc_timer_set(tmr, NULL, n);
    return 1;
}

int luaopen_sevo_mc(lua_State* L) {
    luaL_Reg mod_id[] = {
        { "init", mcl_id_init },
        { "next", mcl_id_next },
        { "split", mcl_id_split },
        { NULL, NULL }
    };
    luaL_Reg meta_timer[] = {
        { "set", mcl_timer_set },
        { "expired", mcl_timer_expired },
        { NULL, NULL }
    };
    luaL_Reg mod_mclib[] = {
        { "timer", mcl_timer_new },
        { NULL, NULL }
    };

    mc_id_init(&g_idgen, ID_EPOCH, -1);
    luaX_register_module(L, "id", mod_id);

    luaX_register_type(L, g_meta_timer, meta_timer);

    luaX_register_funcs(L, mod_mclib);

    return 0;
}

#define MCLIB_IMPLEMENTION
#include <mclib.h>
