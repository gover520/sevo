/*
 *  wrap_gmp.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_gmp.h"
#include "common/version.h"

static const char g_meta_int[] = { CODE_NAME ".meta.int" };

long long mpz_get_ll(const mpz_t u) {
    long long r = 0;
    mpz_export(&r, NULL, -1, sizeof(long long), 0, 0, u);
    return (mpz_sgn(u) < 0) ? -r : r;
}

unsigned long long mpz_get_ull(const mpz_t u) {
    unsigned long long r = 0;
    mpz_export(&r, NULL, -1, sizeof(unsigned long long), 0, 0, u);
    return r;
}

void mpz_set_ll(mpz_t r, long long x) {
    if (x < 0) {
        x = -x;
        mpz_import(r, 1, -1, sizeof(long long), 0, 0, &x);
        mpz_neg(r, r);
    }
    else {
        mpz_import(r, 1, -1, sizeof(long long), 0, 0, &x);
    }
}

void mpz_set_ull(mpz_t r, unsigned long long x) {
    mpz_import(r, 1, -1, sizeof(unsigned long long), 0, 0, &x);
}

bigint_t *luaX_checkbigint(lua_State *L, int index) {
    if (!lua_isuserdata(L, index)) {
        return NULL;
    }
    if (!luaL_checkudata(L, index, g_meta_int)) {
        return NULL;
    }
    return (bigint_t *)lua_touserdata(L, index);
}

int luaX_checkmpz(lua_State *L, int index, mpz_t r) {
    static const char errmsg[] = { "Invalid operand. Expected 'integer' or 'number'" };

    bigint_t *bi = luaX_checkbigint(L, index);
    if (bi) {
        mpz_set(r, bi->x);
        return 1;
    }

    if (lua_isinteger(L, index)) {
        mpz_set_ll(r, lua_tointeger(L, index));
        return 1;
    }

    if (lua_isstring(L, index)) {
        if (0 != mpz_set_str(r, lua_tostring(L, index), 0)) {
            return luaL_error(L, errmsg);
        }
        return 1;
    }

    if (lua_isnumber(L, index)) {
        mpz_set_d(r, lua_tonumber(L, index));
        return 1;
    }

    return luaL_error(L, errmsg);
}

bigint_t *new_bigint(lua_State *L) {
    bigint_t *bi = luaX_newuserdata(L, g_meta_int, sizeof(struct bigint_t));
    mpz_init(bi->x);
    return bi;
}

static int bigint_new(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    return luaX_checkmpz(L, 1, bi->x);
}

static int bigint__gc(lua_State* L) {
    bigint_t *bi = luaX_checkbigint(L, -1);
    if (bi) {
        mpz_clear(bi->x);
    }
    return 0;
}

static int bigint_add(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_add(bi->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_sub(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_sub(bi->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_mul(lua_State* L) {
    bigint_t *bi =new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_mul(bi->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_div(lua_State* L) {
    bigint_t *q = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_tdiv_q(q->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_mod(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_mod(bi->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_pow(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_pow_ui(bi->x, a, mpz_get_ui(b));

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_unm(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a;

    mpz_init(a);

    luaX_checkmpz(L, 1, a);

    mpz_neg(bi->x, a);

    mpz_clear(a);

    return 1;
}

static int bigint_idiv(lua_State* L) {
    return bigint_div(L);
}

static int bigint_band(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_and(bi->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_bor(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_ior(bi->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_bxor(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_xor(bi->x, a, b);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_bnot(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);

    mpz_neg(b, a);
    mpz_sub_ui(bi->x, b, 1);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_shl(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_mul_2exp(bi->x, a, mpz_get_ui(b));

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_shr(lua_State* L) {
    bigint_t *bi = new_bigint(L);
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    mpz_tdiv_r_2exp(bi->x, a, mpz_get_ui(b));

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_len(lua_State* L) {
    bigint_t *bi = luaX_checkbigint(L, 1);
    lua_pushinteger(L, bi ? mpz_get_ll(bi->x) : 0);
    return 1;
}

static int bigint_eq(lua_State* L) {
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    lua_pushboolean(L, 0 == mpz_cmp(a, b));

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_lt(lua_State* L) {
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    lua_pushboolean(L, mpz_cmp(a, b) < 0);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_le(lua_State* L) {
    mpz_t a, b;

    mpz_init(a);
    mpz_init(b);

    luaX_checkmpz(L, 1, a);
    luaX_checkmpz(L, 2, b);

    lua_pushboolean(L, mpz_cmp(a, b) <= 0);

    mpz_clear(a);
    mpz_clear(b);

    return 1;
}

static int bigint_tostring(lua_State* L) {
    char *s;
    mpz_t a;

    mpz_init(a);

    luaX_checkmpz(L, 1, a);

    s = mpz_get_str(NULL, 10, a);
    lua_pushstring(L, s);

    mpz_clear(a);
    mc_free(s);

    return 1;
}

static int bigint_concat(lua_State* L) {
    if (!luaL_callmeta(L, 1, "__tostring")) {
        lua_pushvalue(L, 1);
    }

    if (!luaL_callmeta(L, 2, "__tostring")) {
        lua_pushvalue(L, 2);
    }

    lua_concat(L, 2);

    return 1;
}

int luaopen_sevo_int(lua_State* L) {
    luaL_Reg mod_int[] = {
        { "int", bigint_new },
        { NULL, NULL }
    };
    luaL_Reg meta_int[] = {
        /* operators */
        { "__gc", bigint__gc },
        { "__add", bigint_add },
        { "__sub", bigint_sub },
        { "__mul", bigint_mul },
        { "__div", bigint_div },
        { "__mod", bigint_mod },
        { "__pow", bigint_pow },
        { "__unm", bigint_unm },
        { "__idiv", bigint_idiv },
        { "__band", bigint_band },
        { "__bor", bigint_bor },
        { "__bxor", bigint_bxor },
        { "__bnot", bigint_bnot },
        { "__shl", bigint_shl },
        { "__shr", bigint_shr },
        { "__len", bigint_len },
        { "__eq",  bigint_eq },
        { "__lt",  bigint_lt },
        { "__le",  bigint_le },
        { "__tostring", bigint_tostring },
        { "__concat", bigint_concat },
        /* methods */
        { "add", bigint_add },
        { "sub", bigint_sub },
        { "mul", bigint_mul },
        { "div", bigint_div },
        { "mod", bigint_mod },
        { "pow", bigint_pow },
        { "unm", bigint_unm },
        { "band", bigint_band },
        { "bor", bigint_bor },
        { "bxor", bigint_bxor },
        { "bnot", bigint_bnot },
        { "shl", bigint_shl },
        { "shr", bigint_shr },
        { "eq", bigint_eq },
        { "lt",  bigint_lt },
        { "le",  bigint_le },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_int, meta_int);
    luaX_register_funcs(L, mod_int);

    return 0;
}
