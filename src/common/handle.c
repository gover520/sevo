/*
 *  handle.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include "handle.h"

static mc_resmap_t *g_resmap = NULL;

int handle_init(void) {
    if (!g_resmap) {
        g_resmap = mc_resmap_create();
    }
    return 0;
}

void handle_deinit(void) {
    if (g_resmap) {
        mc_resmap_destroy(g_resmap);
        g_resmap = NULL;
    }
}

handle_t handle_new(void *res) {
    return g_resmap ? mc_resmap_new(g_resmap, res) : 0;
}

void *handle_ref(handle_t h) {
    return g_resmap ? mc_resmap_ref(g_resmap, h) : NULL;
}

void *handle_unref(handle_t h) {
    return g_resmap ? mc_resmap_unref(g_resmap, h) : NULL;
}
