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

    MC_ALLOC_CB_TYPE get_allocator();
    void allocator_cleanup(void);

#ifdef __cplusplus
};
#endif

#endif  /* __ALLOCATOR_H__ */
