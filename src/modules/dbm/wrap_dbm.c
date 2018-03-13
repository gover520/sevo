/*
 *  wrap_dbm.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_dbm.h"
#include "common/logger.h"
#include "common/vfs.h"
#include <lmdb.h>
#include <string.h>

typedef struct dbm_t {
    MDB_env     *env;
    MDB_dbi     dbi;
} dbm_t;

static const char g_meta_dbm[] = { CODE_NAME ".meta.dbm" };

#define luaX_checkdbm(L, idx)   (dbm_t *)luaL_checkudata(L, idx, g_meta_dbm)

static int MDB_CHECK(int errcode) {
    if (MDB_SUCCESS != errcode) {
        LG_ERR("mdb error: %s", mdb_strerror(errcode));
        return -1;
    }
    return 0;
}

static int dbm__gc(lua_State * L) {
    dbm_t *dbm = luaX_checkdbm(L, -1);
    if (dbm && dbm->env) {
        mdb_env_sync(dbm->env, 1);
        mdb_dbi_close(dbm->env, dbm->dbi);
        mdb_env_close(dbm->env);
        dbm->env = NULL;
    }
    return 0;
}

static int dbm_close(lua_State * L) {
    dbm_t *dbm = luaX_checkdbm(L, 1);

    mdb_env_sync(dbm->env, 1);
    mdb_dbi_close(dbm->env, dbm->dbi);
    mdb_env_close(dbm->env);
    dbm->env = NULL;

    return 0;
}

static int dbm_sync(lua_State * L) {
    dbm_t *dbm = luaX_checkdbm(L, 1);

    if (MDB_CHECK(mdb_env_sync(dbm->env, 1))) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int dbm_del(lua_State * L) {
    size_t l = 0;
    dbm_t *dbm = luaX_checkdbm(L, 1);
    const char *key = luaL_checklstring(L, 2, &l);
    MDB_txn* mdb_txn;
    MDB_val k;

    if (MDB_CHECK(mdb_txn_begin(dbm->env, NULL, 0, &mdb_txn))) {
        lua_pushboolean(L, 0);
        return 1;
    }

    k.mv_size = l;
    k.mv_data = (void *)key;

    if (MDB_CHECK(mdb_del(mdb_txn, dbm->dbi, &k, NULL))) {
        mdb_txn_abort(mdb_txn);
        lua_pushboolean(L, 0);
        return 1;
    }

    mdb_txn_commit(mdb_txn);
    lua_pushboolean(L, 1);
    return 1;
}

static int dbm_get(lua_State * L) {
    size_t l = 0;
    dbm_t *dbm = luaX_checkdbm(L, 1);
    const char *key = luaL_checklstring(L, 2, &l);
    MDB_txn* mdb_txn;
    MDB_val k, v;

    if (MDB_CHECK(mdb_txn_begin(dbm->env, NULL, MDB_RDONLY, &mdb_txn))) {
        lua_pushnil(L);
        return 1;
    }

    k.mv_size = l;
    k.mv_data = (void *)key;

    if (0 == mdb_get(mdb_txn, dbm->dbi, &k, &v)) {
        lua_pushlstring(L, (const char *)v.mv_data, v.mv_size);
    } else {
        lua_pushnil(L);
    }
    mdb_txn_abort(mdb_txn);

    return 1;
}

static int dbm_put(lua_State * L) {
    size_t kl = 0, vl = 0;
    dbm_t *dbm = luaX_checkdbm(L, 1);
    const char *key = luaL_checklstring(L, 2, &kl);
    const char *val = luaL_checklstring(L, 3, &vl);
    MDB_txn* mdb_txn;
    MDB_val k, v;

    if (MDB_CHECK(mdb_txn_begin(dbm->env, NULL, 0, &mdb_txn))) {
        lua_pushboolean(L, 0);
        return 1;
    }

    k.mv_size = kl;
    k.mv_data = (void *)key;
    v.mv_size = vl;
    v.mv_data = (void *)val;

    if (MDB_CHECK(mdb_put(mdb_txn, dbm->dbi, &k, &v, 0))) {
        mdb_txn_abort(mdb_txn);
        lua_pushboolean(L, 0);
        return 1;
    }

    mdb_txn_commit(mdb_txn);
    lua_pushboolean(L, 1);
    return 1;
}

static int dbm_keys(lua_State * L) {
    dbm_t *dbm = luaX_checkdbm(L, 1);
    MDB_txn* mdb_txn;
    MDB_cursor *mdb_cur;
    MDB_val k, v;
    lua_Integer idx = 1;

    if (MDB_CHECK(mdb_txn_begin(dbm->env, NULL, MDB_RDONLY, &mdb_txn))) {
        lua_pushnil(L);
        return 1;
    }

    if (MDB_CHECK(mdb_cursor_open(mdb_txn, dbm->dbi, &mdb_cur))) {
        mdb_txn_abort(mdb_txn);
        lua_pushnil(L);
        return 1;
    }

    if (MDB_CHECK(mdb_cursor_get(mdb_cur, &k, &v, MDB_FIRST))) {
        mdb_txn_abort(mdb_txn);
        lua_pushnil(L);
        return 1;
    }

    lua_newtable(L);

    do {
        lua_pushlstring(L, (const char *)k.mv_data, k.mv_size);
        lua_seti(L, -2, idx++);
    } while (MDB_SUCCESS == mdb_cursor_get(mdb_cur, &k, &v, MDB_NEXT));

    mdb_cursor_close(mdb_cur);
    mdb_txn_abort(mdb_txn);

    return 1;
}

static char *dbm_checkpath(char *p, const char *name) {
    if (!name || !name[0]) {
        return NULL;
    }

    if ('/' == name[0] || '\\' == name[0]) {
        strcpy(p, name);
    } else if (isalpha(name[0]) && ':' == name[1]) {
        strcpy(p, name);
    } else {
        sprintf(p, "%s/%s", identdir(), name);
        mc_path_format(p, MC_PATHSEP);
    }

    if (!mc_path_exist(p)) {
        mc_path_mkdir(p);
    }

    return mc_path_exist(p) ? p : NULL;
}

static int dbm_open(lua_State * L) {
    const char *path = luaL_checkstring(L, 1);
    char fullpath[MC_MAX_PATH] = { 0 };
    MDB_env *env;
    MDB_txn *txn;
    MDB_dbi dbi;
    dbm_t *dbm;

    if (!dbm_checkpath(fullpath, path)) {
        return luaL_error(L, "dbm path is invalid.");
    }

    if (MDB_CHECK(mdb_env_create(&env))) {
        return luaL_error(L, "mdb env create failed.");
    }

    if (MDB_CHECK(mdb_env_open(env, fullpath, 0, 0664))) {
        mdb_env_close(env);
        return luaL_error(L, "mdb env open failed.");
    }

    if (MDB_CHECK(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn))) {
        mdb_env_close(env);
        return luaL_error(L, "mdb txn begin failed.");
    }

    if (MDB_CHECK(mdb_dbi_open(txn, NULL, MDB_CREATE, &dbi))) {
        mdb_txn_abort(txn);
        mdb_env_close(env);
        return luaL_error(L, "mdb dbi open failed.");
    }

    mdb_txn_commit(txn);

    dbm = (dbm_t *)luaX_newuserdata(L, g_meta_dbm, sizeof(dbm_t));

    dbm->env = env;
    dbm->dbi = dbi;

    return 1;
}

int luaopen_sevo_dbm(lua_State* L) {
    luaL_Reg meta_dbm[] = {
        { "__gc", dbm__gc },
        { "close", dbm_close },
        { "sync", dbm_sync },
        { "del", dbm_del },
        { "get", dbm_get },
        { "put", dbm_put },
        { "keys", dbm_keys },
        { NULL, NULL }
    };
    luaL_Reg mod_dbm[] = {
        { "open", dbm_open },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_dbm, meta_dbm);
    luaX_register_module(L, "dbm", mod_dbm);

    return 0;
}
