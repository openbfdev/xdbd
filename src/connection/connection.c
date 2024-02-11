#include <bfdev/log.h>
#include <bfdev/array.h>
#include <xdbd.h>
#include <connection.h>
#include <xdbd_event.h>

/*
FIXME: remove this
*/
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

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

    bfdev_log_debug("xdbd_get_connection get c->fd %d, c %p\n", s, c);
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

void xdbd_free_connection(xdbd_t *xdbd, xdbd_connection_t *c) {
    c->data = xdbd->free_connections;
    xdbd->free_connections = c;
    xdbd->free_connection_n++;
}

void xdbd_close_connection(xdbd_t *xdbd, xdbd_connection_t *c) {
    xdbd_socket_t fd;
    xdbd_err_t err;
    if (c->fd == (xdbd_socket_t) -1) {
        return;
    }

    if (c->read->active) {
        xdbd_del_event(c->read, XDBD_READ_EVENT, XDBD_CLOSE_EVENT);
    }

    if (c->write->active) {
        xdbd_del_event(c->write, XDBD_WRITE_EVENT, XDBD_CLOSE_EVENT);
    }

    c->read->closed = 1;
    c->write->closed = 1;

    bfdev_log_debug("xdbd_close_connection close c->fd %d, c %p\n", c->fd, c);

    xdbd_free_connection(xdbd, c);

    fd = c->fd;
    c->fd = (xdbd_socket_t) -1;

    if (xdbd_close_socket(fd) == -1) {
        err = xdbd_socket_errno;

        bfdev_log_debug("xdbd_close_socket error");
    }
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
            //FIXME: some check here
            return XDBD_ERR;
        }

        if (xdbd_nonblocking(s) == -1) {
            bfdev_log_err("xdbd_nonblocking error\n");

            if (xdbd_close_socket(s) == -1) {
                bfdev_log_err("xdbd_close_socket error");
            }
            return XDBD_ERR;
        }

        if (bind(s, ls[i].sockaddr, ls[i].socklen) == -1) {
            //FIXME: some check here
            bfdev_log_info("bind error\n");
            continue;
        }

        if (listen(s, ls[i].backlog) == -1) {
            //FIXME: some check here
            bfdev_log_info("listen error\n");
            continue;
        }

        ls[i].listen = 1;

        ls[i].fd = s;
    }

    return XDBD_OK;
}

ssize_t xdbd_unix_recv(xdbd_connection_t *c, u_char *buf, size_t size) {
    ssize_t       n;
    xdbd_err_t     err;
    xdbd_event_t  *rev;

    rev = c->read;
    do {
        n = recv(c->fd, buf, size, 0);

        // bfdev_log_debug("recv: fd:%d %z of %uz", c->fd, n, size);

        if (n == 0) {
            rev->ready = 0;
            rev->eof = 1;

            return 0;
        }

        if (n > 0) {
/*
            if ((size_t) n < size
                && !(xdbd_event_flags & XDBD_USE_GREEDY_EVENT))
            {
                rev->ready = 0;
            }
*/
            return n;
        }

        err = xdbd_socket_errno;

        if (err == XDBD_EAGAIN || err == XDBD_EINTR) {
            bfdev_log_debug("recv() not ready");
            n = XDBD_AGAIN;

        } else {
            // n = xdbd_connection_error(c, err, "recv() failed");
            bfdev_log_debug("recv() failed");
            break;
        }

    } while (err == XDBD_EINTR);

    rev->ready = 0;

    if (n == XDBD_ERR) {
        rev->error = 1;
    }

    return n;
}

ssize_t xdbd_unix_send(xdbd_connection_t *c, u_char *buf, size_t size) {
    ssize_t       n;
    xdbd_err_t     err;
    xdbd_event_t  *wev;

    wev = c->write;

    for ( ;; ) {
        n = send(c->fd, buf, size, 0);

        // bfdev_log_debug("send: fd:%d %z of %uz", c->fd, n, size);

        if (n > 0) {
            if (n < (ssize_t) size) {
                wev->ready = 0;
            }

            c->sent += n;

            return n;
        }

        err = xdbd_socket_errno;

        if (n == 0) {
            bfdev_log_err("send() returned zero");
            wev->ready = 0;
            return n;
        }

        if (err == XDBD_EAGAIN || err == XDBD_EINTR) {
            wev->ready = 0;

            bfdev_log_debug("send() not ready");

            if (err == XDBD_EAGAIN) {
                return XDBD_AGAIN;
            }

        } else {
            wev->error = 1;
            // (void) xdbd_connection_error(c, err, "send() failed");
            return XDBD_ERR;
        }
    }
}
