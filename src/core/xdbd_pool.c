/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
 */

#include <xdbd.h>
#include <bfdev/align.h>
#include <bfdev/bug.h>
#include <string.h>

static inline void *cache_alloc(xdbd_pool_t *pool, size_t size) {
    xdbd_cache_t *cache;
    void *retval;

    bfdev_align_high_adj(size, XDBD_ALIGN_SIZE);

    cache = bfdev_slist_first_entry(&pool->cache, xdbd_cache_t, list);
    if (bfdev_likely(cache)) {
        retval = bfdev_allocpool_alloc(&cache->pool, size, 0);
        if (bfdev_likely(retval))
            return retval;
    }

    cache = bfdev_malloc(NULL, sizeof(*cache));
    if (bfdev_unlikely(!cache))
        return NULL;

    retval = bfdev_malloc(NULL, XDBD_PAGE_SIZE);
    if (bfdev_unlikely(!retval)) {
        bfdev_free(NULL, cache);
        return NULL;
    }

    cache->data = retval;
    bfdev_allocpool_init(&cache->pool, retval, XDBD_PAGE_SIZE);
    bfdev_slist_add(&pool->cache, &cache->list);

    retval = bfdev_allocpool_alloc(&cache->pool, size, 0);
    BFDEV_BUG_ON(!retval);

    return retval;
}

static inline void *block_alloc(xdbd_pool_t *pool, size_t size) {
    xdbd_block_t *block;
    void *retval;

    bfdev_align_high_adj(size, XDBD_PAGE_SIZE);

    block = bfdev_malloc(NULL, sizeof(*block));
    if (bfdev_unlikely(!block))
        return NULL;

    retval = bfdev_malloc(NULL, size);
    if (bfdev_unlikely(!retval)) {
        bfdev_free(NULL, block);
        return NULL;
    }

    block->data = retval;
    bfdev_slist_add(&pool->block, &block->list);

    return retval;
}

void *xdbd_palloc(xdbd_pool_t *pool, size_t size) {
    if (bfdev_unlikely(!size)) {
        bfdev_log_alert("malloc zero !!!\n");
        return NULL;
    }

    if (size < XDBD_PAGE_SIZE)
        return cache_alloc(pool, size);

    return block_alloc(pool, size);
}

void *xdbd_pcalloc(xdbd_pool_t *pool, size_t size) {
    void *p;
    p = xdbd_palloc(pool, size);
    if (p == NULL) {
        return NULL;
    }

    xdbd_memzero(p, size);
    return p;
}

xdbd_pool_t *xdbd_create_pool() {
    xdbd_pool_t *pool;

    pool = bfdev_malloc(NULL, sizeof(*pool));
    if (bfdev_unlikely(!pool))
        return NULL;

    bfdev_slist_head_init(&pool->cache);
    bfdev_slist_head_init(&pool->block);

    return pool;
}

void xdbd_release_pool(xdbd_pool_t *pool) {
    xdbd_cache_t *cache, *tcache;
    xdbd_block_t *block, *tblock;

    bfdev_slist_for_each_entry_safe(cache, tcache, &pool->cache, list) {
        bfdev_free(NULL, cache->data);
        bfdev_free(NULL, cache);
    }

    bfdev_slist_for_each_entry_safe(block, tblock, &pool->block, list) {
        bfdev_free(NULL, block->data);
        bfdev_free(NULL, block);
    }

    bfdev_slist_head_init(&pool->cache);
    bfdev_slist_head_init(&pool->block);
}

void xdbd_destroy_pool(xdbd_pool_t *pool) {
    xdbd_release_pool(pool);
    bfdev_free(NULL, pool);
}
