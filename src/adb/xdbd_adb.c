#include "bfdev/log.h"
#include "connection.h"
#include "xdbd.h"
#include "xdbd_pool.h"
#include <xdbd_adb.h>
#include <string.h>


void xdbd_adb_wait_apacket_handler(xdbd_event_t *ev) {
    bfdev_log_debug("xdbd_adb_handler\n");
    xdbd_connection_t *c;
    size_t n;
    c = ev->data;
    unsigned char buf[1024];

    xdbd_memzero(buf, sizeof(buf));

    n = c->recv(c, buf, sizeof(buf));

    if (n == XDBD_ERR) {
        bfdev_log_err("c->recv error\n");

        return;
    }

    if (n == XDBD_AGAIN) {
        bfdev_log_info("c->recv need again\n");
    }

    if (n == 0) {
        bfdev_log_debug("close connection by client\n");
        xdbd_adb_close_connection(c);
    }

    bfdev_log_info("%s receive size %ld\n", (char *)buf, n);

}

void xdbd_adb_empty_handler(xdbd_event_t *ev) {
    return;
}

void
xdbd_adb_close_connection(xdbd_connection_t *c) {
    xdbd_pool_t *pool;

    pool = c->pool;


    c->destroyed = 1;

    pool = c->pool;

    xdbd_close_connection(c->listening->xdbd, c);
    xdbd_destroy_pool(pool);
}
