/*
 *  logger.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include <mclib.h>
#include <stdio.h>
#include "logger.h"

typedef struct logger_context_t {
    mc_chan_t   *chan;
    mc_mutex_t  mtx;
    mc_cond_t   cnd;
} logger_context_t;

static int g_level = LGL_MIN;

static void logger_thread(void *param) {
    logger_context_t *ctx = *(logger_context_t **)param;
    mc_sstr_t lmsg;
    mc_fps_t fps;
    void *p;

    if (ctx) {
        mc_fps_init(&fps, NULL, 10);

        while (!mc_chan_is_shutdown(ctx->chan)) {
            mc_fps_wait(&fps, NULL);

            while (mc_chan_readable(ctx->chan)) {
                p = NULL;
                mc_chan_read(ctx->chan, &p);
                if (p) {
                    lmsg = (mc_sstr_t)p;
                    fprintf(stdout, "%s", lmsg);
                    mc_sstr_destroy(lmsg);
                }
            }
        }

        mc_mutex_lock(&ctx->mtx);
        mc_cond_signal(&ctx->cnd);
        mc_mutex_unlock(&ctx->mtx);
    }
}

static logger_context_t *g_ctx = NULL;

int logger_init(void) {
    mc_thread_t t = { logger_thread, &g_ctx };

    if (!g_ctx) {
        g_ctx = (logger_context_t *)mc_malloc(sizeof(logger_context_t));
        g_ctx->chan = mc_chan_create(2048);

        mc_mutex_create(&g_ctx->mtx);
        mc_cond_create(&g_ctx->cnd);

        mc_thread_exec(&t, 1);
    }
    return 0;
}

void logger_deinit(void) {
    if (g_ctx) {
        mc_mutex_lock(&g_ctx->mtx);
        mc_chan_shutdown(g_ctx->chan);
        mc_cond_wait(&g_ctx->cnd, &g_ctx->mtx);
        mc_mutex_unlock(&g_ctx->mtx);

        mc_cond_destroy(&g_ctx->cnd);
        mc_mutex_destroy(&g_ctx->mtx);
        mc_chan_destroy(g_ctx->chan);

        mc_free(g_ctx);
        g_ctx = NULL;
    }
}

int logger_level(int level) {
    int old = g_level;
    if ((level >= LGL_MIN) && (level <= LGL_MAX)) {
        g_level = level;
    }
    return old;
}

int logger(int type, int level, const char *fmt, ...) {
    int retval;
    va_list ap;

    va_start(ap, fmt);
    retval = vlogger(type, level, fmt, ap);
    va_end(ap);

    return retval;
}

int vlogger(int type, int level, const char *fmt, va_list argv) {
    static const char c[] = { "#*!?" };
    static const char *p[] = { " C ", "LUA" };

    mc_sstr_t lmsg;
    struct timeval tv;
    time_t ts;
    char tm[20];
    int ms;

    if (!g_ctx) {
        return -1;
    }

    if ((type < LGT_MIN) || (type >= LGT_MAX)) {
        return -1;
    }

    if ((level < LGL_MIN) || (level >= LGL_MAX)) {
        return -1;
    }

    if (level < g_level) {
        return 0;
    }

    gettimeofday(&tv, NULL);

    ts = tv.tv_sec;
    ms = (int)(tv.tv_usec * 0.001f);

    strftime(tm, sizeof(tm), "%Y-%m-%d %H:%M:%S", localtime(&ts));

    lmsg = mc_sstr_create(128);
    lmsg = mc_sstr_cat_format(lmsg, "[%s] %s.%03d %c ", p[type], tm, ms, c[level]);
    lmsg = mc_sstr_cat_vformat(lmsg, fmt, argv);
    lmsg = mc_sstr_cat_string(lmsg, "\n");

    if (0 != mc_chan_write(g_ctx->chan, lmsg)) {
        mc_sstr_destroy(lmsg);
        return -1;
    }

    return 0;
}
