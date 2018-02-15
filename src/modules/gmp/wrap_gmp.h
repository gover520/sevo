/*
 *  wrap_gmp.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __SONIC_WRAP_GMP_H__
#define __SONIC_WRAP_GMP_H__

#include <mini-gmp.h>
#include "runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct bigint_t {
        mpz_t   x;
    } bigint_t;

    long long mpz_get_ll(const mpz_t u);
    unsigned long long mpz_get_ull(const mpz_t u);
    void mpz_set_ll(mpz_t r, long long x);
    void mpz_set_ull(mpz_t r, unsigned long long x);

    bigint_t *luaX_checkbigint(lua_State *L, int index);
    int luaX_checkmpz(lua_State *L, int index, mpz_t r);

    bigint_t *new_bigint(lua_State *L);
    int luaopen_sonic_int(lua_State* L);

#ifdef __cplusplus
};
#endif

#endif  /* __SONIC_WRAP_GMP_H__ */
