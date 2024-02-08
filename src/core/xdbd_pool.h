#ifndef __XDBD_POOL__H__
#define __XDBD_POOL__H__

#include <bfdev/list.h>

typedef struct {
    void *buf;
    bfdev_list_head_t list;
} xdbd_pool_node_t;

typedef struct {
    bfdev_list_head_t head;
} xdbd_pool_t;

xdbd_pool_t *xdbd_create_pool();
void *xdbd_palloc(xdbd_pool_t *pool, unsigned size);
void xdbd_destroy_pool(xdbd_pool_t *pool);

#endif  /*__XDBD_POOL__H__*/
