/*
 *  version.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "version.h"
#include <stdio.h>

const char *sonic_version() {
    static char _version[16] = { 0 };
    if (!_version[0]) {
        sprintf(_version, "%i.%i.%i", SONIC_MAJOR, SONIC_MINOR, SONIC_PATCH);
    }
    return _version;
}
