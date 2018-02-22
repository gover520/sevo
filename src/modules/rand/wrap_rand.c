/*
 *  wrap_rand.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_rand.h"
#include <stdlib.h>

static int mcl_seed(lua_State * L) {
    unsigned int seed = (unsigned int)luaL_checkinteger(L, 1);
    mc_srand(seed);
    return 0;
}

static int mcl_randint(lua_State * L) {
    int m = (int)luaL_optinteger(L, 1, -1);
    int n = (int)luaL_optinteger(L, 2, -1);
    int rnd = mc_rand();

    if (m > 0) {
        if (n > 0) {
            rnd = rnd % abs(n - m) + MC_MIN(m, n);
        } else {
            rnd = rnd % m;
        }
    }

    lua_pushinteger(L, rnd);
    return 1;
}

static int mcl_random(lua_State * L) {
    lua_pushnumber(L, mc_randf());
    return 1;
}

static int mcl_prime(lua_State * L) {
    int m = (int)luaL_optinteger(L, 1, -1);
    int n = (int)luaL_optinteger(L, 2, -1);

    lua_pushinteger(L, mc_rand_prime(MC_MIN(m, n), MC_MAX(m, n)));
    return 1;
}

static int mcl_normal(lua_State * L) {
    double mean = luaL_checknumber(L, 1);
    double std = luaL_checknumber(L, 2);

    lua_pushnumber(L, mc_rand_normal(mean, std));
    return 1;
}

static int mcl_exp(lua_State * L) {
    double lambda = luaL_checknumber(L, 1);

    lua_pushnumber(L, mc_rand_exp(lambda));
    return 1;
}

static int mcl_poisson(lua_State * L) {
    double lambda = luaL_checknumber(L, 1);

    lua_pushinteger(L, mc_rand_poisson(lambda));
    return 1;
}

static int mcl_binomial(lua_State * L) {
    long n = (long)luaL_checkinteger(L, 1);
    double p = luaL_checknumber(L, 2);

    lua_pushinteger(L, mc_rand_binomial(n, p));
    return 1;
}

int luaopen_sevo_rand(lua_State* L) {
    luaL_Reg mod_rand[] = {
        { "seed", mcl_seed },
        { "randint", mcl_randint },
        { "random", mcl_random },
        { "prime", mcl_prime },
        { "normal", mcl_normal },
        { "exp", mcl_exp },
        { "poisson", mcl_poisson },
        { "binomial", mcl_binomial },
        { NULL, NULL }
    };

    luaX_register_module(L, "rand", mod_rand);

    luaX_getsevo(L, "rand");
    lua_pushinteger(L, MC_RAND_MAX);
    lua_setfield(L, -2, "randmax");
    lua_pop(L, 1);  /* pop rand */

    return 0;
}
