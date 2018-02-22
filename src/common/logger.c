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

static int LG_LEVEL = LGL_MIN;
static mc_sstr_t LG_BUFFER = NULL;

int logger_init(void) {
    if (LG_BUFFER) {
        return 0;
    }

    LG_BUFFER = mc_sstr_create(2048);

    return 0;
}

void logger_deinit(void) {
    logger_flush();

    if (LG_BUFFER) {
        mc_sstr_destroy(LG_BUFFER);
        LG_BUFFER = NULL;
    }
}

int logger_level(int level) {
    int old = LG_LEVEL;
    if ((level >= LGL_MIN) && (level <= LGL_MAX)) {
        LG_LEVEL = level;
    }
    return old;
}

void logger_flush(void) {
    if (LG_BUFFER) {
        fprintf(stdout, "%s", LG_BUFFER);
        mc_sstr_clear(LG_BUFFER);
    }
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

    struct timeval tv;
    time_t ts;
    char msg[1024];
    char tm[20];
    int ms;

    if ((type < LGT_MIN) || (type >= LGT_MAX)) {
        return -1;
    }

    if ((level < LGL_MIN) || (level >= LGL_MAX)) {
        return -1;
    }

    if (level < LG_LEVEL) {
        return 0;
    }

    vsnprintf(msg, sizeof(msg), fmt, argv);

    gettimeofday(&tv, NULL);

    ts = tv.tv_sec;
    ms = (int)(tv.tv_usec * 0.001f);

    strftime(tm, sizeof(tm), "%Y-%m-%d %H:%M:%S", localtime(&ts));

    if (LG_BUFFER) {
        LG_BUFFER = mc_sstr_cat_format(LG_BUFFER, "[%s] %s.%03d %c %s\n", p[type], tm, ms, c[level], msg);
    } else {
        fprintf(stdout, "[%s] %s.%03d %c %s\n", p[type], tm, ms, c[level], msg);
    }
    
    return 0;
}
