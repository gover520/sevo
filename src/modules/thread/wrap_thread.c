/*
 *  wrap_thread.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "wrap_thread.h"
#include "common/vfs.h"
#include "common/logger.h"
#include "modules/sevo/sevo.h"

#define THREAD_READY    0
#define THREAD_STARTING 1
#define THREAD_RUNNING  2

typedef struct mcl_thread_t {
    int         tid;
    int         status;
    mc_sstr_t   file;
    mc_chan_t   *chan0; /* handle(w) -> thread(r) */
    mc_chan_t   *chan1; /* handle(r) <- thread(w) */
    mc_mutex_t  mutex;
    mc_cond_t   cond;
} mcl_thread_t;

static int      g_thread_inited = 0;
static mc_tls_t g_tls;

static const char g_meta_thread[] = { CODE_NAME "meta.thread" };

#define luaX_checkthread(L, idx)    (mcl_thread_t *)luaL_checkudata(L, idx, g_meta_thread)
#define local_thread()              (mcl_thread_t *)mc_tls_get_value(g_tls)
#define tidx(i)                     (local_thread() ? ((i) - 1) : (i))

#define WAIT_INFINITE   0xFFFFFFFF

static mcl_thread_t *thread_handle(lua_State * L, int index, mc_chan_t **r, mc_chan_t **w) {
    mcl_thread_t *thread = local_thread();

    if (thread) {
        if (r) {
            *r = thread->chan0;
        }
        if (w) {
            *w = thread->chan1;
        }
        return thread;
    }

    thread = luaX_checkthread(L, index);

    if (r) {
        *r = thread->chan1;
    }
    if (w) {
        *w = thread->chan0;
    }
    return thread;
}

int thread_init(void) {
    if (!g_thread_inited) {
        mc_tls_create(&g_tls);
        g_thread_inited = 1;
    }
    return 0;
}

void thread_deinit(void) {
    if (g_thread_inited) {
        mc_tls_destroy(g_tls);
        g_thread_inited = 0;
    }
}

static int mcl_thread__gc(lua_State * L) {
    mcl_thread_t *thread = luaX_checkthread(L, -1);
    if (thread) {
        void *p;

        mc_chan_shutdown(thread->chan0);
        mc_chan_shutdown(thread->chan1);

        while (mc_chan_readable(thread->chan0)) {
            mc_chan_read(thread->chan0, &p);
            mc_sstr_destroy((mc_sstr_t)p);
        }

        while (mc_chan_readable(thread->chan1)) {
            mc_chan_read(thread->chan1, &p);
            mc_sstr_destroy((mc_sstr_t)p);
        }

        mc_chan_destroy(thread->chan0);
        mc_chan_destroy(thread->chan1);
        mc_cond_destroy(&thread->cond);
        mc_mutex_destroy(&thread->mutex);

        mc_sstr_destroy(thread->file);
    }
    return 0;
}

static void thread_worker(void *param) {
    mcl_thread_t *thread = (mcl_thread_t *)param;
    lua_State *L;

    mc_tls_set_value(g_tls, thread);

    mc_mutex_lock(&thread->mutex);

    thread->tid = mc_thread_id();
    thread->status = THREAD_STARTING;

    mc_cond_signal(&thread->cond);
    mc_mutex_unlock(&thread->mutex);

    L = luaL_newstate();
    luaL_openlibs(L);

    luaX_preload(L, CODE_NAME, luaopen_sevo);

    luaX_require(L, CODE_NAME);
    lua_pop(L, 1);

    luaX_require(L, CODE_NAME ".parallel");
    lua_pcall(L, 0, 0, LUA_MULTRET);

    lua_close(L);

    mc_mutex_lock(&thread->mutex);

    thread->status = THREAD_READY;

    mc_cond_signal(&thread->cond);
    mc_mutex_unlock(&thread->mutex);
}

static int mcl_thread_run(lua_State * L) {
    mcl_thread_t *thread = local_thread();
    mc_sstr_t data;

    if (!thread) {
        LG_ERR("Not a thread.");
        return luaL_error(L, "Not a thread.");
    }

    data = vfs_read(thread->file, -1);

    if (data) {
        mc_mutex_lock(&thread->mutex);
        thread->status = THREAD_RUNNING;
        mc_mutex_unlock(&thread->mutex);

        if (0 != luaL_loadbuffer(L, data, mc_sstr_length(data), thread->file)) {
            LG_ERR("%s", lua_tostring(L, -1));
            luaL_error(L, lua_tostring(L, -1));
        }
        lua_call(L, 0, LUA_MULTRET);
    }

    mc_sstr_destroy(data);

    return 1;
}

static int mcl_thread_start(lua_State * L) {
    mcl_thread_t *thread = luaX_checkthread(L, 1);

    if (0 != vfs_info(thread->file, NULL)) {
        return luaL_error(L, "Thread create failed, %s not found.", thread->file);
    }

    if (THREAD_READY != thread->status) {
        lua_pushboolean(L, 0);
        return 1;
    }

    mc_mutex_lock(&thread->mutex);

    if (THREAD_READY == thread->status) {
        mc_thread_t t = { thread_worker, thread };

        mc_thread_exec(&t, 1);
        mc_cond_wait(&thread->cond, &thread->mutex);

        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }

    mc_mutex_unlock(&thread->mutex);

    return 1;
}

static int mcl_thread_join(lua_State * L) {
    mcl_thread_t *thread = luaX_checkthread(L, 1);
    unsigned int timeout = (unsigned int)luaL_optinteger(L, 2, WAIT_INFINITE);

    if (THREAD_READY == thread->status) {
        lua_pushboolean(L, 0);
        return 1;
    }

    mc_mutex_lock(&thread->mutex);

    if (THREAD_READY != thread->status) {
        int retval = mc_cond_timedwait(&thread->cond, &thread->mutex, timeout);
        lua_pushboolean(L, MC_WAIT_OK == retval);
    } else {
        lua_pushboolean(L, 0);
    }

    mc_mutex_unlock(&thread->mutex);

    return 1;
}

static int mcl_thread_status(lua_State * L) {
    mcl_thread_t *thread = luaX_checkthread(L, 1);

    mc_mutex_lock(&thread->mutex);

    switch (thread->status) {
    case THREAD_READY:
        lua_pushstring(L, "ready");
        break;
    case THREAD_STARTING:
        lua_pushstring(L, "starting");
        break;
    case THREAD_RUNNING:
        lua_pushstring(L, "running");
        break;
    default:
        luaL_error(L, "Unknown thread status.");
        break;
    }

    mc_mutex_unlock(&thread->mutex);

    return 1;
}

static int mcl_thread_write(lua_State * L) {
    mc_chan_t *chan_w;
    size_t l = 0;
    mcl_thread_t *thread = thread_handle(L, 1, NULL, &chan_w);
    const char *data = luaL_checklstring(L, tidx(2), &l);
    int len = (int)luaL_optinteger(L, tidx(3), (lua_Integer)l);
    mc_sstr_t buf = mc_sstr_from_buffer(data, len);

    lua_pushboolean(L, 0 == mc_chan_write(chan_w, buf));
    return 1;
}

static int mcl_thread_read(lua_State * L) {
    mc_chan_t *chan_r;
    mcl_thread_t *thread = thread_handle(L, 1, &chan_r, NULL);
    void *buf;

    if (0 == mc_chan_read(chan_r, &buf)) {
        mc_sstr_t data = (mc_sstr_t)buf;
        lua_pushlstring(L, data, mc_sstr_length(data));
        mc_sstr_destroy(data);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mcl_thread_writeable(lua_State * L) {
    mc_chan_t *chan_w;
    mcl_thread_t *thread = thread_handle(L, 1, NULL, &chan_w);

    lua_pushboolean(L, mc_chan_writeable(chan_w));
    return 1;
}

static int mcl_thread_readable(lua_State * L) {
    mc_chan_t *chan_r;
    mcl_thread_t *thread = thread_handle(L, 1, &chan_r, NULL);

    lua_pushboolean(L, mc_chan_readable(chan_r));
    return 1;
}

static int mcl_thread_write_size(lua_State * L) {
    mc_chan_t *chan_w;
    mcl_thread_t *thread = thread_handle(L, 1, NULL, &chan_w);

    lua_pushinteger(L, mc_chan_size(chan_w));
    return 1;
}

static int mcl_thread_read_size(lua_State * L) {
    mc_chan_t *chan_r;
    mcl_thread_t *thread = thread_handle(L, 1, &chan_r, NULL);

    lua_pushinteger(L, mc_chan_size(chan_r));
    return 1;
}

static int mcl_thread_new(lua_State * L) {
    const char *fn = luaL_checkstring(L, 1);
    int qsize = (int)luaL_optinteger(L, 2, 512);
    mcl_thread_t *thread = (mcl_thread_t *)luaX_newuserdata(L, g_meta_thread, sizeof(mcl_thread_t));

    thread->tid = -1;
    thread->status = THREAD_READY;
    thread->file = mc_sstr_format("%s.lua", fn);

    thread->chan0 = mc_chan_create(qsize);
    thread->chan1 = mc_chan_create(qsize);

    mc_mutex_create(&thread->mutex);
    mc_cond_create(&thread->cond);

    return 1;
}

int luaopen_sevo_thread(lua_State* L) {
    luaL_Reg meta_thread[] = {
        { "__gc", mcl_thread__gc },
        { "start", mcl_thread_start },
        { "join", mcl_thread_join },
        { "status", mcl_thread_status },
        { "write", mcl_thread_write },
        { "read", mcl_thread_read },
        { "writeable", mcl_thread_writeable },
        { "readable", mcl_thread_readable },
        { "write_size", mcl_thread_write_size },
        { "read_size", mcl_thread_read_size },
        { NULL, NULL }
    };
    luaL_Reg mod_thread[] = {
        { "new", mcl_thread_new },
        { "run", mcl_thread_run },
        { "write", mcl_thread_write },
        { "read", mcl_thread_read },
        { "writeable", mcl_thread_writeable },
        { "readable", mcl_thread_readable },
        { "write_size", mcl_thread_write_size },
        { "read_size", mcl_thread_read_size },
        { NULL, NULL }
    };

    luaX_register_type(L, g_meta_thread, meta_thread);
    luaX_register_module(L, "thread", mod_thread);
    return 0;
}
