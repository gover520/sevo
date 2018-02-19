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

int logger_level(int level) {
    int old = LG_LEVEL;
    if ((level >= LGL_MIN) && (level <= LGL_MAX)) {
        LG_LEVEL = level;
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

    struct timeval tv;
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
    strftime(tm, sizeof(tm), "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
    ms = (int)(tv.tv_usec * 0.001f);

    return fprintf(stdout, "[%s] %s.%03d %c %s\n", p[type], tm, ms, c[level], msg);
}
