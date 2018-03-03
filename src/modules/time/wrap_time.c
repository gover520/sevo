/*
 *  wrap_time.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_time.h"

static const char g_meta_timer[] = { CODE_NAME "meta.timer" };
static const char g_meta_fps[] = { CODE_NAME "meta.fps" };

#define luaX_checktimer(L, idx) (mc_timer_t *)luaL_checkudata(L, idx, g_meta_timer)
#define luaX_checkfps(L, idx)   (mc_fps_t *)luaL_checkudata(L, idx, g_meta_fps)

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

static int mcl_fps_update(lua_State * L) {
    mc_fps_t *fps = luaX_checkfps(L, 1);
    lua_pushboolean(L, 0 == mc_fps_update(fps, NULL));
    return 1;
}

static int mcl_fps_wait(lua_State * L) {
    mc_fps_t *fps = luaX_checkfps(L, 1);
    lua_pushinteger(L, mc_fps_wait(fps, NULL));
    return 1;
}

static int mcl_fps_count(lua_State * L) {
    mc_fps_t *fps = luaX_checkfps(L, 1);
    lua_pushinteger(L, mc_fps_count(fps));
    return 1;
}

static int mcl_fps_delta(lua_State * L) {
    mc_fps_t *fps = luaX_checkfps(L, 1);
    lua_pushinteger(L, mc_fps_delta(fps));
    return 1;
}

static int mcl_fps_delta_avg(lua_State * L) {
    mc_fps_t *fps = luaX_checkfps(L, 1);
    lua_pushinteger(L, mc_fps_delta_avg(fps));
    return 1;
}

static int mcl_fps_avg(lua_State * L) {
    mc_fps_t *fps = luaX_checkfps(L, 1);
    lua_pushinteger(L, mc_fps_avg(fps));
    return 1;
}

static int mcl_fps_new(lua_State * L) {
    mc_fps_t *fps = (mc_fps_t *)luaX_newuserdata(L, g_meta_fps, sizeof(mc_fps_t));
    float rate = (float)luaL_checknumber(L, 1);
    mc_fps_init(fps, NULL, rate);
    return 1;
}

static int mcl_sleep(lua_State * L) {
    unsigned int ms = (unsigned int)luaL_checkinteger(L, 1);
    mc_sleep(ms);
    return 0;
}

static int mcl_now(lua_State * L) {
    lua_pushinteger(L, (lua_Integer)mc_timestamp(NULL));
    return 1;
}

static int mcl_millisec(lua_State * L) {
    lua_pushinteger(L, (lua_Integer)mc_millisec(NULL));
    return 1;
}

static int mcl_microsec(lua_State * L) {
    lua_pushinteger(L, (lua_Integer)mc_microsec(NULL));
    return 1;
}

int luaopen_sevo_time(lua_State* L) {
    luaL_Reg meta_timer[] = {
        { "set", mcl_timer_set },
        { "expired", mcl_timer_expired },
        { NULL, NULL }
    };
    luaL_Reg meta_fps[] = {
        { "update", mcl_fps_update },
        { "wait", mcl_fps_wait },
        { "count", mcl_fps_count },
        { "delta", mcl_fps_delta },
        { "delta_avg", mcl_fps_delta_avg },
        { "avg", mcl_fps_avg },
        { NULL, NULL }
    };
    luaL_Reg mod_time[] = {
        { "timer", mcl_timer_new },
        { "fps", mcl_fps_new },
        { "sleep", mcl_sleep },
        { "now", mcl_now },
        { "millisec", mcl_millisec },
        { "microsec", mcl_microsec },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_timer, meta_timer);
    luaX_register_type(L, g_meta_fps, meta_fps);
    luaX_register_module(L, "time", mod_time);

    return 0;
}
