/*
*  wrap_env.c
*
*  copyright (c) 2018 Xiongfei Shi
*
*  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
*  license: Apache-2.0
*/

#include "wrap_env.h"

static mc_htable_t  *env_htable = NULL;
static mc_rwlock_t  env_rwlock = { 0, 0 };

int env_init(void) {
    if (!env_htable) {
        mc_rwlock_create(&env_rwlock);
        env_htable = mc_htable_create(NULL);
    }
    return 0;
}

void env_deinit(void) {
    if (env_htable) {
        mc_htable_destroy(env_htable);
        env_htable = NULL;
    }
    mc_rwlock_destroy(&env_rwlock);
}

static int env_ht_add(lua_State * L) {
    size_t kl = 0, vl = 0;
    const char *k = luaL_checklstring(L, 1, &kl);
    const char *v = luaL_checklstring(L, 2, &vl);
    int retval;

    mc_rwlock_wrlock(&env_rwlock);
    retval = mc_htable_add(env_htable, k, (int)kl, v, (int)vl);
    mc_rwlock_wrunlock(&env_rwlock);

    lua_pushboolean(L, 0 == retval);
    return 1;
}

static int env_ht_set(lua_State * L) {
    size_t kl = 0, vl = 0;
    const char *k = luaL_checklstring(L, 1, &kl);
    const char *v = luaL_checklstring(L, 2, &vl);
    int retval;

    mc_rwlock_wrlock(&env_rwlock);
    retval = mc_htable_set(env_htable, k, (int)kl, v, (int)vl);
    mc_rwlock_wrunlock(&env_rwlock);

    lua_pushboolean(L, 0 == retval);
    return 1;
}

static int env_ht_get(lua_State * L) {
    size_t l = 0;
    const char *k = luaL_checklstring(L, 1, &l);
    const mc_datum_t *dt;

    mc_rwlock_rdlock(&env_rwlock);
    dt = mc_htable_get(env_htable, k, (int)l);
    if (dt) {
        int vl = 0;
        const char *v = (const char *)mc_datum_value(dt, &vl);
        lua_pushlstring(L, v, vl);
    } else {
        lua_pushnil(L);
    }
    mc_rwlock_rdunlock(&env_rwlock);

    return 1;
}

static int env_ht_del(lua_State * L) {
    size_t l = 0;
    const char *k = luaL_checklstring(L, 1, &l);
    int retval;

    mc_rwlock_wrlock(&env_rwlock);
    retval = mc_htable_erase(env_htable, k, (int)l);
    mc_rwlock_wrunlock(&env_rwlock);

    lua_pushboolean(L, 0 == retval);
    return 1;
}

static int env_ht_keys(lua_State * L) {
    lua_Integer idx = 1;
    mc_htable_iter_t *iter = mc_htable_iter(env_htable);
    const mc_datum_t *dt;
    const char *k;
    int l;

    lua_newtable(L);

    mc_rwlock_rdlock(&env_rwlock);
    while (!!(dt = mc_htable_iter_next(iter))) {
        k = (const char *)mc_datum_key(dt, &l);

        lua_pushlstring(L, k, l);
        lua_seti(L, -2, idx++);
    }
    mc_rwlock_rdunlock(&env_rwlock);

    mc_htable_iter_destroy(iter);

    return 1;
}

int luaopen_sevo_env(lua_State* L) {
    luaL_Reg mod_env[] = {
        { "add", env_ht_add },
        { "set", env_ht_set },
        { "get", env_ht_get },
        { "del", env_ht_del },
        { "keys", env_ht_keys },
        { NULL, NULL }
    };

    luaX_register_module(L, "env", mod_env);
    return 0;
}
