#ifndef __XDBD_ADB_REQUEST__H__
#define __XDBD_ADB_REQUEST__H__

#include "adb.h"
#include "xdbd.h"
#include <xdbd_pool.h>
#include <xdbd_buf.h>
#include <packet.h>

struct xdbd_adb_connection_s {
    xdbd_connection_t *coonection;
    xdbd_pool_t *pool;
};

struct xdbd_adb_request_s {
    xdbd_buf_t *buffer;
    xdbd_buf_t *body;
    xdbd_pool_t *pool;
    xdbd_pool_t *temp_pool;
    xdbd_adb_header_t h;
    xdbd_adb_packet_t *p;
    xdbd_connection_t *coonection;
    xdbd_buf_t *out;
};

xdbd_adb_request_t *xdbd_adb_create_request(xdbd_connection_t *c);

#endif  /*__XDBD_ADB_REQUEST__H__*/
