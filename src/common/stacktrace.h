/*
 *  stacktrace.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __SONIC_STACKTRACE_H__
#define __SONIC_STACKTRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

    int install_stacktrace(const char *outfile);

#ifdef __cplusplus
};
#endif

#endif  /* __SONIC_STACKTRACE_H__ */
