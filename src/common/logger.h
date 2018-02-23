/*
 *  logger.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        LGL_CURRENT = -1,
        LGL_MIN,
        LGL_DEBUG = LGL_MIN,
        LGL_INFO,
        LGL_WARN,
        LGL_ERROR,
        LGL_MAX,
        LGL_DISABLE = LGL_MAX
    };

    enum {
        LGT_MIN,
        LGT_C = LGT_MIN,
        LGT_LUA,
        LGT_MAX
    };

    int logger_init(void);
    void logger_deinit(void);

    int logger_level(int level);

    int logger(int type, int level, const char *fmt, ...);
    int vlogger(int type, int level, const char *fmt, va_list argv);

    #define LG_DBG(fmt, ...)    logger(LGT_C, LGL_DEBUG, fmt, ##__VA_ARGS__)
    #define LG_INF(fmt, ...)    logger(LGT_C, LGL_INFO,  fmt, ##__VA_ARGS__)
    #define LG_WRN(fmt, ...)    logger(LGT_C, LGL_WARN,  fmt, ##__VA_ARGS__)
    #define LG_ERR(fmt, ...)    logger(LGT_C, LGL_ERROR, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
};
#endif

#endif  /* __LOGGER_H__ */
