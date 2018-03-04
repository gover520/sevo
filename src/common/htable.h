/*
 *  htable.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __HTABLE_H__
#define __HTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct datum_t          datum_t;
    typedef struct htable_t         htable_t;
    typedef struct htable_iter_t    htable_iter_t;

    htable_t *htable_create(void);
    void htable_destroy(htable_t *ht);
    int htable_clear(htable_t *ht);
    int htable_slots(htable_t *ht);
    int htable_size(htable_t *ht);
    int htable_add(htable_t *ht, const char *key, int ksize, const char *val, int vsize);
    int htable_set(htable_t *ht, const char *key, int ksize, const char *val, int vsize);
    int htable_erase(htable_t *ht, const char *key, int ksize);
    const datum_t *htable_get(htable_t *ht, const char *key, int ksize);

    htable_iter_t *htable_iter(htable_t *ht);
    const datum_t *htable_iter_next(htable_iter_t *iter);
    void htable_iter_destroy(htable_iter_t *iter);

    const char *datum_key(const datum_t *d, int *ksize);
    const char *datum_value(const datum_t *d, int *vsize);

#ifdef __cplusplus
};
#endif

#endif  /* __HTABLE_H__ */
