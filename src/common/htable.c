/*
 *  htable.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include <mclib.h>
#include <limits.h>
#include <string.h>
#include "htable.h"

struct datum_t {
    datum_t     *next;
    char        *key;
    char        *value;
    int         ksize;
    int         vsize;
    char        buffer[2];
};

struct htable_t {
    datum_t         **table;
    int             size;
    int             used;
    unsigned int    mask;
};

struct htable_iter_t {
    int         index;
    htable_t    *ht;
    datum_t     *entry;
    datum_t     *next;
};

#define HTABLE_INIT_SIZE    8
#define HTABLE_MAX_SIZE     (INT_MAX >> 1)

#define htable_hash(k, l)   mc_hash32(k, l)

static int htable_compare(const char *k1, int l1, const char *k2, int l2) {
    if (l1 > l2) {
        return 1;
    }
    if (l1 < l2) {
        return -1;
    }
    return memcmp(k1, k2, l1);
}

static void datum_set(datum_t *e, const char *key, int ksize, const char *val, int vsize) {
    e->ksize = ksize;
    e->vsize = vsize;

    e->key = e->buffer;
    e->value = e->buffer + ksize + 1;

    memcpy(e->key, key, e->ksize);
    memcpy(e->value, val, e->vsize);
}

static int datum_new(htable_t *ht, unsigned int h, const char *key, int ksize, const char *val, int vsize) {
    datum_t *e = (datum_t *)mc_calloc(1, sizeof(datum_t) + ksize + vsize);

    datum_set(e, key, ksize, val, vsize);

    e->next = ht->table[h];
    ht->table[h] = e;

    ht->used += 1;

    return 0;
}

static int htable_expand(htable_t *ht, int size) {
    datum_t **t;
    datum_t *e, *n;
    int newsize, newused, i;
    unsigned int newmask, h;

    if (ht->used > size) {
        return -1;
    }

    if (0 == size) {
        size = HTABLE_INIT_SIZE;
    }

    if (size >= HTABLE_MAX_SIZE) {
        size = HTABLE_MAX_SIZE;
    }

    newsize = (int)mc_next_power(size);
    newmask = newsize - 1;
    newused = ht->used;

    t = (datum_t **)mc_calloc(newsize, sizeof(datum_t *));

    if (ht->table) {
        for (i = 0; (i < ht->size) && (ht->used > 0); ++i) {
            e = ht->table[i];

            while (e) {
                n = e->next;

                h = htable_hash(e->key, e->ksize) & newmask;
                e->next = t[h];
                t[h] = e;

                ht->used -= 1;
                e = n;
            }
        }

        mc_free(ht->table);
    }

    ht->table = t;
    ht->used = newused;
    ht->size = newsize;
    ht->mask = newmask;

    return 0;
}

static int htable_expand_if_needed(htable_t *ht) {
    if (ht->used == ht->size) {
        return htable_expand(ht, ht->size << 1);
    }
    return 0;
}

htable_t *htable_create(void) {
    htable_t *ht = (htable_t *)mc_malloc(sizeof(htable_t));

    ht->table = NULL;
    ht->size = 0;
    ht->used = 0;
    ht->mask = 0;

    return ht;
}

void htable_destroy(htable_t *ht) {
    htable_clear(ht);
    mc_free(ht);
}

int htable_clear(htable_t *ht) {
    int i;
    datum_t *e, *n;

    if (ht->table) {
        for (i = 0; (i < ht->size) && (ht->size > 0); ++i) {
            e = ht->table[i];

            while (e) {
                n = e->next;

                mc_free(e);

                ht->used -= 1;
                e = n;
            }
        }

        mc_free(ht->table);
        ht->table = NULL;

        ht->size = 0;
        ht->mask = 0;
    }
    return 0;
}

int htable_slots(htable_t *ht) {
    return ht->size;
}

int htable_size(htable_t *ht) {
    return ht->used;
}

int htable_add(htable_t *ht, const char *key, int ksize, const char *val, int vsize) {
    unsigned int h;
    datum_t *e;

    if (0 != htable_expand_if_needed(ht)) {
        return -1;
    }

    ksize = (ksize > 0) ? ksize : (int)strlen(key);
    vsize = (vsize > 0) ? vsize : (int)strlen(val);

    h = htable_hash(key, ksize) & ht->mask;
    e = ht->table[h];

    while (e) {
        if (0 == htable_compare(key, ksize, e->key, e->ksize)) {
            return -1;  /* the element already exists */
        }
        e = e->next;
    }
    return datum_new(ht, h, key, ksize, val, vsize);
}

int htable_set(htable_t *ht, const char *key, int ksize, const char *val, int vsize) {
    unsigned int h;
    datum_t *e, *p = NULL;

    if (0 != htable_expand_if_needed(ht)) {
        return -1;
    }

    ksize = (ksize > 0) ? ksize : (int)strlen(key);
    vsize = (vsize > 0) ? vsize : (int)strlen(val);

    h = htable_hash(key, ksize) & ht->mask;
    e = ht->table[h];

    while (e) {
        if (0 == htable_compare(key, ksize, e->key, e->ksize)) {
            if (p) {
                p->next = e->next;
            } else {
                ht->table[h] = e->next;
            }

            mc_free(e);
            ht->used -= 1;

            return datum_new(ht, h, key, ksize, val, vsize);
        }
        p = e;
        e = e->next;
    }
    return (0 == datum_new(ht, h, key, ksize, val, vsize)) ? 1 : -1;
}

int htable_erase(htable_t *ht, const char *key, int ksize) {
    unsigned int h;
    datum_t *e, *p = NULL;

    if ((0 == ht->size) || (0 == ht->used)) {
        return -1;
    }

    ksize = (ksize > 0) ? ksize : (int)strlen(key);

    h = htable_hash(key, ksize) & ht->mask;
    e = ht->table[h];

    while (e) {
        if (0 == htable_compare(key, ksize, e->key, e->ksize)) {
            if (p) {
                p->next = e->next;
            } else {
                ht->table[h] = e->next;
            }

            mc_free(e);
            ht->used -= 1;

            return 0;
        }
        p = e;
        e = e->next;
    }
    return -1;
}

const datum_t *htable_get(htable_t *ht, const char *key, int ksize) {
    unsigned int h;
    datum_t *e;

    if ((0 == ht->size) || (0 == ht->used)) {
        return NULL;
    }

    ksize = (ksize > 0) ? ksize : (int)strlen(key);

    h = htable_hash(key, ksize) & ht->mask;
    e = ht->table[h];

    while (e) {
        if (0 == htable_compare(key, ksize, e->key, e->ksize)) {
            return e;
        }
        e = e->next;
    }
    return NULL;
}

htable_iter_t *htable_iter(htable_t *ht) {
    htable_iter_t *iter = (htable_iter_t *)mc_malloc(sizeof(htable_iter_t));

    iter->ht = ht;
    iter->index = -1;
    iter->entry = NULL;
    iter->next = NULL;

    return iter;
}

const datum_t *htable_iter_next(htable_iter_t *iter) {
    while (1) {
        if (!iter->entry) {
            iter->index += 1;

            if (iter->index >= iter->ht->size) {
                break;
            }

            iter->entry = iter->ht->table[iter->index];
        } else {
            iter->entry = iter->next;
        }
        if (iter->entry) {
            iter->next = iter->entry->next;
            return iter->entry;
        }
    }
    return NULL;
}

void htable_iter_destroy(htable_iter_t *iter) {
    mc_free(iter);
}

const char *datum_key(const datum_t *d, int *ksize) {
    if (ksize) {
        *ksize = d->ksize;
    }
    return d->key;
}

const char *datum_value(const datum_t *d, int *vsize) {
    if (vsize) {
        *vsize = d->vsize;
    }
    return d->value;
}
