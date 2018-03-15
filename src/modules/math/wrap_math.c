/*
*  wrap_math.c
*
*  copyright (c) 2018 Xiongfei Shi
*
*  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
*  license: Apache-2.0
*/

#include "wrap_math.h"
#include "common/vector.h"
#include "common/matrix.h"

typedef struct l_vec2_t {
    vec2_t      vec2;
} l_vec2_t;

typedef struct l_vec3_t {
    vec3_t      vec3;
} l_vec3_t;

typedef struct l_mat33_t {
    mat33_t     mat33;
} l_mat33_t;

#define v2(l)   l->vec2
#define v3(l)   l->vec3
#define m33(l)  l->mat33

static const char g_meta_vec2[] = { CODE_NAME ".meta.vec2" };
static const char g_meta_vec3[] = { CODE_NAME ".meta.vec3" };
static const char g_meta_mat33[] = { CODE_NAME ".meta.mat33" };

#define luaX_checkvec2(L, idx)  (l_vec2_t *)luaL_checkudata(L, idx, g_meta_vec2)
#define luaX_checkvec3(L, idx)  (l_vec3_t *)luaL_checkudata(L, idx, g_meta_vec3)
#define luaX_checkmat33(L, idx) (l_mat33_t *)luaL_checkudata(L, idx, g_meta_mat33)
#define new_vec2(L)             (l_vec2_t *)luaX_newuserdata(L, g_meta_vec2, sizeof(l_vec2_t));
#define new_vec3(L)             (l_vec3_t *)luaX_newuserdata(L, g_meta_vec3, sizeof(l_vec3_t));
#define new_mat33(L)            (l_mat33_t *)luaX_newuserdata(L, g_meta_mat33, sizeof(l_mat33_t));

static int l_math_concat(lua_State* L) {
    if (!luaL_callmeta(L, 1, "__tostring")) {
        lua_pushvalue(L, 1);
    }

    if (!luaL_callmeta(L, 2, "__tostring")) {
        lua_pushvalue(L, 2);
    }

    lua_concat(L, 2);

    return 1;
}

static int l_vec2_add(lua_State* L) {
    l_vec2_t *a = luaX_checkvec2(L, 1);
    l_vec2_t *b = luaX_checkvec2(L, 2);
    l_vec2_t *r = new_vec2(L)

    vec2_add(v2(r), v2(a), v2(b));

    return 1;
}

static int l_vec2_sub(lua_State* L) {
    l_vec2_t *a = luaX_checkvec2(L, 1);
    l_vec2_t *b = luaX_checkvec2(L, 2);
    l_vec2_t *r = new_vec2(L);

    vec2_sub(v2(r), v2(a), v2(b));

    return 1;
}

static int l_vec2_mul(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);
    real_t s = (real_t)luaL_checknumber(L, 2);
    l_vec2_t *r = new_vec2(L);

    vec2_mul(v2(r), v2(v), s);

    return 1;
}

static int l_vec2_div(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);
    real_t s = (real_t)luaL_checknumber(L, 2);
    l_vec2_t *r = new_vec2(L);

    vec2_div(v2(r), v2(v), s);

    return 1;
}

static int l_vec2_unm(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);
    l_vec2_t *r = new_vec2(L);

    vec2_neg(v2(r), v2(v));

    return 1;
}

static int l_vec2_len(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);

    lua_pushnumber(L, (lua_Number)vec2_len(v2(v)));

    return 1;
}

static int l_vec2_eq(lua_State* L) {
    l_vec2_t *a = luaX_checkvec2(L, 1);
    l_vec2_t *b = luaX_checkvec2(L, 2);

    lua_pushboolean(L, r_equal(vx(v2(a)), vx(v2(b)))
        && r_equal(vy(v2(a)), vy(v2(b))));

    return 1;
}

static int l_vec2_tostring(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);
    char buffer[64] = { 0 };

    sprintf(buffer, "vec2(%lf %lf)", vx(v2(v)), vy(v2(v)));
    lua_pushstring(L, buffer);

    return 1;
}

static int l_vec2_dim(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);

    lua_pushinteger(L, 2);

    return 1;
}

static int l_vec2_lensq(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);

    lua_pushnumber(L, (lua_Number)vec2_lensq(v2(v)));

    return 1;
}

static int l_vec2_dot(lua_State* L) {
    l_vec2_t *a = luaX_checkvec2(L, 1);
    l_vec2_t *b = luaX_checkvec2(L, 2);

    lua_pushnumber(L, (lua_Number)vec2_dot(v2(a), v2(b)));

    return 1;
}

static int l_vec2_cross(lua_State* L) {
    l_vec2_t *a = luaX_checkvec2(L, 1);
    l_vec2_t *b = luaX_checkvec2(L, 2);

    lua_pushnumber(L, (lua_Number)vec2_cross(v2(a), v2(b)));

    return 1;
}

static int l_vec2_normalize(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);
    real_t len = (real_t)luaL_optnumber(L, 2, r_one);

    lua_pushnumber(L, vec2_normalize(v2(v), len));

    return 1;
}

static int l_vec2_rotate(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);
    real_t theta = (real_t)luaL_checknumber(L, 2);
    l_vec2_t *r = new_vec2(L);

    vec2_rotate(v2(r), v2(v), theta);

    return 1;
}

static int l_vec2_xy(lua_State* L) {
    l_vec2_t *v = luaX_checkvec2(L, 1);

    lua_pushnumber(L, vx(v2(v)));
    lua_pushnumber(L, vy(v2(v)));

    return 2;
}

static int l_vec3_add(lua_State* L) {
    l_vec3_t *a = luaX_checkvec3(L, 1);
    l_vec3_t *b = luaX_checkvec3(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_add(v3(r), v3(a), v3(b));

    return 1;
}

static int l_vec3_sub(lua_State* L) {
    l_vec3_t *a = luaX_checkvec3(L, 1);
    l_vec3_t *b = luaX_checkvec3(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_sub(v3(r), v3(a), v3(b));

    return 1;
}

static int l_vec3_mul(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    real_t s = (real_t)luaL_checknumber(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_mul(v3(r), v3(v), s);

    return 1;
}

static int l_vec3_div(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    real_t s = (real_t)luaL_checknumber(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_div(v3(r), v3(v), s);

    return 1;
}

static int l_vec3_unm(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    l_vec3_t *r = new_vec3(L);

    vec3_neg(v3(r), v3(v));

    return 1;
}

static int l_vec3_len(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);

    lua_pushnumber(L, (lua_Number)vec3_len(v3(v)));

    return 1;
}

static int l_vec3_eq(lua_State* L) {
    l_vec3_t *a = luaX_checkvec3(L, 1);
    l_vec3_t *b = luaX_checkvec3(L, 2);

    lua_pushboolean(L, r_equal(vx(v3(a)), vx(v3(b)))
        && r_equal(vy(v3(a)), vy(v3(b)))
        && r_equal(vz(v3(a)), vz(v3(b))));

    return 1;
}

static int l_vec3_tostring(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    char buffer[64] = { 0 };

    sprintf(buffer, "vec3(%lf %lf %lf)", vx(v3(v)), vy(v3(v)), vz(v3(v)));
    lua_pushstring(L, buffer);

    return 1;
}

static int l_vec3_dim(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);

    lua_pushinteger(L, 3);

    return 1;
}

static int l_vec3_lensq(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);

    lua_pushnumber(L, (lua_Number)vec3_lensq(v3(v)));

    return 1;
}

static int l_vec3_dot(lua_State* L) {
    l_vec3_t *a = luaX_checkvec3(L, 1);
    l_vec3_t *b = luaX_checkvec3(L, 2);

    lua_pushnumber(L, (lua_Number)vec3_dot(v3(a), v3(b)));

    return 1;
}

static int l_vec3_cross(lua_State* L) {
    l_vec3_t *a = luaX_checkvec3(L, 1);
    l_vec3_t *b = luaX_checkvec3(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_cross(v3(r), v3(a), v3(b));

    return 1;
}

static int l_vec3_normalize(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    real_t len = (real_t)luaL_optnumber(L, 2, r_one);

    lua_pushnumber(L, vec3_normalize(v3(v), len));

    return 1;
}

static int l_vec3_rotatex(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    real_t theta = (real_t)luaL_checknumber(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_rotate_x(v3(r), v3(v), theta);

    return 1;
}

static int l_vec3_rotatey(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    real_t theta = (real_t)luaL_checknumber(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_rotate_y(v3(r), v3(v), theta);

    return 1;
}

static int l_vec3_rotatez(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);
    real_t theta = (real_t)luaL_checknumber(L, 2);
    l_vec3_t *r = new_vec3(L);

    vec3_rotate_z(v3(r), v3(v), theta);

    return 1;
}

static int l_vec3_xyz(lua_State* L) {
    l_vec3_t *v = luaX_checkvec3(L, 1);

    lua_pushnumber(L, vx(v3(v)));
    lua_pushnumber(L, vy(v3(v)));
    lua_pushnumber(L, vz(v3(v)));

    return 3;
}

static int l_mat33_add(lua_State* L) {
    l_mat33_t *a = luaX_checkmat33(L, 1);
    l_mat33_t *b = luaX_checkmat33(L, 2);
    l_mat33_t *m = new_mat33(L);

    mat33_add(m33(m), m33(a), m33(b));

    return 1;
}

static int l_mat33_sub(lua_State* L) {
    l_mat33_t *a = luaX_checkmat33(L, 1);
    l_mat33_t *b = luaX_checkmat33(L, 2);
    l_mat33_t *m = new_mat33(L);

    mat33_sub(m33(m), m33(a), m33(b));

    return 1;
}

static int l_mat33_mul(lua_State* L) {
    l_mat33_t *a = luaX_checkmat33(L, 1);
    l_mat33_t *b = luaX_checkmat33(L, 2);
    l_mat33_t *m = new_mat33(L);

    mat33_mul(m33(m), m33(a), m33(b));

    return 1;
}

static int l_mat33_unm(lua_State* L) {
    l_mat33_t *e = luaX_checkmat33(L, 1);
    l_mat33_t *m = new_mat33(L);

    mat33_inverse(m33(m), m33(e));

    return 1;
}

static int l_mat33_eq(lua_State* L) {
    l_mat33_t *a = luaX_checkmat33(L, 1);
    l_mat33_t *b = luaX_checkmat33(L, 2);

    lua_pushboolean(L, r_equal(m33(a)[0], m33(b)[0])
        && r_equal(m33(a)[1], m33(b)[1])
        && r_equal(m33(a)[2], m33(b)[2])
        && r_equal(m33(a)[3], m33(b)[3])
        && r_equal(m33(a)[4], m33(b)[4])
        && r_equal(m33(a)[5], m33(b)[5])
        && r_equal(m33(a)[6], m33(b)[6])
        && r_equal(m33(a)[7], m33(b)[7])
        && r_equal(m33(a)[8], m33(b)[8]));

    return 1;
}

static int l_mat33_tostring(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);
    char buffer[160] = { 0 };

    sprintf(buffer, "mat33(%lf %lf %lf, %lf %lf %lf, %lf %lf %lf)", 
        m33(m)[0], m33(m)[3], m33(m)[6],
        m33(m)[1], m33(m)[4], m33(m)[7],
        m33(m)[2], m33(m)[5], m33(m)[8]);
    lua_pushstring(L, buffer);

    return 1;
}

static int l_mat33_dim(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);

    lua_pushinteger(L, 3);
    lua_pushinteger(L, 3);

    return 2;
}

static int l_mat33_identity(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);

    mat33_identity(m33(m));

    return 0;
}

static int l_mat33_determinant(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);

    lua_pushnumber(L, mat33_determinant(m33(m)));

    return 1;
}

static int l_mat33_transpose(lua_State* L) {
    l_mat33_t *e = luaX_checkmat33(L, 1);
    l_mat33_t *m = new_mat33(L);

    mat33_transpose(m33(m), m33(e));

    return 1;
}

static int l_mat33_transformation(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);
    real_t x = (real_t)luaL_checknumber(L, 2);
    real_t y = (real_t)luaL_checknumber(L, 3);
    real_t theta = (real_t)luaL_checknumber(L, 4);
    real_t sx = (real_t)luaL_checknumber(L, 5);
    real_t sy = (real_t)luaL_checknumber(L, 6);
    real_t ox = (real_t)luaL_checknumber(L, 7);
    real_t oy = (real_t)luaL_checknumber(L, 8);
    real_t kx = (real_t)luaL_checknumber(L, 9);
    real_t ky = (real_t)luaL_checknumber(L, 10);

    mat33_transformation(m33(m), x, y, theta, sx, sy, ox, oy, kx, ky);

    return 0;
}

static int l_mat33_transformxy(lua_State* L) {
    int top = lua_gettop(L);
    l_mat33_t *m = luaX_checkmat33(L, 1);
    l_vec2_t *r = new_vec2(L);

    if (3 == top) {
        vec2_t v;
        vx(v) = (real_t)luaL_checknumber(L, 2);
        vy(v) = (real_t)luaL_checknumber(L, 3);

        mat33_transformxy(v2(r), m33(m), v);
    } else {
        l_vec2_t *v = luaX_checkvec2(L, 2);

        mat33_transformxy(v2(r), m33(m), v2(v));
    }

    return 1;
}

static int l_mat33_transformxyz(lua_State* L) {
    int top = lua_gettop(L);
    l_mat33_t *m = luaX_checkmat33(L, 1);
    l_vec3_t *r = new_vec3(L);

    if (4 == top) {
        vec3_t v;
        vx(v) = (real_t)luaL_checknumber(L, 2);
        vy(v) = (real_t)luaL_checknumber(L, 3);
        vz(v) = (real_t)luaL_checknumber(L, 4);

        mat33_transformxyz(v3(r), m33(m), v);
    } else {
        l_vec3_t *v = luaX_checkvec3(L, 2);

        mat33_transformxyz(v3(r), m33(m), v3(v));
    }

    return 1;
}

static int l_mat33_row(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);
    int idx = (int)luaL_checkinteger(L, 2);
    l_vec3_t *r = new_vec3(L);

    if ((idx < 1) || (idx > 3)) {
        return luaL_error(L, "Invalid operand. Expected 1, 2 or 3");
    }

    vx(v3(r)) = m33(m)[idx - 1];
    vy(v3(r)) = m33(m)[idx + 2];
    vz(v3(r)) = m33(m)[idx + 5];

    return 1;
}

static int l_mat33_column(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);
    int idx = (int)luaL_checkinteger(L, 2);
    l_vec3_t *r = new_vec3(L);

    if ((idx < 1) || (idx > 3)) {
        return luaL_error(L, "Invalid operand. Expected 1, 2 or 3");
    }

    idx = (idx - 1) * 3;

    vx(v3(r)) = m33(m)[idx];
    vy(v3(r)) = m33(m)[idx + 1];
    vz(v3(r)) = m33(m)[idx + 2];

    return 1;
}

static int l_vec2_new(lua_State* L) {
    int top = lua_gettop(L);
    l_vec2_t *v = new_vec2(L);

    if (2 == top) {
        vx(v2(v)) = (real_t)luaL_checknumber(L, 1);
        vy(v2(v)) = (real_t)luaL_checknumber(L, 2);
    } else if (1 == top) {
        vx(v2(v)) = vy(v2(v)) = (real_t)luaL_checknumber(L, 1);
    } else {
        vx(v2(v)) = vy(v2(v)) = r_zero;
    }

    return 1;
}

static int l_vec3_new(lua_State* L) {
    int top = lua_gettop(L);
    l_vec3_t *v = new_vec3(L);

    if (3 == top) {
        vx(v3(v)) = (real_t)luaL_checknumber(L, 1);
        vy(v3(v)) = (real_t)luaL_checknumber(L, 2);
        vz(v3(v)) = (real_t)luaL_checknumber(L, 3);
    }
    else if (2 == top) {
        vx(v3(v)) = (real_t)luaL_checknumber(L, 1);
        vy(v3(v)) = (real_t)luaL_checknumber(L, 2);
        vz(v3(v)) = r_zero;
    } else if (1 == top) {
        vx(v3(v)) = vy(v3(v)) = vz(v3(v)) = (real_t)luaL_checknumber(L, 1);
    }
    else {
        vx(v3(v)) = vy(v3(v)) = vz(v3(v)) = r_zero;
    }

    return 1;
}

static int l_mat33_new(lua_State* L) {
    int top = lua_gettop(L);
    l_mat33_t *m = new_mat33(L);

    if (9 == top) {
        m33(m)[0] = (real_t)luaL_checknumber(L, 1);
        m33(m)[3] = (real_t)luaL_checknumber(L, 2);
        m33(m)[6] = (real_t)luaL_checknumber(L, 3);

        m33(m)[1] = (real_t)luaL_checknumber(L, 4);
        m33(m)[4] = (real_t)luaL_checknumber(L, 5);
        m33(m)[7] = (real_t)luaL_checknumber(L, 6);

        m33(m)[2] = (real_t)luaL_checknumber(L, 7);
        m33(m)[5] = (real_t)luaL_checknumber(L, 8);
        m33(m)[8] = (real_t)luaL_checknumber(L, 9);
    } else if (3 == top) {
        mat33_identity(m33(m));

        m33(m)[0] = (real_t)luaL_checknumber(L, 1);
        m33(m)[4] = (real_t)luaL_checknumber(L, 2);
        m33(m)[8] = (real_t)luaL_checknumber(L, 3);
    } else {
        mat33_identity(m33(m));
    }

    return 1;
}

static int l_radian(lua_State* L) {
    static const real_t deg2rad = (real_t)(MC_PI / 180.0);
    real_t deg = (real_t)luaL_checknumber(L, 1);

    lua_pushnumber(L, deg * deg2rad);

    return 1;
}

static int l_degree(lua_State* L) {
    static const real_t rad2deg = (real_t)(180.0 / MC_PI);
    real_t rad = (real_t)luaL_checknumber(L, 1);

    lua_pushnumber(L, rad * rad2deg);

    return 1;
}

int luaopen_sevo_math(lua_State* L) {
    luaL_Reg mod_math[] = {
        { "vec2", l_vec2_new },
        { "vec3", l_vec3_new },
        { "mat33", l_mat33_new },
        { "radian", l_radian },
        { "degree", l_degree },
        { NULL, NULL }
    };
    luaL_Reg meta_vec2[] = {
        { "__add", l_vec2_add },
        { "__sub", l_vec2_sub },
        { "__mul", l_vec2_mul },
        { "__div", l_vec2_div },
        { "__unm", l_vec2_unm },
        { "__len", l_vec2_len },
        { "__eq",  l_vec2_eq },
        { "__tostring", l_vec2_tostring },
        { "__concat", l_math_concat },
        { "dim", l_vec2_dim },
        { "lensq", l_vec2_lensq },
        { "dot", l_vec2_dot },
        { "cross", l_vec2_cross },
        { "norm", l_vec2_normalize },
        { "rotate", l_vec2_rotate },
        { "xy", l_vec2_xy },
        { NULL, NULL }
    };
    luaL_Reg meta_vec3[] = {
        { "__add", l_vec3_add },
        { "__sub", l_vec3_sub },
        { "__mul", l_vec3_mul },
        { "__div", l_vec3_div },
        { "__unm", l_vec3_unm },
        { "__len", l_vec3_len },
        { "__eq",  l_vec3_eq },
        { "__tostring", l_vec3_tostring },
        { "__concat", l_math_concat },
        { "dim", l_vec3_dim },
        { "lensq", l_vec3_lensq },
        { "dot", l_vec3_dot },
        { "cross", l_vec3_cross },
        { "norm", l_vec3_normalize },
        { "rotatex", l_vec3_rotatex },
        { "rotatey", l_vec3_rotatey },
        { "rotatez", l_vec3_rotatez },
        { "xyz", l_vec3_xyz },
        { NULL, NULL }
    };
    luaL_Reg meta_mat33[] = {
        { "__add", l_mat33_add },
        { "__sub", l_mat33_sub },
        { "__mul", l_mat33_mul },
        { "__unm", l_mat33_unm },
        { "__eq",  l_mat33_eq },
        { "__tostring", l_mat33_tostring },
        { "__concat", l_math_concat },
        { "dim", l_mat33_dim },
        { "identity", l_mat33_identity },
        { "det", l_mat33_determinant },
        { "transpose", l_mat33_transpose },
        { "transform", l_mat33_transformation },
        { "transformxy", l_mat33_transformxy },
        { "transformxyz", l_mat33_transformxyz },
        { "row", l_mat33_row },
        { "column", l_mat33_column },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_vec2, meta_vec2);
    luaX_register_type(L, g_meta_vec3, meta_vec3);
    luaX_register_type(L, g_meta_mat33, meta_mat33);
    luaX_register_module(L, "math", mod_math);

    return 0;
}
