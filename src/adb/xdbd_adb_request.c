#include "xdbd_pool.h"
#include <xdbd_adb_request.h>
#include <connection.h>

xdbd_adb_request_t *xdbd_adb_create_request(xdbd_connection_t *c) {
    xdbd_adb_request_t *r;

    r = xdbd_pcalloc(c->pool, sizeof(xdbd_adb_request_t));
    if (r == NULL) {
        return NULL;
    }

    r->pool = c->pool;
    r->temp_pool = c->temp_pool;

    r->buffer = c->buffer;

    r->coonection = c;

    return r;
}
