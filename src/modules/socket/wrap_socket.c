/*
 *  wrap_socket.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_socket.h"
#include <string.h>

#ifdef _WIN32
#undef FD_SETSIZE
#define FD_SETSIZE  1024
#endif

#ifdef _MSC_VER
# define strcasecmp _stricmp
#endif

static const char g_meta_socket[] = { CODE_NAME "meta.socket" };

typedef struct mcl_socket_t {
    SOCKET      sock;
    int         family;
} mcl_socket_t;

#define luaX_checksocket(L, idx)    (mcl_socket_t *)luaL_checkudata(L, idx, g_meta_socket)

static int luaX_checkaddr(lua_State *L, int index, mc_addr_t *addr) {
    if (lua_isnil(L, index)) {
        return luaL_error(L, "Invalid operand. Expected { addr, port }");
    }
    luaL_checktype(L, index, LUA_TTABLE);

    lua_pushnumber(L, 1);
    lua_gettable(L, index);
    strcpy(addr->addr, luaL_optstring(L, -1, "0.0.0.0"));
    lua_pop(L, 1);

    lua_pushnumber(L, 2);
    lua_gettable(L, index);
    addr->port = (unsigned short)luaL_optinteger(L, -1, 0);
    lua_pop(L, 1);

    return 0;
}

static int luaX_checksockaddr(lua_State *L, int index, mcl_socket_t *s, struct sockaddr_storage *ss, int len) {
    mc_addr_t addr;
    luaX_checkaddr(L, index, &addr);
    return (mc_resolve_host(ss, len, s->family, addr.addr, addr.port) > 0) ? 0 : -1;
}

static int luaX_returnaddr(lua_State *L, mc_addr_t *addr) {
    lua_createtable(L, 2, 0);

    lua_pushstring(L, addr->addr);
    lua_rawseti(L, -2, 1);

    lua_pushinteger(L, addr->port);
    lua_rawseti(L, -2, 2);

    return 1;
}

static int mcl_socket__gc(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, -1);
    if (s && (INVALID_SOCKET != s->sock)) {
        mc_socket_close(s->sock);
        s->sock = INVALID_SOCKET;
    }
    return 0;
}

static int mcl_socket_getfd(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, 1);
    lua_pushinteger(L, s->sock);
    return 1;
}

static int mcl_socket_getfamily(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, 1);
    if (AF_INET6 == s->family) {
        lua_pushliteral(L, "inet6");
    } else {
        lua_pushliteral(L, "inet4");
    }
    return 1;
}

static int mcl_socket_close(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, 1);
    if (INVALID_SOCKET != s->sock) {
        mc_socket_close(s->sock);
        s->sock = INVALID_SOCKET;
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

static int mcl_socket_shutdown(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, 1);
    const char *mode = luaL_optstring(L, 2, NULL);
    int how = SHUT_RDWR;

    if (!mode || (0 == strcasecmp(mode, "both"))) {
        how = SHUT_RDWR;
    }
    if (mode && (0 == strcasecmp(mode, "recv"))) {
        how = SHUT_RD;
    }
    if (mode && (0 == strcasecmp(mode, "send"))) {
        how = SHUT_WR;
    }
    lua_pushboolean(L, 0 == mc_socket_shutdown(s->sock, how));
    return 1;
}

static int mcl_socket_bind(lua_State *L) {
    struct sockaddr_storage ss;
    mcl_socket_t *s = luaX_checksocket(L, 1);

    luaX_checksockaddr(L, 2, s, &ss, 1);
    lua_pushboolean(L, 0 == mc_socket_bind(s->sock, &ss));
    
    return 1;
}

static int mcl_socket_listen(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, 1);
    int backlog = (int)luaL_optinteger(L, 2, MC_SOMAXCONN);

    lua_pushboolean(L, 0 == mc_socket_listen(s->sock, backlog));
    
    return 1;
}

static int mcl_socket_connect(lua_State *L) {
    struct sockaddr_storage ss;
    mcl_socket_t *s = luaX_checksocket(L, 1);

    luaX_checksockaddr(L, 2, s, &ss, 1);
    lua_pushboolean(L, 0 == mc_socket_connect(s->sock, &ss));

    return 1;
}

static int mcl_socket_accept(lua_State *L) {
    struct sockaddr_storage ss;
    mcl_socket_t *s = luaX_checksocket(L, 1);
    SOCKET c = mc_socket_accept(s->sock, &ss);
    mcl_socket_t *n;
    mc_addr_t addr;

    if (INVALID_SOCKET == c) {
        lua_pushnil(L);
        return 1;
    }

    mc_addr_info(&addr, &ss);

    n = (mcl_socket_t *)luaX_newuserdata(L, g_meta_socket, sizeof(mcl_socket_t));

    n->sock = c;
    n->family = s->family;

    luaX_returnaddr(L, &addr);

    return 2;
}

static int mcl_socket_getpeername(lua_State *L) {
    struct sockaddr_storage ss;
    mcl_socket_t *s = luaX_checksocket(L, 1);
    mc_addr_t addr;

    mc_socket_peername(s->sock, &ss);
    mc_addr_info(&addr, &ss);

    return luaX_returnaddr(L, &addr);
}

static int mcl_socket_getsockname(lua_State *L) {
    struct sockaddr_storage ss;
    mcl_socket_t *s = luaX_checksocket(L, 1);
    mc_addr_t addr;

    mc_socket_sockname(s->sock, &ss);
    mc_addr_info(&addr, &ss);

    return luaX_returnaddr(L, &addr);
}

static int mcl_socket_send(lua_State *L) {
    size_t l = 0;
    mcl_socket_t *s = luaX_checksocket(L, 1);
    const char *data = luaL_checklstring(L, 2, &l);
    int len = (int)luaL_optinteger(L, 3, (lua_Integer)l);

    lua_pushinteger(L, mc_socket_send(s->sock, data, len, 0));
    
    return 1;
}

static int mcl_socket_sendto(lua_State *L) {
    size_t l = 0;
    struct sockaddr_storage ss;
    mcl_socket_t *s = luaX_checksocket(L, 1);
    const char *data = luaL_checklstring(L, 3, &l);
    int len = (int)luaL_optinteger(L, 4, (lua_Integer)l);

    luaX_checksockaddr(L, 2, s, &ss, 1);
    lua_pushinteger(L, mc_socket_sendto(s->sock, &ss, data, len, 0));

    return 1;
}

static int mcl_socket_recv(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, 1);

    return 1;
}

static int mcl_socket_recvfrom(lua_State *L) {
}

static int mcl_socket_sendall(lua_State *L) {
}

static int mcl_socket_recvall(lua_State *L) {
}

static int mcl_socket_reuseaddr(lua_State *L) {
}

static int mcl_socket_nodelay(lua_State *L) {
}

static int mcl_socket_nonblock(lua_State *L) {
}

static int mcl_socket_broadcast(lua_State *L) {
}

static int mcl_socket_add_membership(lua_State *L) {
}

static int mcl_socket_drop_membership(lua_State *L) {
}

static int mcl_socket_multicast_loop(lua_State *L) {
}

static int mcl_socket_select(lua_State *L) {
    unsigned int timeout = (unsigned int)luaL_optinteger(L, 3, 0);
    int rlen, wlen;

    if (lua_isnil(L, 1)) {
        rlen = 0;
    }
    if (lua_isnil(L, 2)) {
        wlen = 0;
    }
}

static int mcl_socket_tcp(lua_State *L) {
    int type = (int)luaL_optinteger(L, 1, 4);
    SOCKET sock = INVALID_SOCKET;
    int family = -1;
    mcl_socket_t *s;

    if (4 == type) {
        sock = mc_socket_tcp4();
        family = AF_INET;
    }
    if (6 == type) {
        sock = mc_socket_tcp6();
        family = AF_INET6;
    }

    if (INVALID_SOCKET == sock) {
        return luaL_error(L, "Invalid operand. Expected 4 or 6");
    }

    s = (mcl_socket_t *)luaX_newuserdata(L, g_meta_socket, sizeof(mcl_socket_t));

    s->sock = sock;
    s->family = family;

    return 1;
}

static int mcl_socket_udp(lua_State *L) {
    int type = (int)luaL_optinteger(L, 1, 4);
    SOCKET sock = INVALID_SOCKET;
    int family = -1;
    mcl_socket_t *s;

    if (4 == type) {
        sock = mc_socket_udp4();
        family = AF_INET;
    }
    if (6 == type) {
        sock = mc_socket_udp6();
        family = AF_INET6;
    }

    if (INVALID_SOCKET == sock) {
        return luaL_error(L, "Invalid operand. Expected 4 or 6");
    }

    s = (mcl_socket_t *)luaX_newuserdata(L, g_meta_socket, sizeof(mcl_socket_t));

    s->sock = sock;
    s->family = family;
    
    return 1;
}

static int mcl_socket_error(lua_State *L) {
    mcl_socket_t *s = luaX_checksocket(L, 1);
    int err = 0;
    socklen_t len = sizeof(err);

    if (getsockopt(s->sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len) < 0) {
        err = mc_socket_errno();
    }

    switch (err) {
    case MC_EINTR:
        lua_pushliteral(L, "interrupted");
        break;
    case MC_EFAULT:
        lua_pushliteral(L, "fault");
        break;
    case MC_EINVAL:
        lua_pushliteral(L, "invalid");
        break;
    case MC_ENOTCONN:
        lua_pushliteral(L, "notconnected");
        break;
    case MC_ESHUTDOWN:
        lua_pushliteral(L, "shutdown");
        break;
    case MC_ENOTSOCK:
        lua_pushliteral(L, "nonsocket");
        break;
    case MC_EADDRINUSE:
        lua_pushliteral(L, "inuse");
        break;
    case MC_ECONNREFUSED:
        lua_pushliteral(L, "refused");
        break;
    case MC_ECONNABORTED:
        lua_pushliteral(L, "aborted");
        break;
    case MC_ECONNRESET:
        lua_pushliteral(L, "reset");
        break;
    case MC_ETIMEDOUT:
        lua_pushliteral(L, "timeout");
        break;
    case MC_EAGAIN:
    case MC_EINPROGRESS:
        lua_pushliteral(L, "again");
        break;
    case MC_EISCONN:
        lua_pushliteral(L, "connected");
        break;
    case MC_EACCES:
        lua_pushliteral(L, "denied");
        break;
    case MC_EALREADY:
        lua_pushliteral(L, "already");
        break;
    default:
        lua_pushliteral(L, "others");
        break;
    }
    return 1;
}

static int mcl_hostname(lua_State *L) {
    lua_pushstring(L, mc_hostname());
    return 1;
}

int luaopen_sevo_socket(lua_State* L) {
    luaL_Reg meta_socket[] = {
        { "__gc", mcl_socket__gc },
        { "getfd", mcl_socket_getfd },
        { "getfamily", mcl_socket_getfamily },
        { "error", mcl_socket_error },
        { "close", mcl_socket_close },
        { "shutdown", mcl_socket_shutdown },
        { "bind", mcl_socket_bind },
        { "listen", mcl_socket_listen },
        { "connect", mcl_socket_connect },
        { "accept", mcl_socket_accept },
        { "getpeername", mcl_socket_getpeername },
        { "getsockname", mcl_socket_getsockname },
        { "send", mcl_socket_send },
        { "sendto", mcl_socket_sendto },
        { "recv", mcl_socket_recv },
        { "recvfrom", mcl_socket_recvfrom },
        { "sendall", mcl_socket_sendall },
        { "recvall", mcl_socket_recvall },
        { "reuseaddr", mcl_socket_reuseaddr },
        { "nodelay", mcl_socket_nodelay },
        { "nonblock", mcl_socket_nonblock },
        { "broadcast", mcl_socket_broadcast },
        { "add_membership", mcl_socket_add_membership },
        { "drop_membership", mcl_socket_drop_membership },
        { "multicast_loop", mcl_socket_multicast_loop },
        { NULL, NULL }
    };
    luaL_Reg mod_socket[] = {
        { "select", mcl_socket_select },
        { "tcp", mcl_socket_tcp },
        { "udp", mcl_socket_udp },
        { "hostname", mcl_hostname },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_socket, meta_socket);
    luaX_register_module(L, "socket", mod_socket);
    return 0;
}
