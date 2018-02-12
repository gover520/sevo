/*
 *  version.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __SONIC_VERSION_H__
#define __SONIC_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

    #define SONIC_MAJOR         0
    #define SONIC_MINOR         1
    #define SONIC_PATCH         0
    #define SONIC_VERSION_NUM   ((SONIC_MAJOR * 100) + SONIC_MINOR)
    #define SONIC_VERSION_FULL  ((SONIC_VERSION_NUM * 1000) + SONIC_PATCH)

    const char *sonic_version();

#ifdef __cplusplus
};
#endif

#endif  /* __SONIC_VERSION_H__ */
