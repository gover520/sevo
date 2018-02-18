/*
 *  version.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

    #define __STR2(x)       #x
    #define __STR(x)        __STR2(x)

    #define VERSION_MAJOR   0
    #define VERSION_MINOR   1
    #define VERSION_PATCH   0
    #define VERSION         (__STR(VERSION_MAJOR) "." __STR(VERSION_MINOR) "." __STR(VERSION_PATCH))
    #define VERSION_NUM     ((VERSION_MAJOR * 100) + VERSION_MINOR)
    #define VERSION_FULL    ((VERSION_NUM * 1000) + VERSION_PATCH)

    #define CODE_NAME       "sevo"

#ifdef __cplusplus
};
#endif

#endif  /* __VERSION_H__ */
