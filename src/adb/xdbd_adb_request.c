#include "adb.h"
#include "xdbd_adb.h"
#include "xdbd_buf.h"
#include "xdbd_pool.h"
#include <xdbd_adb_request.h>
#include <connection.h>

xdbd_adb_request_t *xdbd_adb_create_request(xdbd_connection_t *c) {
    xdbd_adb_request_t *r;
    xdbd_adb_packet_t *p;

    r = xdbd_pcalloc(c->pool, sizeof(xdbd_adb_request_t));
    if (r == NULL) {
        return NULL;
    }

    r->pool = c->pool;
    r->temp_pool = c->temp_pool;

    r->buffer = c->buffer;

    r->coonection = c;

    p = xdbd_pcalloc(r->pool, sizeof(xdbd_adb_packet_t));
    if (p == NULL) {
        return NULL;
    }

    p->payload = xdbd_create_buf(r->pool, ADB_MAX_PACKET_SIZE);
    if (p->payload == NULL) {
        return NULL;
    }

    r->p = p;
    return r;
}
