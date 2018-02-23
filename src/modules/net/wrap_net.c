/*
 *  wrap_net.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_net.h"
#include "common/version.h"
#include "modules/gmp/wrap_gmp.h"

#ifdef _MSC_VER
# define strcasecmp _stricmp
#endif

typedef struct mcl_net_t {
    mc_net_t    *net;
} mcl_net_t;

typedef struct mcl_peer_t {
    int         cb_ref;
    mc_peer_t   *peer;
} mcl_peer_t;

static const char g_meta_net[] = { CODE_NAME "meta.net" };
static const char g_meta_peer[] = { CODE_NAME "meta.peer" };

#define luaX_checknet(L, idx)   (mcl_net_t *)luaL_checkudata(L, idx, g_meta_net)
#define luaX_checkpeer(L, idx)  (mcl_peer_t *)luaL_checkudata(L, idx, g_meta_peer)

static void on_peer_delete(mc_peer_t *peer) {
    mcl_peer_t *hnd = (mcl_peer_t *)mc_net_get(peer);
    if (hnd) {
        /* peer will be freed */
        hnd->peer = NULL;
    }
}

static int mcl_peer_id(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);
    bigint_t *bi;
    
    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }

    bi = new_bigint(L);
    mpz_set_ull(bi->x, mc_net_id(hnd->peer));
    return 1;
}

static int mcl_peer_addr(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);
    const char *ty = luaL_optstring(L, 2, NULL);
    struct sockaddr_storage ss;
    mc_addr_t addr = { { 0 }, 0 };
    unsigned char hw[MC_HWADDR_LEN];
    char hwaddr[MC_HWADDR_LEN * 2 + 1] = { 0 };

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }

    if (0 == strcasecmp(ty, "hwaddr") ||
        0 == strcasecmp(ty, "macaddr")) {
        mc_net_hwaddr(hnd->peer, hw);

        mc_byte2hex(hw[0], hwaddr + 0);
        mc_byte2hex(hw[1], hwaddr + 2);
        mc_byte2hex(hw[2], hwaddr + 4);
        mc_byte2hex(hw[3], hwaddr + 6);
        mc_byte2hex(hw[4], hwaddr + 8);
        mc_byte2hex(hw[5], hwaddr + 10);

        lua_pushstring(L, hwaddr);
        return 1;
    } else if (0 == strcasecmp(ty, "local")) {
        mc_socket_sockname(mc_net_socket(hnd->peer), &ss);
        mc_addr_info(&addr, &ss);

        lua_pushstring(L, addr.addr);
        lua_pushinteger(L, addr.port);
        return 2;
    } else if (0 == strcasecmp(ty, "remote")) {
        mc_socket_peername(mc_net_socket(hnd->peer), &ss);
        mc_addr_info(&addr, &ss);

        lua_pushstring(L, addr.addr);
        lua_pushinteger(L, addr.port);
        return 2;
    }

    return luaL_error(L, "Invalid operand. Expected 'hwaddr' or 'macaddr', 'local' or 'remote'");
}

static int mcl_peer_close(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }

    lua_pushboolean(L, 0 == mc_net_close(hnd->peer));
    return 1;
}

static int mcl_peer_auth(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_peer_accept(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_peer_reject(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_peer_ping(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_peer_pong(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_peer_send(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_peer_status(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_peer__gc(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, -1);
    if (hnd) {
        if (hnd->peer) {
            /* The peer will be freed later */
            mc_net_set(hnd->peer, NULL);
            mc_net_close(hnd->peer);
            hnd->peer = NULL;
        }

        luaL_unref(L, LUA_REGISTRYINDEX, hnd->cb_ref);
        hnd->cb_ref = LUA_NOREF;
    }
}

static int mcl_peer_tostring(lua_State *L) {
    mcl_peer_t *hnd = luaX_checkpeer(L, 1);

    if (!hnd->peer) {
        luaL_error(L, "Peer is already closed.");
    }
}

static int mcl_net_server(lua_State *L) {
    mcl_net_t *hnd = luaX_checknet(L, -1);
}

static int mcl_net_connect(lua_State *L) {
    mcl_net_t *hnd = luaX_checknet(L, -1);
}

static int mcl_net_timeout(lua_State *L) {
    mcl_net_t *hnd = luaX_checknet(L, -1);
}

static int mcl_net_update(lua_State *L) {
    mcl_net_t *hnd = luaX_checknet(L, -1);
}

static int mcl_net__gc(lua_State *L) {
    mcl_net_t *hnd = luaX_checknet(L, -1);
    if (hnd && hnd->net) {
        mc_net_destroy(hnd->net);
        hnd->net = NULL;
    }
    return 0;
}

static int mcl_net_tostring(lua_State *L) {
}

static int mcl_net(lua_State *L) {
    mcl_net_t *hnd = (mcl_net_t *)luaX_newuserdata(L, g_meta_net, sizeof(mcl_net_t));
    hnd->net = mc_net_create();
    return 1;
}

int luaopen_sevo_net(lua_State* L) {
    luaL_Reg meta_peer[] = {
        { "id", mcl_peer_id },
        { "addr", mcl_peer_addr },
        { "close", mcl_peer_close },
        { "auth", mcl_peer_auth },
        { "accept", mcl_peer_accept },
        { "reject", mcl_peer_reject },
        { "ping", mcl_peer_ping },
        { "pong", mcl_peer_pong },
        { "send", mcl_peer_send },
        { "status", mcl_peer_status },
        { "__gc", mcl_peer__gc },
        { "__tostring", mcl_peer_tostring },
        { NULL, NULL }
    };
    luaL_Reg meta_net[] = {
        { "server", mcl_net_server },
        { "connect", mcl_net_connect },
        { "timeout", mcl_net_timeout },
        { "update", mcl_net_update },
        { "__gc", mcl_net__gc },
        { "__tostring", mcl_net_tostring },
        { NULL, NULL }
    };
    luaL_Reg mod_net[] = {
        { "net", mcl_net },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_peer, meta_peer);
    luaX_register_type(L, g_meta_net, meta_net);
    luaX_register_funcs(L, mod_net);
    return 0;
}
