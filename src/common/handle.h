/*
 *  handle.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __HANDLE_H__
#define __HANDLE_H__

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef unsigned int    handle_t;

    int handle_init(void);
    void handle_deinit(void);

    handle_t handle_new(void *res);

    void *handle_ref(handle_t h);
    void *handle_unref(handle_t h);

#ifdef __cplusplus
};
#endif

#endif  /* __HANDLE_H__ */
