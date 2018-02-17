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
        LG_CURRENT = -1,
        LG_MIN,
        LG_DEBUG = LG_MIN,
        LG_INFO,
        LG_WARN,
        LG_ERROR,
        LG_MAX,
        LG_DISABLE = LG_MAX
    };

    void logger_set(int level, const char *filename);
    void logger_flush(void);

    int logger(int level, const char *fmt, ...);
    int vlogger(int level, const char *fmt, va_list argv);

    #define LG_DBG(fmt, ...)    logger(LG_DEBUG, fmt, ##__VA_ARGS__)
    #define LG_INF(fmt, ...)    logger(LG_INFO, fmt, ##__VA_ARGS__)
    #define LG_WRN(fmt, ...)    logger(LG_WARN, fmt, ##__VA_ARGS__)
    #define LG_ERR(fmt, ...)    logger(LG_ERROR, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
};
#endif

#endif  /* __LOGGER_H__ */
