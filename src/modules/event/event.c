/*
 *  event.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "event.h"

static mc_chan_t    *g_chan = NULL;

int event_init(int capacity) {
    if (g_chan) {
        return 0;
    }

    g_chan = mc_chan_create(capacity);

    return 0;
}

void event_deinit(void) {
    if (g_chan) {
        mc_chan_destroy(g_chan);
        g_chan = NULL;
    }
}

static int event_pump(void) {
    return 0;
}

static int event_poll(void) {
    void *p = NULL;

    if (g_chan && mc_chan_can_read(g_chan)) {
        if (0 == mc_chan_read(g_chan, &p)) {
            return (int)(intptr_t)p;
        }
    }
    return -1;
}

static int event_push(int ref) {
    if (g_chan && mc_chan_can_write(g_chan)) {
        if (0 == mc_chan_write(g_chan, (void *)(intptr_t)ref)) {
            return 0;
        }
    }
    return -1;
}

static int w_evt_init(lua_State * L) {
    int qmax = (int)luaL_checkinteger(L, 1);
    lua_pushboolean(L, 0 == event_init(qmax));
    return 1;
}

static int w_evt_pump(lua_State * L) {
    lua_pushboolean(L, 0 == event_pump());
    return 1;
}

static int w_evt_poll(lua_State * L) {
    int ref = event_poll();
    if (ref > 0) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int w_evt_push(lua_State * L) {
    int ref;
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_pushvalue(L, 1);
    ref = luaL_ref(L, LUA_REGISTRYINDEX);

    if (0 == event_push(ref)) {
        lua_pushboolean(L, 1);
    } else {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        lua_pushboolean(L, 0);
    }

    return 1;
}

int luaopen_sevo_event(lua_State* L) {
    luaL_Reg mod_event[] = {
        { "init", w_evt_init },
        { "pump", w_evt_pump },
        { "poll_t", w_evt_poll },
        { "push_t", w_evt_push },
        { NULL, NULL }
    };

    luaX_register_module(L, "event", mod_event);
    return 0;
}
