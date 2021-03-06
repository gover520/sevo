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
#include "common/quaternion.h"

typedef struct l_vec2_t {
    vec2_t      vec2;
} l_vec2_t;

typedef struct l_vec3_t {
    vec3_t      vec3;
} l_vec3_t;

typedef struct l_mat33_t {
    mat33_t     mat33;
} l_mat33_t;

typedef struct l_quat_t {
    quat_t      quat;
} l_quat_t;

#define v2(l)   l->vec2
#define v3(l)   l->vec3
#define m3(l)   l->mat33
#define qr(l)   l->quat

static const char g_meta_vec2[] = { CODE_NAME ".meta.vec2" };
static const char g_meta_vec3[] = { CODE_NAME ".meta.vec3" };
static const char g_meta_mat33[] = { CODE_NAME ".meta.mat33" };
static const char g_meta_quat[] = { CODE_NAME ".meta.quat" };

#define luaX_checkvec2(L, idx)  (l_vec2_t *)luaL_checkudata(L, idx, g_meta_vec2)
#define luaX_checkvec3(L, idx)  (l_vec3_t *)luaL_checkudata(L, idx, g_meta_vec3)
#define luaX_checkmat33(L, idx) (l_mat33_t *)luaL_checkudata(L, idx, g_meta_mat33)
#define luaX_checkquat(L, idx)  (l_quat_t *)luaL_checkudata(L, idx, g_meta_quat)
#define new_vec2(L)             (l_vec2_t *)luaX_newuserdata(L, g_meta_vec2, sizeof(l_vec2_t));
#define new_vec3(L)             (l_vec3_t *)luaX_newuserdata(L, g_meta_vec3, sizeof(l_vec3_t));
#define new_mat33(L)            (l_mat33_t *)luaX_newuserdata(L, g_meta_mat33, sizeof(l_mat33_t));
#define new_quat(L)             (l_quat_t *)luaX_newuserdata(L, g_meta_quat, sizeof(l_quat_t));

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
    l_vec2_t *r = new_vec2(L);

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

    mat33_add(m3(m), m3(a), m3(b));

    return 1;
}

static int l_mat33_sub(lua_State* L) {
    l_mat33_t *a = luaX_checkmat33(L, 1);
    l_mat33_t *b = luaX_checkmat33(L, 2);
    l_mat33_t *m = new_mat33(L);

    mat33_sub(m3(m), m3(a), m3(b));

    return 1;
}

static int l_mat33_mul(lua_State* L) {
    l_mat33_t *a = luaX_checkmat33(L, 1);
    l_mat33_t *b = luaX_checkmat33(L, 2);
    l_mat33_t *m = new_mat33(L);

    mat33_mul(m3(m), m3(a), m3(b));

    return 1;
}

static int l_mat33_unm(lua_State* L) {
    l_mat33_t *e = luaX_checkmat33(L, 1);
    l_mat33_t *m = new_mat33(L);

    mat33_inverse(m3(m), m3(e));

    return 1;
}

static int l_mat33_eq(lua_State* L) {
    l_mat33_t *a = luaX_checkmat33(L, 1);
    l_mat33_t *b = luaX_checkmat33(L, 2);

    lua_pushboolean(L, r_equal(e11(m3(a)), e11(m3(b)))
        && r_equal(e21(m3(a)), e21(m3(b)))
        && r_equal(e31(m3(a)), e31(m3(b)))
        && r_equal(e12(m3(a)), e12(m3(b)))
        && r_equal(e22(m3(a)), e22(m3(b)))
        && r_equal(e32(m3(a)), e32(m3(b)))
        && r_equal(e13(m3(a)), e13(m3(b)))
        && r_equal(e23(m3(a)), e23(m3(b)))
        && r_equal(e33(m3(a)), e33(m3(b))));

    return 1;
}

static int l_mat33_tostring(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);
    char buffer[160] = { 0 };

    sprintf(buffer, "mat33(%lf %lf %lf, %lf %lf %lf, %lf %lf %lf)", 
        e11(m3(m)), e12(m3(m)), e13(m3(m)),
        e21(m3(m)), e22(m3(m)), e23(m3(m)),
        e31(m3(m)), e32(m3(m)), e33(m3(m)));
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

    mat33_identity(m3(m));

    return 0;
}

static int l_mat33_determinant(lua_State* L) {
    l_mat33_t *m = luaX_checkmat33(L, 1);

    lua_pushnumber(L, mat33_determinant(m3(m)));

    return 1;
}

static int l_mat33_transpose(lua_State* L) {
    l_mat33_t *e = luaX_checkmat33(L, 1);
    l_mat33_t *m = new_mat33(L);

    mat33_transpose(m3(m), m3(e));

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

    mat33_transformation(m3(m), x, y, theta, sx, sy, ox, oy, kx, ky);

    return 0;
}

static int l_mat33_transform2(lua_State* L) {
    int top = lua_gettop(L);
    l_mat33_t *m = luaX_checkmat33(L, 1);
    l_vec2_t *r = new_vec2(L);

    if (3 == top) {
        vec2_t v;
        vx(v) = (real_t)luaL_checknumber(L, 2);
        vy(v) = (real_t)luaL_checknumber(L, 3);

        mat33_transformxy(v2(r), m3(m), v);
    } else {
        l_vec2_t *v = luaX_checkvec2(L, 2);

        mat33_transformxy(v2(r), m3(m), v2(v));
    }

    return 1;
}

static int l_mat33_transform3(lua_State* L) {
    int top = lua_gettop(L);
    l_mat33_t *m = luaX_checkmat33(L, 1);
    l_vec3_t *r = new_vec3(L);

    if (4 == top) {
        vec3_t v;
        vx(v) = (real_t)luaL_checknumber(L, 2);
        vy(v) = (real_t)luaL_checknumber(L, 3);
        vz(v) = (real_t)luaL_checknumber(L, 4);

        mat33_transformxyz(v3(r), m3(m), v);
    } else {
        l_vec3_t *v = luaX_checkvec3(L, 2);

        mat33_transformxyz(v3(r), m3(m), v3(v));
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

    vx(v3(r)) = m3(m)[idx - 1];
    vy(v3(r)) = m3(m)[idx + 2];
    vz(v3(r)) = m3(m)[idx + 5];

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

    vx(v3(r)) = m3(m)[idx];
    vy(v3(r)) = m3(m)[idx + 1];
    vz(v3(r)) = m3(m)[idx + 2];

    return 1;
}

static int l_quat_add(lua_State* L) {
    l_quat_t *a = luaX_checkquat(L, 1);
    l_quat_t *b = luaX_checkquat(L, 2);
    l_quat_t *r = new_quat(L);

    quat_add(qr(r), qr(a), qr(b));

    return 1;
}

static int l_quat_sub(lua_State* L) {
    l_quat_t *a = luaX_checkquat(L, 1);
    l_quat_t *b = luaX_checkquat(L, 2);
    l_quat_t *r = new_quat(L);

    quat_sub(qr(r), qr(a), qr(b));

    return 1;
}

static int l_quat_mul(lua_State* L) {
    l_quat_t *a = luaX_checkquat(L, 1);
    l_quat_t *b = luaX_checkquat(L, 2);
    l_quat_t *r = new_quat(L);

    quat_mul(qr(r), qr(a), qr(b));

    return 1;
}

static int l_quat_unm(lua_State* L) {
    l_quat_t *q = luaX_checkquat(L, 1);
    l_quat_t *r = new_quat(L);

    quat_neg(qr(r), qr(q));

    return 1;
}

static int l_quat_len(lua_State* L) {
    l_quat_t *q = luaX_checkquat(L, 1);

    lua_pushnumber(L, (lua_Number)quat_len(qr(q)));

    return 1;
}

static int l_quat_eq(lua_State* L) {
    l_quat_t *a = luaX_checkquat(L, 1);
    l_quat_t *b = luaX_checkquat(L, 2);

    lua_pushboolean(L, r_equal(qw(qr(a)), qw(qr(b)))
        && r_equal(qx(qr(a)), qx(qr(b)))
        && r_equal(qy(qr(a)), qy(qr(b)))
        && r_equal(qz(qr(a)), qz(qr(b))));

    return 1;
}

static int l_quat_tostring(lua_State* L) {
    l_quat_t *q = luaX_checkquat(L, 1);
    char buffer[64] = { 0 };

    sprintf(buffer, "quat(%lf %lf %lf %lf)", qw(qr(q)), qx(qr(q)), qy(qr(q)), qz(qr(q)));
    lua_pushstring(L, buffer);

    return 1;
}

static int l_quat_lensq(lua_State* L) {
    l_quat_t *q = luaX_checkquat(L, 1);

    lua_pushnumber(L, (lua_Number)quat_lensq(qr(q)));

    return 1;
}

static int l_quat_conjugate(lua_State* L) {
    l_quat_t *q = luaX_checkquat(L, 1);
    l_quat_t *r = new_quat(L);

    quat_conjugate(qr(r), qr(q));

    return 1;
}

static int l_quat_dot(lua_State* L) {
    l_quat_t *a = luaX_checkquat(L, 1);
    l_quat_t *b = luaX_checkquat(L, 2);

    lua_pushnumber(L, (lua_Number)quat_dot(qr(a), qr(b)));

    return 1;
}

static int l_quat_normalize(lua_State* L) {
    l_quat_t *q = luaX_checkquat(L, 1);
    real_t len = (real_t)luaL_optnumber(L, 2, r_one);

    lua_pushnumber(L, quat_normalize(qr(q), len));

    return 1;
}

static int l_quat_slerp(lua_State* L) {
    l_quat_t *a = luaX_checkquat(L, 1);
    l_quat_t *b = luaX_checkquat(L, 2);
    real_t t = (real_t)luaL_checknumber(L, 3);
    l_quat_t *r = new_quat(L);

    quat_slerp(qr(r), qr(a), qr(b), t);

    return 1;
}

static int l_quat_rotate(lua_State* L) {
    int top = lua_gettop(L);
    l_quat_t *q = luaX_checkquat(L, 1);
    l_vec3_t *r = new_vec3(L);

    if (4 == top) {
        vec3_t v;
        vx(v) = (real_t)luaL_checknumber(L, 2);
        vy(v) = (real_t)luaL_checknumber(L, 3);
        vz(v) = (real_t)luaL_checknumber(L, 4);

        quat_rotate(v3(r), qr(q), v);
    } else {
        l_vec3_t *v = luaX_checkvec3(L, 2);

        quat_rotate(v3(r), qr(q), v3(v));
    }

    return 1;
}

static int l_quat_matrix(lua_State* L) {
    l_quat_t *q = luaX_checkquat(L, 1);
    l_mat33_t *m = new_mat33(L);

    quat_tomatrix(m3(m), qr(q));

    return 1;
}

static int l_quat_euler(lua_State* L) {
    int top = lua_gettop(L);
    l_quat_t *q = luaX_checkquat(L, 1);

    if (1 == top) {
        l_vec3_t *r = new_vec3(L);

        quat_toeuler(v3(r), qr(q));

        return 1;
    }

    if (4 == top) {
        vec3_t v;
        vx(v) = (real_t)luaL_checknumber(L, 2);
        vy(v) = (real_t)luaL_checknumber(L, 3);
        vz(v) = (real_t)luaL_checknumber(L, 4);

        quat_fromeuler(qr(q), v);
    } else {
        l_vec3_t *v = luaX_checkvec3(L, 2);

        quat_fromeuler(qr(q), v3(v));
    }

    return 0;
}

static int l_quat_angleaxis(lua_State* L) {
    int top = lua_gettop(L);
    l_quat_t *q = luaX_checkquat(L, 1);
    real_t theta = (real_t)luaL_checknumber(L, 2);

    if (5 == top) {
        vec3_t v;
        vx(v) = (real_t)luaL_checknumber(L, 3);
        vy(v) = (real_t)luaL_checknumber(L, 4);
        vz(v) = (real_t)luaL_checknumber(L, 5);

        quat_fromangleaxis(qr(q), v, theta);
    } else {
        l_vec3_t *v = luaX_checkvec3(L, 3);

        quat_fromangleaxis(qr(q), v3(v), theta);
    }

    return 0;
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
        e11(m3(m)) = (real_t)luaL_checknumber(L, 1);
        e12(m3(m)) = (real_t)luaL_checknumber(L, 2);
        e13(m3(m)) = (real_t)luaL_checknumber(L, 3);

        e21(m3(m)) = (real_t)luaL_checknumber(L, 4);
        e22(m3(m)) = (real_t)luaL_checknumber(L, 5);
        e23(m3(m)) = (real_t)luaL_checknumber(L, 6);

        e31(m3(m)) = (real_t)luaL_checknumber(L, 7);
        e32(m3(m)) = (real_t)luaL_checknumber(L, 8);
        e33(m3(m)) = (real_t)luaL_checknumber(L, 9);
    } else if (3 == top) {
        mat33_identity(m3(m));

        e11(m3(m)) = (real_t)luaL_checknumber(L, 1);
        e22(m3(m)) = (real_t)luaL_checknumber(L, 2);
        e33(m3(m)) = (real_t)luaL_checknumber(L, 3);
    } else {
        mat33_identity(m3(m));
    }

    return 1;
}

static int l_quat_new(lua_State* L) {
    int top = lua_gettop(L);
    l_quat_t *q = new_quat(L);

    if (4 == top) {
        qw(qr(q)) = (real_t)luaL_checknumber(L, 1);
        qx(qr(q)) = (real_t)luaL_checknumber(L, 2);
        qy(qr(q)) = (real_t)luaL_checknumber(L, 3);
        qz(qr(q)) = (real_t)luaL_checknumber(L, 4);
    } else {
        qw(qr(q)) = r_one;
        qx(qr(q)) = qy(qr(q)) = qz(qr(q)) = r_zero;
    }

    return 1;
}

int luaopen_sevo_math(lua_State* L) {
    luaL_Reg mod_math[] = {
        { "vec2", l_vec2_new },
        { "vec3", l_vec3_new },
        { "mat33", l_mat33_new },
        { "quat", l_quat_new },
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
        { "transform2", l_mat33_transform2 },
        { "transform3", l_mat33_transform3 },
        { "row", l_mat33_row },
        { "column", l_mat33_column },
        { NULL, NULL }
    };
    luaL_Reg meta_quat[] = {
        { "__add", l_quat_add },
        { "__sub", l_quat_sub },
        { "__mul", l_quat_mul },
        { "__unm", l_quat_unm },
        { "__len", l_quat_len },
        { "__eq",  l_quat_eq },
        { "__tostring", l_quat_tostring },
        { "__concat", l_math_concat },
        { "lensq", l_quat_lensq },
        { "conjugate", l_quat_conjugate },
        { "dot", l_quat_dot },
        { "norm", l_quat_normalize },
        { "slerp", l_quat_slerp },
        { "rotate", l_quat_rotate },
        { "matrix", l_quat_matrix },
        { "euler", l_quat_euler },
        { "angleaxis", l_quat_angleaxis },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_vec2, meta_vec2);
    luaX_register_type(L, g_meta_vec3, meta_vec3);
    luaX_register_type(L, g_meta_mat33, meta_mat33);
    luaX_register_type(L, g_meta_quat, meta_quat);
    luaX_register_module(L, "math", mod_math);

    return 0;
}
