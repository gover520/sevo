/*
 *  wrap_net.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_net.h"
#include "common/handle.h"
#include "modules/gmp/wrap_gmp.h"
#include <string.h>

#ifdef _MSC_VER
# define strcasecmp _stricmp
#endif

typedef struct mcl_net_t {
    mc_net_t    *net;
} mcl_net_t;

typedef struct n_session_t {
    mcl_net_t   *holder;
    handle_t    handle;
} n_session_t;

static const char g_meta_net[] = { CODE_NAME "meta.net" };

#define luaX_checknet(L, idx)   (mcl_net_t *)luaL_checkudata(L, idx, g_meta_net)

static int mcl_net__gc(lua_State *L) {
    mcl_net_t *n = luaX_checknet(L, -1);
    if (n && n->net) {
        mc_net_destroy(n->net);
        n->net = NULL;
    }
    return 0;
}

static void on_peer_delete(mc_peer_t *peer) {
    n_session_t *sess = (n_session_t *)mc_net_get(peer);
    if (sess) {
        handle_unref(sess->handle);
        mc_free(sess);
    }
}

static int net_mode(lua_State *L, const char *mode) {
    if (0 == strcasecmp(mode, "tcp")) {
        return MC_NET_TCP;
    }
    if (0 == strcasecmp(mode, "udp")) {
        return MC_NET_UDP;
    }
    return luaL_error(L, "Invalid operand. Expected 'tcp' or 'udp'");
}

static n_session_t *session_new(mcl_net_t *holder, mc_peer_t *peer) {
    n_session_t *sess = (n_session_t *)mc_malloc(sizeof(n_session_t));

    sess->handle = handle_new(peer);
    sess->holder = holder;

    mc_net_set(peer, sess);
    mc_net_on_delete(peer, on_peer_delete);

    return sess;
}

static int mcl_net_server(lua_State *L) {
    mcl_net_t *n = luaX_checknet(L, 1);
    const char *mode = luaL_checkstring(L, 2);
    const char *host = luaL_checkstring(L, 3);
    unsigned short port = (unsigned short)luaL_checkinteger(L, 4);
    int backlog = (int)luaL_optinteger(L, 5, -1);
    n_session_t *sess;
    mc_peer_t *s;

    s = mc_net_server(n->net, net_mode(L, mode), host, port, backlog);

    if (s) {
        sess = session_new(n, s);
        lua_pushinteger(L, (lua_Integer)sess->handle);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mcl_net_connect(lua_State *L) {
    mcl_net_t *n = luaX_checknet(L, 1);
    const char *mode = luaL_checkstring(L, 2);
    const char *host = luaL_checkstring(L, 3);
    unsigned short port = (unsigned short)luaL_checkinteger(L, 4);
    n_session_t *sess;
    mc_peer_t *c;

    c = mc_net_connect(n->net, net_mode(L, mode), host, port);

    if (c) {
        sess = session_new(n, c);
        lua_pushinteger(L, (lua_Integer)sess->handle);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mcl_net_timeout(lua_State *L) {
    mcl_net_t *n = luaX_checknet(L, 1);
    const char *ty = luaL_checkstring(L, 2);
    int timeout = (int)luaL_checkinteger(L, 3);

    if ((0 == strcasecmp(ty, "conn")) || (0 == strcasecmp(ty, "connect"))) {
        lua_pushinteger(L, mc_net_conn_timeout(n->net, timeout));
        return 1;
    }

    if ((0 == strcasecmp(ty, "auth")) || (0 == strcasecmp(ty, "authenticate"))) {
        lua_pushinteger(L, mc_net_auth_timeout(n->net, timeout));
        return 1;
    }

    return luaL_error(L, "Invalid operand. Expected 'conn' or 'connect', 'auth' or 'authenticate'");
}

static int mcl_net_recv(lua_State *L) {
    mcl_net_t *n = luaX_checknet(L, 1);
    mc_net_event_t *evt;
    mc_peer_t *parent;
    n_session_t *sess;
    bigint_t *bi;

    while (!!(evt = mc_net_recv(n->net))) {
        sess = (n_session_t *)mc_net_get(evt->peer);

        if (sess) {
            break;
        }

        if (mc_net_is_closing(evt->peer) || mc_net_is_closed(evt->peer)) {
            mc_free(evt);
            continue;
        }

        parent = mc_net_parent(evt->peer);

        if (!parent) {
            mc_net_close(evt->peer);
            mc_free(evt);
            continue;
        }

        if (!mc_net_get(parent)) {
            mc_net_close(evt->peer);
            mc_free(evt);
            continue;
        }

        sess = session_new(n, evt->peer);
        break;
    }

    if (!evt) {
        lua_pushnil(L);
        return 1;
    }

    lua_createtable(L, 0, 3);

    switch (evt->command) {
    case MC_NET_INCOMING:
        lua_pushstring(L, "incoming");
        lua_setfield(L, -2, "cmd");
        break;
    case MC_NET_CONNECT_TIMEOUT:
        lua_pushstring(L, "conn-timeout");
        lua_setfield(L, -2, "cmd");
        break;
    case MC_NET_HALO:
        lua_pushstring(L, "halo");
        lua_setfield(L, -2, "cmd");

        lua_pushinteger(L, evt->halo.proto_version);
        lua_setfield(L, -2, "protover");
        break;
    case MC_NET_AUTH:
        lua_pushstring(L, "auth");
        lua_setfield(L, -2, "cmd");

        lua_pushstring(L, evt->auth.passwd);
        lua_setfield(L, -2, "passwd");
        break;
    case MC_NET_ACCEPTED:
        lua_pushstring(L, "accepted");
        lua_setfield(L, -2, "cmd");

        lua_pushstring(L, evt->accepted.welcome);
        lua_setfield(L, -2, "welcome");
        break;
    case MC_NET_REJECTED:
        lua_pushstring(L, "rejected");
        lua_setfield(L, -2, "cmd");

        lua_pushstring(L, evt->rejected.reason);
        lua_setfield(L, -2, "reason");
        break;
    case MC_NET_PING:
        lua_pushstring(L, "ping");
        lua_setfield(L, -2, "cmd");

        bi = new_bigint(L);
        mpz_set_ull(bi->x, evt->ping.timestamp);
        lua_setfield(L, -2, "time");
        break;
    case MC_NET_PONG:
        lua_pushstring(L, "pong");
        lua_setfield(L, -2, "cmd");

        bi = new_bigint(L);
        mpz_set_ull(bi->x, evt->pong.timestamp);
        lua_setfield(L, -2, "time");
        break;
    case MC_NET_OUTGOING:
        lua_pushstring(L, "outgoing");
        lua_setfield(L, -2, "cmd");
        break;
    case MC_NET_DATA:
        lua_pushstring(L, "data");
        lua_setfield(L, -2, "cmd");

        lua_pushlstring(L, (const char *)evt->data.ptr, evt->data.size);
        lua_setfield(L, -2, "data");
        break;
    }

    lua_pushinteger(L, (lua_Integer)sess->handle);
    lua_setfield(L, -2, "who");

    mc_free(evt);
    return 1;
}

static int mcl_net_update(lua_State *L) {
    mcl_net_t *n = luaX_checknet(L, 1);
    lua_pushboolean(L, 0 == mc_net_update(n->net));
    return 1;
}

static mc_peer_t *luaX_checkpeer(lua_State *L, int index) {
    handle_t h = (handle_t)luaL_checkinteger(L, index);
    mc_peer_t *peer;

    if (0 == h) {
        luaL_error(L, "Invalid session handle.");
        return NULL;
    }

    peer = handle_ref(h);

    if (!peer) {
        luaL_error(L, "Peer is already closed.");
        return NULL;
    }

    handle_unref(h);

    return peer;
}

static int mcl_peer_addr(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    const char *ty = luaL_optstring(L, 2, NULL);
    struct sockaddr_storage ss;
    mc_addr_t addr = { { 0 }, 0 };
    unsigned char hw[MC_HWADDR_LEN];
    char hwaddr[MC_HWADDR_LEN * 2 + 6] = { 0 };

    if (0 == strcasecmp(ty, "hwaddr") ||
        0 == strcasecmp(ty, "macaddr")) {
        mc_net_hwaddr(peer, hw);

        mc_byte2hex(hw[0], hwaddr + 0); hwaddr[2] = ':';
        mc_byte2hex(hw[1], hwaddr + 3); hwaddr[5] = ':';
        mc_byte2hex(hw[2], hwaddr + 6); hwaddr[8] = ':';
        mc_byte2hex(hw[3], hwaddr + 9); hwaddr[11] = ':';
        mc_byte2hex(hw[4], hwaddr + 12);hwaddr[14] = ':';
        mc_byte2hex(hw[5], hwaddr + 15);

        lua_pushstring(L, hwaddr);
        return 1;
    }

    if (0 == strcasecmp(ty, "local")) {
        mc_socket_sockname(mc_net_socket(peer), &ss);
        mc_addr_info(&addr, &ss);

        lua_pushstring(L, addr.addr);
        lua_pushinteger(L, addr.port);
        return 2;
    }

    if (0 == strcasecmp(ty, "remote")) {
        mc_socket_peername(mc_net_socket(peer), &ss);
        mc_addr_info(&addr, &ss);

        lua_pushstring(L, addr.addr);
        lua_pushinteger(L, addr.port);
        return 2;
    }

    return luaL_error(L, "Invalid operand. Expected 'hwaddr' or 'macaddr', 'local' or 'remote'");
}

static int mcl_peer_mode(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    int type = mc_net_type(peer);

    if (MC_NET_TCP == type) {
        lua_pushstring(L, "tcp");
    }

    if (MC_NET_UDP == type) {
        lua_pushstring(L, "udp");
    }

    return 1;
}

static int mcl_peer_close(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    lua_pushboolean(L, 0 == mc_net_close(peer));
    return 1;
}

static int mcl_peer_auth(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    const char *passwd = luaL_checkstring(L, 2);
    lua_pushboolean(L, 0 == mc_net_auth(peer, passwd));
    return 1;
}

static int mcl_peer_accept(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    const char *welcome = luaL_checkstring(L, 2);
    lua_pushboolean(L, 0 == mc_net_accept(peer, welcome));
    return 1;
}

static int mcl_peer_reject(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    const char *reson = luaL_checkstring(L, 2);
    lua_pushboolean(L, 0 == mc_net_reject(peer, reson));
    return 1;
}

static int mcl_peer_ping(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    mpz_t ts;

    mpz_init(ts);

    luaX_checkmpz(L, 2, ts);
    lua_pushboolean(L, 0 == mc_net_ping(peer, mpz_get_ull(ts)));

    mpz_clear(ts);

    return 1;
}

static int mcl_peer_pong(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    mpz_t ts;

    mpz_init(ts);

    luaX_checkmpz(L, 2, ts);
    lua_pushboolean(L, 0 == mc_net_pong(peer, mpz_get_ull(ts)));

    mpz_clear(ts);

    return 1;
}

static int mcl_peer_send(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);
    size_t l = 0;
    const char *data = luaL_checklstring(L, 2, &l);
    int len = (int)luaL_optinteger(L, 3, (lua_Integer)l);

    lua_pushboolean(L, 0 == mc_net_send(peer, data, len));
    return 1;
}

static int mcl_peer_status(lua_State *L) {
    mc_peer_t *peer = luaX_checkpeer(L, 1);

    if (!peer) {
        lua_pushstring(L, "closed");
    } else if (mc_net_is_connecting(peer)) {
        lua_pushstring(L, "connecting");
    } else if (mc_net_is_connected(peer)) {
        lua_pushstring(L, "connected");
    } else if (mc_net_is_closing(peer)) {
        lua_pushstring(L, "closing");
    } else if (mc_net_is_closed(peer)) {
        lua_pushstring(L, "closed");
    } else {
        return luaL_error(L, "Peer unknown status.");
    }
    return 1;
}

static int mcl_net_new(lua_State *L) {
    mcl_net_t *n = (mcl_net_t *)luaX_newuserdata(L, g_meta_net, sizeof(mcl_net_t));
    n->net = mc_net_create();
    return 1;
}

int luaopen_sevo_net(lua_State* L) {
    luaL_Reg meta_net[] = {
        { "__gc", mcl_net__gc },
        { "server", mcl_net_server },
        { "connect", mcl_net_connect },
        { "timeout", mcl_net_timeout },
        { "recv", mcl_net_recv },
        { "update", mcl_net_update },
        { NULL, NULL }
    };
    luaL_Reg mod_net[] = {
        { "new", mcl_net_new },
        { "addr", mcl_peer_addr },
        { "mode", mcl_peer_mode },
        { "close", mcl_peer_close },
        { "auth", mcl_peer_auth },
        { "accept", mcl_peer_accept },
        { "reject", mcl_peer_reject },
        { "ping", mcl_peer_ping },
        { "pong", mcl_peer_pong },
        { "send", mcl_peer_send },
        { "status", mcl_peer_status },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_net, meta_net);
    luaX_register_module(L, "net", mod_net);
    return 0;
}
