#include <bfdev/array.h>
#include <xdbd.h>
#include <connection.h>
#include <xdbd_event.h>
#include <string.h>

xdbd_listening_t *xdbd_create_listening(xdbd_t *xdbd, struct sockaddr *sockaddr, socklen_t socklen) {
    xdbd_listening_t *ls;
    struct sockaddr  *sa;

    ls = bfdev_array_push(&xdbd->listening, 1);
    if (ls == NULL) {
        return NULL;
    }

    xdbd_memzero(ls, sizeof(xdbd_listening_t));

    xdbd_memcpy(sa, sockaddr, socklen);

    ls->sockaddr = sa;
    ls->socklen = socklen;

    ls->fd = (xdbd_socket_t) -1;
    ls->type = SOCK_STREAM;

    return ls;
}

xdbd_connection_t *xdbd_get_connection(xdbd_t *xdbd, xdbd_socket_t s) {
    xdbd_connection_t *c;
    xdbd_event_t *rev, *wev;

    c = xdbd->free_connections;

    if (c == NULL) {
        //connection pool is over
        return NULL;
    }

    xdbd->free_connections = c->data;
    xdbd->free_connection_n--;

    rev = c->read;
    wev = c->write;

    xdbd_memzero(c, sizeof(xdbd_connection_t));

    c->read = rev;
    c->write = wev;
    c->fd = s;

    xdbd_memzero(rev, sizeof(xdbd_event_t));
    xdbd_memzero(wev, sizeof(xdbd_event_t));

    rev->index = XDBD_INVALID_INDEX;
    wev->index = XDBD_INVALID_INDEX;

    rev->data = c;
    wev->data = c;

    wev->write = 1;
    return c;
}

int xdbd_open_listening_sockets(xdbd_t *xdbd) {
    xdbd_listening_t *ls;
    int i;
    xdbd_socket_t s;
    ls = bfdev_array_data(&xdbd->listening, 0);

    for (i = 0; i < bfdev_array_index(&xdbd->listening); i++) {
        if (ls[i].fd != (xdbd_socket_t) -1) {
            continue;
        }

        s = xdbd_socket(ls[i].sockaddr->sa_family, ls[i].type, 0);

        if (s ==  (xdbd_socket_t) -1) {

            return XDBD_ERR;
        }

        if (bind(s, ls[i].sockaddr, ls[i].socklen) == -1) {
            //FIXME: some check here
            continue;
        }

        if (listen(s, ls[i].backlog) == -1) {
            //FIXME: some check here
            continue;
        }

        ls[i].listen = 1;

        ls[i].fd = s;
    }

    return XDBD_OK;
}
