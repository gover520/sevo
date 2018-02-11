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

static FILE *LG_STREAM = NULL;
static int LG_LEVEL = LG_MIN;

#define LG_MAXLEN               1024
#define LG_FD                   (LG_STREAM ? LG_STREAM : stdout)

#define LG_TIME_STR(b, s, u)    do { time_t t = (s); int ms = (int)((u) * 0.001);  \
                                    int o = (int)strftime((b), sizeof(b), "%Y-%m-%d %H:%M:%S.", localtime(&t));  \
                                    snprintf((b) + o, sizeof(b) - o, "%03d", ms); } while (0)

void logger_set(int level, const char *filename) {
    FILE *lg_steam = NULL;

    if ((level >= LG_MIN) && (level <= LG_MAX)) {
        LG_LEVEL = level;
    }

    logger_flush();

    if (LG_DISABLE == LG_LEVEL) {
        return;
    }

    if (filename) {
        lg_steam = fopen(filename, "a");
    }

    if (lg_steam != LG_STREAM) {
        FILE *tmp_stream = LG_STREAM;

        LG_STREAM = lg_steam;

        if (tmp_stream) {
            fclose(tmp_stream);
        }
    }
}

void logger_flush(void) {
    fflush(LG_FD);
}

int logger(int level, const char *fmt, ...) {
    int retval;
    va_list ap;

    va_start(ap, fmt);
    retval = vlogger(level, fmt, ap);
    va_end(ap);

    return retval;
}

int vlogger(int level, const char *fmt, va_list argv) {
    static const char c[] = { "#*!?" };

    struct timeval tv;
    char tm[64];
    char msg[LG_MAXLEN];
    int pid, tid;

    if ((level < LG_MIN) || (level >= LG_MAX)) {
        return -1;
    }

    if (level < LG_LEVEL) {
        return -1;
    }

    pid = mc_process_id();
    tid = mc_thread_id();

    gettimeofday(&tv, NULL);
    LG_TIME_STR(tm, tv.tv_sec, tv.tv_usec);

    vsnprintf(msg, sizeof(msg), fmt, argv);

    return fprintf(LG_FD, "%i:%i %s %c %s\n", pid, tid, tm, c[level], msg);
}
