/*
 *  allocator.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <mclib.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        ALOC_DEFAULT = -1,
        ALOC_TLSF,
        ALOC_MAX
    };

    MC_ALLOC_CB_TYPE get_allocator(int type);
    void allocator_cleanup(void);

#ifdef __cplusplus
};
#endif

#endif  /* __ALLOCATOR_H__ */
