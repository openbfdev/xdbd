#define MODULE_NAME "xdbd_adb"
#define bfdev_log_fmt(fmt) MODULE_NAME ": " fmt

#include <bfdev/log.h>
#include <connection.h>
#include <xdbd.h>
#include <xdbd_buf.h>
#include <adb.h>
#include <packet.h>
#include <xdbd_pool.h>
#include <xdbd_adb.h>
#include <string.h>
#include <xdbd_event.h>
#include <xdbd_adb_request.h>

size_t xdbd_adb_read_adb_header(xdbd_adb_request_t *r) {
    ssize_t n;
    xdbd_connection_t *c;
    int rc;
    xdbd_event_t *rev;

    c = r->coonection;
    rev = c->read;

    n = xdbd_buf_size(r->buffer);

    if (n >= sizeof(xdbd_adb_header_t)) {
        return n;
    }

    if (rev->ready) {
        n = c->recv(c, r->buffer->last, r->buffer->end - r->buffer->last);
    }else {
        n = XDBD_AGAIN;
    }

    if (n == XDBD_AGAIN) {
        if (xdbd_handle_read_event(rev, 0) != XDBD_OK) {
            xdbd_adb_close_request(r);
            return XDBD_ERR;
        }
        return XDBD_AGAIN;
    }

    if (n == 0) {
        bfdev_log_debug("close connection by client\n");
    }

    if (n == 0 || n == XDBD_ERR) {
        xdbd_adb_close_request(r);
        return XDBD_ERR;
    }

    r->buffer->last += n;
    return n;
}

int xdbd_adb_parse_adb_header(xdbd_adb_request_t *r, xdbd_buf_t *b) {
    ssize_t n;
    xdbd_adb_packet_t *p;
    n = xdbd_buf_size(b);

    if (n < sizeof(xdbd_adb_header_t)) {
        return XDBD_AGAIN;
    }

    p = r->p;

    xdbd_memcpy(&r->h, b->start, sizeof(xdbd_adb_header_t));
    p->header = r->h;

    r->buffer->pos += sizeof(xdbd_adb_header_t);
    return XDBD_OK;
}

size_t xdbd_adb_read_adb_payload(xdbd_adb_request_t *r) {
    ssize_t n;
    xdbd_connection_t *c;
    int rc;
    xdbd_event_t *rev;

    c = r->coonection;
    rev = c->read;

    n = xdbd_buf_size(r->buffer);

    if (n >= r->h.dlen) {
        return n;
    }

    if (rev->ready) {
        n = c->recv(c, r->buffer->last, r->buffer->end - r->buffer->last);
    }else {
        n = XDBD_AGAIN;
    }

    if (n == XDBD_AGAIN) {
        if (xdbd_handle_read_event(rev, 0) != XDBD_OK) {
            xdbd_adb_close_request(r);
            return XDBD_ERR;
        }
    }

    if (n == 0) {
        bfdev_log_debug("close connection by client\n");
    }

    if (n == 0 || n == XDBD_ERR) {
        xdbd_adb_close_request(r);
        return XDBD_ERR;
    }

    r->buffer->last += n;
    return n;
}

int xdbd_adb_parse_adb_payload(xdbd_adb_request_t *r, xdbd_buf_t *b) {
    xdbd_buf_append_buf(r->p->payload, r->pool, b);

    xdbd_dump_adb_packet(r->pool, r->p);

    //XDBD_OK
    r->buffer->pos += r->p->header.dlen;
    return XDBD_OK;

}

void xdbd_adb_process_adb_payload(xdbd_event_t *rev) {
    xdbd_connection_t *c;
    xdbd_adb_request_t *r;
    int rc;
    size_t n;
    c = rev->data;

    r = c->data;
    rc = XDBD_AGAIN;

    if (rev->timeout) {
        c->timeout = 1;
        xdbd_adb_close_request(r);
        return;
    }

    for (;;) {
        if (rc == XDBD_AGAIN) {
            n = xdbd_adb_read_adb_payload(r);

            if (n == XDBD_AGAIN || n == XDBD_ERR) {
                break;
            }
        }

        rc = xdbd_adb_parse_adb_payload(r, r->buffer);

        if (rc == XDBD_OK) {
            rev->handler = xdbd_adb_block_reading;
            //do something for send packet

            rc = xdbd_process_packet(r, r->p);

            if (rc == XDBD_AGAIN) {
                return;
            }

            if (rc == XDBD_ERR) {
                return;
            }

            if (xdbd_buf_size(r->out) == 0) {
                rev->handler = xdbd_adb_wait_apacket_handler;
                break;
            }

            //TODO: more check.
            n = c->send(c, r->out->pos, xdbd_buf_size(r->out));
            bfdev_log_info("send %ld\n", n);
            if (n == 0) {
                bfdev_log_err("fuck close\n");
            }

            rev->handler = xdbd_adb_wait_apacket_handler;
            break;
        }

        if (rc == XDBD_AGAIN) {
            if (r->buffer->pos == r->buffer->last) {
                //never reach payload size too much
                return;
            }

            break;
        }

        if (rc == XDBD_ERR) {
            xdbd_adb_close_request(r);
            return ;
        }
    }
}

void xdbd_adb_process_adb_header(xdbd_event_t *rev) {
    xdbd_connection_t *c;
    xdbd_adb_request_t *r;
    int rc;
    size_t n;
    c = rev->data;

    r = c->data;
    rc = XDBD_AGAIN;

    if (rev->timeout) {
        c->timeout = 1;
        xdbd_adb_close_request(r);
        return;
    }

    for (;;) {
        if (rc == XDBD_AGAIN) {
            n = xdbd_adb_read_adb_header(r);

            if (n == XDBD_AGAIN || n == XDBD_ERR) {
                break;
            }
        }

        rc = xdbd_adb_parse_adb_header(r, r->buffer);

        if (rc == XDBD_OK) {
            rev->handler = xdbd_adb_process_adb_payload;
            xdbd_adb_process_adb_payload(rev);
            break;
        }

        if (rc == XDBD_AGAIN) {
            break;
        }

        if (rc == XDBD_ERR) {
            xdbd_adb_close_request(r);
            return ;
        }
    }
}

void xdbd_adb_wait_apacket_handler(xdbd_event_t *ev) {
    bfdev_log_debug("xdbd_adb_wait_apacket_handler\n");
    xdbd_connection_t *c;
    size_t n;
    xdbd_buf_t *b;
    size_t size;
    c = ev->data;

    size = ADB_MAX_PACKET_SIZE;
    b = c->buffer;
    if (b == NULL) {
        b = xdbd_create_buf(c->pool, size);
        if (b == NULL) {
            xdbd_adb_close_connection(c);
            return;
        }

        c->buffer = b;
    }

    xdbd_reset_buf(b);

    size = b->end - b->start;
    n = c->recv(c, b->start, size);

    if (n == XDBD_ERR) {
        bfdev_log_err("c->recv error\n");
        xdbd_adb_close_connection(c);
        return;
    }

    if (n == XDBD_AGAIN) {
        bfdev_log_info("c->recv need again\n");

        if (xdbd_handle_read_event(ev, 0) != XDBD_OK) {
            xdbd_adb_close_connection(c);
            return;
        }

        return;
    }

    if (n == 0) {
        bfdev_log_debug("close connection by client\n");
        xdbd_adb_close_connection(c);
        return;
    }

    b->last += n;

    bfdev_log_info("%s receive size %ld\n", (char *)b->start, n);

    c->data = xdbd_adb_create_request(c);
    if (c->data == NULL) {
        xdbd_adb_close_connection(c);
        return;
    }

    ev->handler = xdbd_adb_process_adb_header;
    xdbd_adb_process_adb_header(ev);
}

void xdbd_adb_empty_handler(xdbd_event_t *ev) {
    return;
}


void xdbd_adb_block_reading(xdbd_event_t *ev) {
    xdbd_connection_t *c;
    c = ev->data;

    if (xdbd_event_flags & XDBD_USE_LEVEL_EVENT
    && c->read->active) {

        if (xdbd_del_event(c->read, XDBD_READ_EVENT, 0) != XDBD_OK) {
            //FIXME:
            // ngx_http_close_request(r, 0);
        }
    }
    return;
}

void xdbd_adb_close_connection(xdbd_connection_t *c) {
    xdbd_pool_t *pool, *temp_pool;
    pool = c->pool;


    c->destroyed = 1;

    pool = c->pool;
    temp_pool = c->temp_pool;

    xdbd_close_connection(c->listening->xdbd, c);
    xdbd_destroy_pool(pool);

    xdbd_destroy_pool(temp_pool);
}

void xdbd_adb_close_request(xdbd_adb_request_t *r) {
    xdbd_connection_t *c;

    c = r->coonection;

    xdbd_adb_close_connection(c);
}
