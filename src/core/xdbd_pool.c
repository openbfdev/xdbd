#include "bfdev/list.h"
#include <bfdev/allocator.h>
#include <xdbd_pool.h>
#include <xdbd.h>

xdbd_pool_t *xdbd_create_pool() {
    xdbd_pool_t *pool = NULL;

    pool = bfdev_malloc(NULL, sizeof(xdbd_pool_t));
    if (pool == NULL) {
        return NULL;
    }

    bfdev_list_head_init(&pool->head);
    return pool;
}

void *xdbd_palloc(xdbd_pool_t *pool, unsigned size) {
    xdbd_pool_node_t *node = NULL;
    node = bfdev_malloc(NULL, sizeof(xdbd_pool_node_t));
    if (node == NULL) {
        return NULL;
    }

    node->buf = bfdev_malloc(NULL, size);
    if (node->buf == NULL) {
        bfdev_free(NULL, node);
        return NULL;
    }

    bfdev_list_add(&pool->head, &node->list);
    return node->buf;
}

void xdbd_destroy_pool(xdbd_pool_t *pool) {

    //FIXME:
    return;
}
