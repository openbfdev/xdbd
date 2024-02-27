/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
 */

#ifndef __XDBD_POOL__H__
#define __XDBD_POOL__H__

#include <stddef.h>
#include <stdint.h>
#include <bfdev/allocator.h>
#include <bfdev/allocpool.h>
#include <bfdev/slist.h>

#ifndef XDBD_PAGE_SIZE
# define XDBD_PAGE_SIZE 4096
#endif

#ifndef XDBD_ALIGN_SIZE
# define XDBD_ALIGN_SIZE BFDEV_BYTES_PER_LONG
#endif

typedef struct xdbd_pool xdbd_pool_t;
typedef struct xdbd_cache xdbd_cache_t;
typedef struct xdbd_block xdbd_block_t;

struct xdbd_pool {
    bfdev_slist_head_t cache;
    bfdev_slist_head_t block;
};

struct xdbd_cache {
    bfdev_slist_head_t list;
    bfdev_allocpool_t pool;
    void *data;
};

struct xdbd_block {
    bfdev_slist_head_t list;
    void *data;
};

extern __bfdev_malloc void *
xdbd_palloc(xdbd_pool_t *pool, size_t size);

extern xdbd_pool_t *
xdbd_create_pool();

extern void
xdbd_release_pool(xdbd_pool_t *pool);

extern void
xdbd_destroy_pool(xdbd_pool_t *pool);

#endif  /*__XDBD_POOL__H__*/
